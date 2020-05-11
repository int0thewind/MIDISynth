/*
  ==============================================================================

    SynthesiserSource.cpp
    Created: 10 May 2020 11:03:51am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#include "SynthesiserSource.h"

//// ==============================================================================
//// SynthesiserAudioSound Class
//// ==============================================================================

bool SynthesiserAudioSound::appliesToNote(int midiNoteNumber) {
    return true;
}

bool SynthesiserAudioSound::appliesToChannel(int midiChannel) {
    return true;
}

//// ==============================================================================
//// SynthesiserAudioSource Class
//// ==============================================================================

SynthesiserAudioSource::SynthesiserAudioSource(MidiKeyboardState &state)
    : midiKeyboardState(state) {
    this->synthesiser.addSound(new SynthesiserAudioSound());
}

void SynthesiserAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    this->synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthesiserAudioSource::releaseResources() {
    // Do nothing!
}

void SynthesiserAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    this->midiKeyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                               bufferToFill.numSamples, true);
    this->synthesiser.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                                 bufferToFill.startSample, bufferToFill.numSamples);
}

void SynthesiserAudioSource::removeVoice(int index) {
    this->synthesiser.removeVoice(index);
}

void SynthesiserAudioSource::addVoice(SynthesiserVoice *voice) {
    if (this->synthesiser.getNumVoices() >= MAX_VOICES) { return; }
    this->synthesiser.addVoice(voice);
}

void SynthesiserAudioSource::removeAllVoices() {
    this->synthesiser.clearVoices();
}

Array<SynthesiserVoice*> SynthesiserAudioSource::getAllVoices() {
    Array<SynthesiserVoice*> allVoices;
    for (int i = 0; i < synthesiser.getNumVoices(); i++) {
        allVoices.add(synthesiser.getVoice(i));
    }
    return allVoices;
}

//// ==============================================================================
//// ElementaryVoice Class
//// ==============================================================================

ElementaryVoice::ElementaryVoice(const String& voiceType) {
    jassert(this->voiceTypes.contains(voiceType));
    this->voiceType = voiceType;

    amplitudeFactorSlider.addListener(this);
    amplitudeFactorSlider.setRange(0.0, 10.0);
    amplitudeFactorSlider.setValue(1.0);
    amplitudeFactorSlider.setSkewFactorFromMidPoint(1.0);
    addAndMakeVisible(amplitudeFactorSlider);
    addAndMakeVisible(amplitudeFactorLabel);

    frequencyFactorSlider.addListener(this);
    frequencyFactorSlider.setRange(0.0, 10.0);
    frequencyFactorSlider.setValue(1.0);
    frequencyFactorSlider.setSkewFactorFromMidPoint(1.0);
    addAndMakeVisible(frequencyFactorSlider);
    addAndMakeVisible(frequencyFactorLabel);

    tailOnSlider.addListener(this);
    tailOnSlider.setRange(0.0, 1.0);
    tailOnSlider.setValue(1.0);
    tailOnSlider.setSkewFactorFromMidPoint(0.8);
    addAndMakeVisible(tailOnSlider);
    addAndMakeVisible(tailOnLabel);

    tailOffSlider.addListener(this);
    tailOffSlider.setRange(0.0, 1.0);
    tailOffSlider.setValue(0.0);
    tailOffSlider.setSkewFactorFromMidPoint(0.2);
    addAndMakeVisible(tailOffSlider);
    addAndMakeVisible(tailOffLabel);

    this->setSize(480, 136);
}

bool ElementaryVoice::canPlaySound(SynthesiserSound *sound) {
    return sound != nullptr;
}

void
ElementaryVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) {
    this->angle.setCurrentAngle(0.0);
    this->dynamics = velocity * this->amplitudeFactor;
    auto frequency = (float) (this->frequencyFactor * MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    // If the frequency is bigger than Nyquist frequency we do not bother setup the angleDelta
    float angleDelta = frequency <= NYQUIST_FREQUENCY ?
            MathConstants<float>::twoPi * frequency / (float) this->getSampleRate() : 0.0f;
    this->angle.setAngleDelta(angleDelta);

    this->tailOff = 0.0f;
    this->tailOn = 0.0f;
}

void ElementaryVoice::stopNote(float velocity, bool allowTailOff) {
    if (allowTailOff) {
        if (this->tailOff == 0.0) {
            this->tailOff = 1.0;
        }
    } else {
        this->clearCurrentNote();
        this->angle.setAngleDelta(0.0);
    }
}

void ElementaryVoice::pitchWheelMoved(int newPitchWheelValue) {
    // No operation
}

void ElementaryVoice::controllerMoved(int controllerNumber, int newControllerValue) {
    // No operation
}

void ElementaryVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    if (this->angle.getAngleDelta() == 0.0 && !voiceTypes.contains(voiceType)) {
        return;
    }
    if (this->tailOff > 0.0) {
        while (--numSamples >= 0) {
            float amplitude = dynamics * tailOn * tailOff;
            float currentSample = getCurrentSample(amplitude);
            for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
                outputBuffer.addSample(i, startSample, currentSample);
            }
            ++angle;
            ++startSample;
            tailOff *= tailOffFactor;
            if (this->tailOff <= 0.005) {
                clearCurrentNote();
                angle.setAngleDelta(0.0);
                break;
            }
        }
    } else {
        while (--numSamples >= 0) {
            if (this->tailOn < 1.0) {
                this->tailOn += tailOnFactor;
            }
            float amplitude = dynamics * tailOn;
            float currentSample = getCurrentSample(amplitude);
            for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
                outputBuffer.addSample(i, startSample, currentSample);
            }
            ++angle;
            ++startSample;
        }
    }
}

void ElementaryVoice::sliderValueChanged(Slider *slider) {

}

void ElementaryVoice::paint(Graphics &g) {
    g.fillAll(this->getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.drawRoundedRectangle(this->getLocalBounds().toFloat(), 6, 2);
}

void ElementaryVoice::resized() {
    Rectangle<int> globalBound = this->getLocalBounds();
    globalBound.removeFromTop(8);
    globalBound.removeFromBottom(8);
    globalBound.removeFromLeft(8);
    globalBound.removeFromRight(8);

    Rectangle<int> firstRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> secondRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> thirdRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> fourthRow = globalBound.removeFromTop(24);

    amplitudeFactorLabel.setBounds(firstRow.removeFromLeft(120));
    amplitudeFactorSlider.setBounds(firstRow);

    frequencyFactorLabel.setBounds(secondRow.removeFromLeft(120));
    frequencyFactorSlider.setBounds(secondRow);

    tailOnLabel.setBounds(thirdRow.removeFromLeft(120));
    tailOnSlider.setBounds(thirdRow);

    tailOffLabel.setBounds(fourthRow.removeFromLeft(120));
    tailOffSlider.setBounds(fourthRow);
}

float ElementaryVoice::getCurrentSample(float amplitude) {
    if (voiceType == "Square") {
        return (float) this->angle < MathConstants<float>::pi ? amplitude : -amplitude;
    } else if (voiceType == "Sawtooth") {
        return -amplitude + (float) angle * (amplitude / MathConstants<float>::pi);
    } else if (voiceType == "Triangle") {
        return (float) this->angle < MathConstants<float>::pi ?
            -amplitude + 2 * (float) angle * (amplitude / MathConstants<float>::pi) :
             amplitude - 2 * ((float) angle - MathConstants<float>::pi) * (amplitude / MathConstants<float>::pi);
    }
    // If no matching voiceType return sine wave
    return (float) std::sin((float) this->angle) * amplitude;
}

String ElementaryVoice::getStringRepresentation() {
    return "Wave type: " + voiceType;
}
