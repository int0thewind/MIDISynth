/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthesiserSource.h"

/**
 * This component lives inside our window, and this is where we put all our controls and content.
 * This component manages all the graphical components in our application,
 * listens to all the components action,
 * and also manages the audio activity in our application.
 * It has the ability to update the application-wide values that is shared among all the classes,
 * and has the ability to response to the application-wide value change.
 * It also has a internal timer to update the CPU usage of the current audio device.
 */
class MainComponent :
        public AudioAppComponent,
        public Button::Listener,
        public ComboBox::Listener,
        public juce::Timer,
        public ListBoxModel
{
public:
//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

    /** Constructor of the main component */
    MainComponent();

    /** Destructor of the main component */
    ~MainComponent() override;

//// ==============================================================================
//// Audio related function
//// ==============================================================================

    /**
     * This function will be called when the audio device is started,
     * or when its settings (i.e. sample rate, block size, etc) are changed.
     * @see AudioProcessor::prepareToPlay()
     * @param samplesPerBlockExpected Samples contained in buffers which is passed into the getNextAudioBlock().
     * @param sampleRate the expected sample rate of our synthesiser
     */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /**
     * Render the next audio buffer.
     * This function accepts a reference to the buffer we need to fill.
     * @param bufferToFill the buffer to fill in with expected audio data
     */
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;

    /**
     * Called when the audio source is changing from prepared state to unprepared state.
     * This function is essentially NOP, though we still have to call the synthesiser's releaseResource() formally.
     * @see ElementaryVoiceSynthesiser::releaseResource()
     */
    void releaseResources() override;

//// ==============================================================================
//// Graphical interface rendering
//// ==============================================================================

    /**
     * Called when the main component is initialised
     * @param g the graphics of the main component. User unreachable. Managed by JUCE.
     */
    void paint (Graphics& g) override;

    /**
     * Called when the size of the main component is initialised or resized
     * This is the place where the size of each sub components are set up.
     */
    void resized() override;

//// ==============================================================================
//// Component callback functions.
//// ==============================================================================

    /**
     * Called when a button is clicked
     * @param button the button which is clicked
     */
    void buttonClicked(Button *button) override;

    /**
     * Called when a combo box has its menu changed
     * @param comboBoxThatHasChanged the combo box which is clicked
     */
    void comboBoxChanged(ComboBox *comboBox) override;

//// ==============================================================================
//// Timer callback
//// ==============================================================================

    /**
     * Called periodically to carry out certain task
     * This function periodically updates the CPU usage.
     */
    void timerCallback() override;

//// ==============================================================================
//// List box module
//// ==============================================================================
    /**
     * Return the number of rows that our synthesiser list should have.
     * @return the number of rows that our synthesiser list should have
     */
    int getNumRows() override;

    /**
     * Paint the voice to our list
     * @param rowNumber the index of the voice number to paint on
     * @param g graphics object that is related to the row
     * @param width the width of the line
     * @param height the height of the line
     * @param rowIsSelected whether the row is selected or not
     */
    void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;

private:
    TextButton audioSettings {"Audio Settings"};
    // TODO should have an selector to choose input devices?

    MidiKeyboardComponent midiKeyboardComponent;
    MidiKeyboardState midiKeyboardState;

    ListBox synthesiserList;
    ComboBox synthesiserVoiceAdder;
    TextButton addVoiceButton {"Add Voice"};

    AudioVisualiserComponent audioVisualiserComponent;
    Label cpuUsageLabel {"cpuUsageLabel", "CPU usage:"};
    Label cpuUsage {"cpuUsage", "0.00 %"};

    ElementaryVoiceSynthesiser audioSource;

    inline void openAudioSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
