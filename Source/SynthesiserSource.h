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
class SynthesiserAudioSound : public SynthesiserSound {
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

class ElementaryVoice : public SynthesiserVoice, public Component, public Slider::Listener {
public:
//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

    explicit ElementaryVoice(const String& voiceType);
    ~ElementaryVoice() override = default;

//// ==============================================================================
//// Voice rendering methods
//// ==============================================================================

    bool canPlaySound(SynthesiserSound *sound) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

//// ==============================================================================
//// Component callback functions
//// ==============================================================================

    void sliderValueChanged(Slider *slider) override;

//// ==============================================================================
//// Graphical interface rendering
//// ==============================================================================

    void paint(Graphics& g) override;
    void resized() override;

//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

    void showWindowAsync();

private:
    StringArray voiceTypes {"Sine", "Square", "Triangle", "Sawtooth"};
    String voiceType {""};

    double currentAngle = 0.0;
    double angleDelta = 0.0;

    double amplitudeFactor = 1.0;
    Slider amplitudeFactorSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};
    Label amplitudeFactorLabel {"amplitudeFactorLabel", "Amplitude factor:"};

    double frequencyFactor = 1.0;
    Slider frequencyFactorSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label frequencyFactorLabel {"frequencyFactorLabel", "Frequency factor:"};

    double tailOn = 1.0;
    Slider tailOnSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label tailOnLabel {"tailOnLabel", "Tail on value:"};

    double tailOff = 1.0;
    Slider tailOffSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label tailOffLabel {"tailOffLabel", "Tail off value:"};
};

class SynthesiserAudioSource : public AudioSource {
public:
    /**
     * This constructor receives the alias of a midi keyboard state from the main component
     * Also, it sets the synthesiser's voice to our synthesiserAudioSound, which accepts all channels and notes
     * @param state the alias of the midiKeyboardState from the main component.
     */
    explicit SynthesiserAudioSource(MidiKeyboardState& state);

    /**
     * Called when the audio source is changing from unprepared state to prepared state
     * We set the internal synthesiser's sample rate to the expected sample rate.
     * @param samplesPerBlockExpected Samples contained in buffers which is passed into the getNextAudioBlock().
     * @param sampleRate the expected sample rate of our synthesiser
     */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /**
     * Called when the audio source is changing from prepared state to unprepared state.
     * This function is NOP
     */
    void releaseResources() override;

    /**
     * Render the next audio buffer.
     * This function accepts a reference to the buffer we need to fill.
     * @param bufferToFill the buffer to fill in with expected audio data
     */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

    /**
     * Add voice to the internal synthesiser
     * @param voice
     */
    void addVoice(SynthesiserVoice* voice);

    /**
     * Remove voice by its index
     * @todo would the rest of the index automatically update?
     * @param index
     */
    void removeVoice(int index);

    /**
     * Remove all the voices in the internal synthesiser.
     */
    void removeAllVoices();

private:
    const int MAX_VOICES = 8;
    /**
     * Reference of the MIDI Keyboard State
     * The whole application should only accepts one MIDI Keyboard State, which is from the main component
     */
    MidiKeyboardState& midiKeyboardState;

    /**
     * The internal synthesiser that combines different voices.
     */
    Synthesiser synthesiser;
};