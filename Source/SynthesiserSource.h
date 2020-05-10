/*
  ==============================================================================

    SynthesiserSource.h
    Created: 10 May 2020 11:03:51am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
/**
 * A data class that determines whether a sound or a channel can be played by the synthesiser
 */
class SynthesiserAudioSound : public SynthesiserSound
{
    /**
     * Return whether a note can be played or not.
     * This method accepts all the available note number.
     * @param midiNoteNumber the MIDI note number to check
     * @return true for all note number
     */
    bool appliesToNote (int midiNoteNumber) override;

    /**
     * Return whether a channel can be played or not
     * This method accepts all the available MIDI channel
     * @param midiChannel the MIDI channel to check
     * @return true for all channel
     */
    bool appliesToChannel (int midiChannel) override;
};

class SineWaveVoice : public SynthesiserVoice {
public:
    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
};

class SquareWaveVoice : public SynthesiserVoice {
public:
    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
};

class TriangleWaveVoice : public SynthesiserVoice {
public:
    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
};

class SawtoothWaveVoice : public SynthesiserVoice {
public:
    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
};

class SoundFontVoice : public SynthesiserVoice {
public:
    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
};

class SynthesiserAudioSource : public AudioSource {
public:
    explicit SynthesiserAudioSource(const MidiKeyboardState& state);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void releaseResources() override;

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

    void addVoice();

    void removeVoice();

private:
    MidiKeyboardState& midiKeyboardState;
    Synthesiser synthesiser;
};