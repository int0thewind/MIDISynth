/*
  ==============================================================================

    SynthesiserSource.h
    Created: 10 May 2020 11:03:51am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include <cmath>

class PeriodicAngle {
public:
    explicit PeriodicAngle(float angleLimit = MathConstants<float>::twoPi,
                           float angleDelta = 0.0f) :
                           angleLimit(angleLimit), currentAngle(0.0f) , angleDelta(angleDelta) {};

    PeriodicAngle(PeriodicAngle& newAngle) {
        currentAngle = newAngle.getCurrentAngle();
        angleDelta = newAngle.getAngleDelta();
    }

    float getAngleDelta() const {
        return angleDelta;
    }

    float getCurrentAngle() const {
        return currentAngle;
    }

    void setCurrentAngle(float newCurrentAngle) {
        currentAngle = newCurrentAngle;
        reduceCurrentAngle();
    }

    void setAngleDelta(float newAngleDelta) {
        if (!checkAngleDelta(newAngleDelta)) {
            std::cerr << "Invalid angleDelta. It should be bigger than the angleLimit. " << std::endl;
            return;
        }
        angleDelta = newAngleDelta;
    }

    float getAngleLimit() const {
        return angleLimit;
    }

    void setAngleLimit(float newAngleLimit) {
        if (!checkAngleLimit(newAngleLimit)) {
            std::cerr << "Invalid angleLimit. It should be bigger than the angleLimit. " << std::endl;
            return;
        }
        angleLimit = newAngleLimit;
        reduceCurrentAngle();
    }

    void resetAll() {
        currentAngle = 0;
        angleDelta = 0;
    }

    explicit operator float() const {
        return currentAngle;
    }

    PeriodicAngle& operator++() {
        this->currentAngle += angleDelta;
        reduceCurrentAngle();
        return *this;
    }

    PeriodicAngle operator++(int i) {
        PeriodicAngle toReturn = *this;
        this->currentAngle += angleDelta * (float) i;
        reduceCurrentAngle();
        return toReturn;
    }

private:
    float angleLimit = MathConstants<float>::twoPi;
    float currentAngle = 0.0f;
    float angleDelta = 0.0f;

    inline void reduceCurrentAngle() {
        while (currentAngle >= angleLimit) {
            currentAngle -= angleLimit;
        }
    }

    inline bool checkAngleDelta(float newAngleDelta) const {
        return newAngleDelta < angleLimit;
    }

    inline bool checkAngleLimit(float newAngleLimit) const {
        return angleDelta < newAngleLimit;
    }
};

/**
 * A data class that determines whether a sound or a channel can be played by the synthesiser
 */
class ElementarySound : public SynthesiserSound {
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

class ElementaryVoice :
public SynthesiserVoice, public Component, public Slider::Listener, public ComboBox::Listener {
public:
//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

    explicit ElementaryVoice(const String& newVoiceType);
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
    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;

//// ==============================================================================
//// Graphical interface rendering
//// ==============================================================================

    void paint(Graphics& g) override;
    void resized() override;

//// ==============================================================================
//// Other helper functions
//// ==============================================================================

    String toString();
    bool shouldUpdateStatus();

private:
    bool shouldUpdate = false;

    const StringArray voiceTypes {"Sine", "Square", "Triangle", "Sawtooth"};
    String voiceType {"Sine"};
    ComboBox voiceSelection {"voiceSelection"};

    const float NYQUIST_FREQUENCY = 44100.0f;

    mutable PeriodicAngle angle;
    float dynamics = 0.0f;
    float tailOn = 0.0f;
    float tailOff = 0.0f;

    float amplitudeFactor = 1.0f;
    Slider amplitudeFactorSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};
    Label amplitudeFactorLabel {"amplitudeFactorLabel", "Amplitude factor:"};

    float frequencyFactor = 1.0f;
    Slider frequencyFactorSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label frequencyFactorLabel {"frequencyFactorLabel", "Frequency factor:"};

    float tailOnFactor = 1.0f;
    Slider tailOnSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label tailOnLabel {"tailOnLabel", "Tail on value:"};

    float tailOffFactor = 0.0f;
    Slider tailOffSlider
        {Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft};;
    Label tailOffLabel {"tailOffLabel", "Tail off value:"};

    inline float getCurrentSample(float amplitude);
};

class VoiceSynthesiser : public AudioSource {
public:
    /**
     * This constructor receives the alias of a midi keyboard state from the main component
     * Also, it sets the synthesiser's voice to our synthesiserAudioSound, which accepts all channels and notes
     * @param state the alias of the midiKeyboardState from the main component.
     */
    explicit VoiceSynthesiser(MidiKeyboardState& state);

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
    void addVoice(ElementaryVoice* voice);

    /**
     * Remove voice by its index.
     * The index is the array index. It starts from 0
     * @param index the index of the voice to remove
     */
    void removeVoice(int index);

    /**
     * Remove all the voices in the internal synthesiser.
     */
    void removeAllVoices();

    /**
     * Get the voice by the index.
     * The index is the array index. It starts from 0
     * @param index the index of the voice.
     * @return a pointer holding the voice
     */
    ElementaryVoice* getVoice(int index);

    /**
     * Get the total number of voices our synthesiser have
     * @return the total number of voices
     */
    int getTotalNumVoices();

    /**
     * Helper function to help updating the Synthesiser List in the MainComponent
     * It's being periodically called by the MainComponent's timer.
     * If one of the voice's status is updated, it will return true.
     * @see MainComponent::timerCallback(), ElementaryVoice::shouldUpdateStatus()
     * @return true if one of our voice's info is being modified
     */
    bool shouldUpdateStatus();

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