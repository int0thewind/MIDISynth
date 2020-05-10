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
    this->synthesiser.addVoice(voice);
}

void SynthesiserAudioSource::removeAllVoices() {
    this->synthesiser.clearVoices();
}
