#include "MainComponent.h"

//// ==============================================================================
//// Constructors and destructors
//// ==============================================================================

MainComponent::MainComponent() :
    midiKeyboardComponent(midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    synthesiserList("synthesiserList", this),
    audioVisualiserComponent(2),
    audioSource(midiKeyboardState)
{
    // Initialise audioSettings button
    audioSettings.addListener(this);
    addAndMakeVisible(audioSettings);
    // Initialise midiKeyboardComponent
    midiKeyboardComponent.setOctaveForMiddleC(4);
    addAndMakeVisible(midiKeyboardComponent);
    // Initialise synthesiserList
    addAndMakeVisible(synthesiserList);
    // Initialise synthesiserVoiceAdder
    synthesiserVoiceAdder.addListener(this);
    synthesiserVoiceAdder.addItemList(
            {"Sine", "Square", "Triangle", "Sawtooth"}, 1);
    synthesiserVoiceAdder.setSelectedId(1, dontSendNotification);
    addAndMakeVisible(synthesiserVoiceAdder);
    // Initialise addVoiceButton
    addVoiceButton.addListener(this);
    addAndMakeVisible(addVoiceButton);
    // Initialise audioVisualiserComponent
    addAndMakeVisible(audioVisualiserComponent);
    // Initialise CPU Usage Label
    addAndMakeVisible(cpuUsageLabel);
    // Initialise CPU Usage
    cpuUsage.setJustificationType(Justification::centredLeft);
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
}

MainComponent::~MainComponent() {
    // This shuts down the audio device and clears the audio source.
    this->shutdownAudio();
    this->audioSource.removeAllVoices();
}

//// ==============================================================================
//// Audio related function
//// ==============================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {
    this->audioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    this->audioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    this->audioSource.releaseResources();
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
    int totalWidth = globalBound.getWidth();

    this->audioSettings.setBounds(
            globalBound.removeFromTop(24).removeFromLeft(120));

    globalBound.removeFromTop(8);
    this->midiKeyboardComponent.setBounds(globalBound.removeFromTop(64));
    globalBound.removeFromTop(8);
    Rectangle<int> lastColumn = globalBound.removeFromBottom(24);
    globalBound.removeFromBottom(8);

    this->cpuUsageLabel.setBounds(lastColumn.removeFromLeft(100));
    this->cpuUsage.setBounds(lastColumn.removeFromLeft(100));
    this->addVoiceButton.setBounds(lastColumn.removeFromRight(120));
    this->synthesiserVoiceAdder.setBounds(
            lastColumn.removeFromRight(
                    (int) totalWidth * (MathConstants<float>::sqrt2 - 1.) - 128));

    Rectangle<int> middleColumn = globalBound;
    this->synthesiserList.setBounds(
            middleColumn.removeFromRight((int) totalWidth * (MathConstants<float>::sqrt2 - 1.)));
    middleColumn.removeFromRight(8);
    this->audioVisualiserComponent.setBounds(middleColumn);
}

//// ==============================================================================
//// Component callback functions.
//// ==============================================================================

void MainComponent::buttonClicked(Button *button) {
    if (button == &this->audioSettings) {
        this->openAudioSettings();
    } else if (button == &this->addVoiceButton) {
        // TODO implement this!
        String voiceType = synthesiserVoiceAdder.getItemText(synthesiserVoiceAdder.getSelectedItemIndex());
        auto* voiceToAdd = new ElementaryVoice(voiceType);
        this->audioSource.addVoice(voiceToAdd);

        synthesiserList.updateContent();
        synthesiserList.repaint();
    }
}

void MainComponent::comboBoxChanged(ComboBox *comboBox) {
    if (comboBox == &this->synthesiserVoiceAdder) {
        // NOP
    }
}

//// ==============================================================================
//// Timer callback
//// ==============================================================================

void MainComponent::timerCallback() {
    // Updates the CPU usage
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << this->deviceManager.getCpuUsage() << " %";
    this->cpuUsage.setText(ss.str(), dontSendNotification);
}

//// ==============================================================================
//// List box module
//// ==============================================================================

int MainComponent::getNumRows() {
    return audioSource.getTotalNumVoices();
}

void MainComponent::paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) {
    if (rowNumber > this->getNumRows()) { return; }
    g.setColour(Colours::white);
    g.setFont(15.0);
    Rectangle<int> textBound = Rectangle<int>(4, 0, width, height);
    g.drawText(audioSource.getVoice(rowNumber)->toString(), textBound, Justification::centredLeft);
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
