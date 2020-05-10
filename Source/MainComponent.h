/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthesiserList.h"
#include "SynthesiserSource.h"
#include "SynthesiserData.h"

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
        public Slider::Listener,
        public ComboBox::Listener,
        public juce::ValueTree::Listener,
        public juce::Timer
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
     * @see SynthesiserAudioSource::releaseResource()
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
     * Called when a slider has been modified
     * @param slider the slider which has been modified
     */
    void sliderValueChanged(Slider *slider) override;

    /**
     * Called when a combo box has its menu changed
     * @param comboBoxThatHasChanged the combo box which is clicked
     */
    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;

//// ==============================================================================
//// Value tree listener
//// ==============================================================================
    /**
     * Called when the value tree we listen to has a property change
     * @param treeWhosePropertyHasChanged the value tree that has the property changed
     * @param property the property which is changed
     */
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;

//// ==============================================================================
//// Timer callback
//// ==============================================================================
    /**
     * Called periodically to carry out certain task
     */
    void timerCallback() override;

private:
    TextButton audioSettings {"Audio Settings"};
    // TODO should have an selector to choose input devices?

    MidiKeyboardComponent midiKeyboardComponent;
    MidiKeyboardState midiKeyboardState;

    Label tailOnLabel {"tailOn", "Tail on:"};
    Slider tailOnSlider {"tailOnSlider"};
    Label tailOffLabel {"tailOff", "Tail off:"};
    Slider tailOffSlider {"tailOffSlider"};

    SynthesiserList synthesiserList;
    ComboBox synthesiserVoiceAdder;

    AudioVisualiserComponent audioVisualiserComponent;
    Label cpuUsageLabel {"cpuUsageLabel", "CPU usage:"};
    Label cpuUsage {"cpuUsage", "0.00 %"};

    SynthesiserAudioSource audioSource;
    SynthesiserData synthesiserData;

    inline void openAudioSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
