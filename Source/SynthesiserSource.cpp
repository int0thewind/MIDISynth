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

//// ==============================================================================
//// ElementaryVoice Class
//// ==============================================================================

ElementaryVoice::ElementaryVoice(const String& voiceType) {
    jassert(this->voiceTypes.contains(voiceType));
    this->voiceType = voiceType;
}

bool ElementaryVoice::canPlaySound(SynthesiserSound *sound) {
    return sound != nullptr;
}

void
ElementaryVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) {

}

void ElementaryVoice::stopNote(float velocity, bool allowTailOff) {

}

void ElementaryVoice::pitchWheelMoved(int newPitchWheelValue) {

}

void ElementaryVoice::controllerMoved(int controllerNumber, int newControllerValue) {

}

void ElementaryVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {

}

void ElementaryVoice::sliderValueChanged(Slider *slider) {

}

void ElementaryVoice::paint(Graphics &g) {
    g.fillAll(this->getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void ElementaryVoice::resized() {
    Component::resized();
}

void ElementaryVoice::showWindowAsync() {

}
