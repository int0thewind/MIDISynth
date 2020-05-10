/*
  ==============================================================================

    SynthesiserSource.cpp
    Created: 10 May 2020 11:03:51am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#include "SynthesiserSource.h"

bool SynthesiserAudioSound::appliesToNote(int midiNoteNumber) {
    return true;
}

bool SynthesiserAudioSound::appliesToChannel(int midiChannel) {
    return true;
}
