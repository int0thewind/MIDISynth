#include "MainComponent.h"

//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

MainComponent::MainComponent() :
    midiKeyboardComponent(midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    audioVisualiserComponent(2),
    audioSource(midiKeyboardState)
{
    // Initialise audioSettings button
    audioSettings.addListener(this);
    addAndMakeVisible(audioSettings);
    // Initialise midiKeyboardComponent
    midiKeyboardComponent.setOctaveForMiddleC(4);
    addAndMakeVisible(midiKeyboardComponent);
    // Initialise tailOn Label
    addAndMakeVisible(tailOnLabel);
    // Initialise tailOnSlider
    tailOnSlider.addListener(this);
    tailOnSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    tailOnSlider.setRange(0.05, 0.95);
    tailOnSlider.setValue(0.95);
    addAndMakeVisible(tailOnSlider);
    // Initialise tailOff Label
    addAndMakeVisible(tailOffLabel);
    // Initialise tailOffSlider
    tailOffSlider.addListener(this);
    tailOffSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    tailOffSlider.setRange(0.05, 0.95);
    tailOffSlider.setValue(0.05);
    addAndMakeVisible(tailOffSlider);
    // Initialise synthesiserList
    addAndMakeVisible(synthesiserList);
    // Initialise synthesiserVoiceAdder
    synthesiserVoiceAdder.addListener(this);
    synthesiserVoiceAdder.addItemList(
            {"Sine", "Square", "Sawtooth", "Triangle", "Custom Sound Font"}, 1);
    synthesiserVoiceAdder.setText("Select the voice to add", dontSendNotification);
    addAndMakeVisible(synthesiserVoiceAdder);
    // Initialise audioVisualiserComponent
    addAndMakeVisible(audioVisualiserComponent);
    // Initialise CPU Usage Label
    addAndMakeVisible(cpuUsageLabel);
    // Initialise CPU Usage
    addAndMakeVisible(cpuUsage);
    // Make sure we set the size of the component at last!
    this->setSize (852,608);
    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio) &&
        !RuntimePermissions::isGranted (RuntimePermissions::recordAudio)) {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                [&] (bool granted) { if (granted) {
                    this->setAudioChannels(2, 2);
                }
        });
    } else {
        this->setAudioChannels(2, 2);
    }
    // Timer setup
    this->startTimer(500);
    // TODO value tree listener initialisation
}

MainComponent::~MainComponent() {
    // This shuts down the audio device and clears the audio source.
    this->shutdownAudio();
}

//// ==============================================================================
//// Audio related function
//// ==============================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {

}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {

    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources() {

}

//// ==============================================================================
//// Graphical interface rendering
//// ==============================================================================

void MainComponent::paint (Graphics& g) {
    g.fillAll(this->getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    Rectangle<int> globalBound = this->getLocalBounds();
    globalBound.removeFromRight(8);
    globalBound.removeFromBottom(8);
    globalBound.removeFromLeft(8);
    globalBound.removeFromTop(8);

    Rectangle<int> leftColumn = globalBound.removeFromLeft(608);
    Rectangle<int> rightColumn = globalBound.removeFromRight(236);

    this->audioSettings.setBounds(
            leftColumn.removeFromTop(24).removeFromLeft(120));
    leftColumn.removeFromTop(8);
    this->midiKeyboardComponent.setBounds(leftColumn.removeFromTop(64));
    leftColumn.removeFromTop(8);
    this->audioVisualiserComponent.setBounds(leftColumn.removeFromTop(456));
    leftColumn.removeFromTop(8);
    this->cpuUsageLabel.setBounds(leftColumn.removeFromLeft(96));
    this->cpuUsage.setBounds(leftColumn.removeFromLeft(72));

    rightColumn.removeFromTop(36);
    Rectangle<int> onSlider = rightColumn.removeFromTop(24);
    this->tailOnLabel.setBounds(onSlider.removeFromLeft(60));
    this->tailOnSlider.setBounds(onSlider);
    rightColumn.removeFromTop(8);
    Rectangle<int> offSlider = rightColumn.removeFromTop(24);
    this->tailOffLabel.setBounds(offSlider.removeFromLeft(60));
    this->tailOffSlider.setBounds(offSlider);
    rightColumn.removeFromTop(12);

    this->synthesiserList.setBounds(rightColumn.removeFromTop(456));
    rightColumn.removeFromTop(8);
    this->synthesiserVoiceAdder.setBounds(rightColumn);
}

//// ==============================================================================
//// Component callback functions.
//// ==============================================================================

void MainComponent::buttonClicked(Button *button) {

}

void MainComponent::sliderValueChanged(Slider *slider) {

}

void MainComponent::comboBoxChanged(ComboBox *comboBoxThatHasChanged) {

}

//// ==============================================================================
//// Value tree listener
//// ==============================================================================

void MainComponent::valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property) {

}

//// ==============================================================================
//// Timer callback
//// ==============================================================================

void MainComponent::timerCallback() {

}

//// ==============================================================================
//// Private functions
//// ==============================================================================

void MainComponent::openAudioSettings() {
    auto audioSettingsPanel = std::make_unique<juce::AudioDeviceSelectorComponent>(this->deviceManager, 0, 2, 0, 2, true, true, true, true);
    audioSettingsPanel->setSize(500, 270);
    juce::DialogWindow::LaunchOptions dialogWindow;
    dialogWindow.useNativeTitleBar = true;
    dialogWindow.resizable = false;
    dialogWindow.dialogTitle = "Audio Settings";
    dialogWindow.dialogBackgroundColour = this->getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
    dialogWindow.content.setOwned(audioSettingsPanel.release());
    dialogWindow.launchAsync();
}
