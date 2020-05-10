/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() {
    // Make sure you set the size of the component after
    // you add any child components.
    this->setSize (800,600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio) &&
        !RuntimePermissions::isGranted (RuntimePermissions::recordAudio)) {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                [&] (bool granted) {
                if (granted) { this->setAudioChannels(2, 2); }
            });
    } else {
        // Specify the number of input and output channels that we want to open
        this->setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent() {
    // This shuts down the audio device and clears the audio source.
    this->shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources() {
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g) {
    g.fillAll (this->getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::buttonClicked(Button *button) {

}

void MainComponent::sliderValueChanged(Slider *slider) {

}
