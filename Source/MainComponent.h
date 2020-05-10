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

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent :
        public AudioAppComponent,
        public Button::Listener,
        public Slider::Listener,
        public juce::ValueTree::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(Button *button) override;
    void sliderValueChanged(Slider *slider) override;

private:

    TextButton openAudioSettings {"openAudioSettings"};
    MidiKeyboardComponent midiKeyboardComponent;
    MidiKeyboardState midiKeyboardState;
    SynthesiserList synthesiserList;
    Slider tailOnSlider;
    Slider tailOffSlider;

    SynthesiserAudioSource audioSource;

    SynthesiserData synthesiserData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
