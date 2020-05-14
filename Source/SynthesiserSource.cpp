/*
  ==============================================================================

    SynthesiserSource.cpp
    Created: 10 May 2020 11:03:51am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#include "SynthesiserSource.h"

//// ==============================================================================
//// ElementarySound Class
//// ==============================================================================

bool ElementarySound::appliesToNote(int midiNoteNumber) {
    return true;
}

bool ElementarySound::appliesToChannel(int midiChannel) {
    return true;
}

//// ==============================================================================
//// VoiceSynthesiser Class
//// ==============================================================================

VoiceSynthesiser::VoiceSynthesiser(MidiKeyboardState &state)
    : midiKeyboardState(state) {
    this->synthesiser.addSound(new ElementarySound());
}

void VoiceSynthesiser::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    this->synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void VoiceSynthesiser::releaseResources() {
    // Do nothing!
}

void VoiceSynthesiser::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    if (synthesiser.getNumVoices() == 0) {
        return;
    }
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    this->midiKeyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                               bufferToFill.numSamples, true);
    this->synthesiser.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                                       bufferToFill.startSample, bufferToFill.numSamples);
}

void VoiceSynthesiser::removeVoice(int index) {
    this->synthesiser.removeVoice(index);
}

void VoiceSynthesiser::addVoice(ElementaryVoice *voice) {
    if (this->synthesiser.getNumVoices() >= MAX_VOICES) { return; }
    this->synthesiser.addVoice(voice);
}

void VoiceSynthesiser::removeAllVoices() {
    this->synthesiser.clearVoices();
}

ElementaryVoice* VoiceSynthesiser::getVoice(int index) {
    return dynamic_cast<ElementaryVoice*>(synthesiser.getVoice(index));
}

int VoiceSynthesiser::getTotalNumVoices() {
    return synthesiser.getNumVoices();
}

bool VoiceSynthesiser::shouldUpdateStatus() {
    for (int i = 0; i < synthesiser.getNumVoices(); ++i) {
        if (dynamic_cast<ElementaryVoice *>(synthesiser.getVoice(i))->shouldUpdateStatus()) {
            return true;
        }
    }
    return false;
}

//// ==============================================================================
//// ElementaryVoice Class
//// ==============================================================================

ElementaryVoice::ElementaryVoice(const String& newVoiceType) {
    assert(this->voiceTypes.contains(newVoiceType));
    this->voiceType = newVoiceType;

    voiceSelection.addListener(this);
    voiceSelection.addItemList(voiceTypes, 1);
    voiceSelection.setSelectedItemIndex(voiceTypes.indexOf(this->voiceType));
    addAndMakeVisible(voiceSelection);

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
    tailOnSlider.setRange(0.00001, 1.0);
    tailOnSlider.setValue(1.0);
    tailOnSlider.setSkewFactorFromMidPoint(0.8);
    addAndMakeVisible(tailOnSlider);
    addAndMakeVisible(tailOnLabel);

    tailOffSlider.addListener(this);
    tailOffSlider.setRange(0.0, 0.9999);
    tailOffSlider.setValue(0.0);
    tailOffSlider.setSkewFactorFromMidPoint(0.9);
    addAndMakeVisible(tailOffSlider);
    addAndMakeVisible(tailOffLabel);

    this->setSize(480, 168);
}

bool ElementaryVoice::canPlaySound(SynthesiserSound *sound) {
    return dynamic_cast<ElementarySound*>(sound) != nullptr;
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
//    while (--numSamples >= 0) {
//        float amplitude = dynamics;
//        float currentSample = getCurrentSample(amplitude);
//        for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
//            outputBuffer.addSample(i, startSample, currentSample);
//        }
//        ++angle;
//        ++startSample;
//    }

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
    shouldUpdate = true;
    if (slider == &amplitudeFactorSlider) {
        amplitudeFactor = (float) slider->getValue();
    } else if (slider == &frequencyFactorSlider) {
        frequencyFactor = (float) slider->getValue();
    }
    else if (slider == &tailOnSlider) {
        tailOnFactor = (float) slider->getValue();
    } else if (slider == &tailOffSlider) {
        tailOffFactor = (float) slider->getValue();
    }
}

void ElementaryVoice::comboBoxChanged(ComboBox *comboBoxThatHasChanged) {
    shouldUpdate = true;
    if (comboBoxThatHasChanged == &voiceSelection) {
        voiceType = voiceTypes[comboBoxThatHasChanged->getSelectedItemIndex()];
    }
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

    Rectangle<int> header = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> firstRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> secondRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> thirdRow = globalBound.removeFromTop(24);
    globalBound.removeFromTop(8);
    Rectangle<int> fourthRow = globalBound.removeFromTop(24);

    voiceSelection.setBounds(header.removeFromLeft(120));

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
        return (float) this->angle < (angle.getAngleLimit() / 2.0f) ? amplitude : -amplitude;
    } else if (voiceType == "Sawtooth") {
        return (float) angle * (amplitude / (angle.getAngleLimit() / 2.0f)) - amplitude;
    } else if (voiceType == "Triangle") {
        if ((float) angle <= angle.getAngleLimit() / 2.0f) {
            return (4.0f * amplitude / angle.getAngleLimit()) * (float) angle - amplitude;
        } else {
            return (-4.0f * amplitude / angle.getAngleLimit()) * (float) angle + 3.0f * amplitude;
        }
    }
    // If no matching voiceType return sine wave
    return (float) std::sin((float) angle) * amplitude;
}

String ElementaryVoice::toString() {
    std::stringstream ss;
    ss << voiceType << " wave. ";
    ss << "amp: " << std::fixed << std::setprecision(2) <<  amplitudeFactor << ". ";
    ss << "freq: " << std::fixed << std::setprecision(2) <<  frequencyFactor << ". ";
    ss << "on: " << std::fixed << std::setprecision(2) <<  tailOnFactor << ". ";
    ss << "off: " << std::fixed << std::setprecision(2) <<  tailOffFactor << ".";
    return ss.str();
}

bool ElementaryVoice::shouldUpdateStatus() {
    if (shouldUpdate) {
        shouldUpdate = false;
        return true;
    }
    return false;
}
