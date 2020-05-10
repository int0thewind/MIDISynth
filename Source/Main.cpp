/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

#include <memory>
#include "MainComponent.h"

//==============================================================================
class MIDISynthApplication : public JUCEApplication {
public:
    //==============================================================================
    /**
     * Trivial constructor for our application.
     * Nothing to do here.
     */
    MIDISynthApplication() = default;

    /**
     * Application name getter
     * @return the name of the application
     */
    const String getApplicationName() override {
        return ProjectInfo::projectName;
    }

    /**
     * Application Version getter
     * @return the version number of the application
     */
    const String getApplicationVersion() override {
        return ProjectInfo::versionString;
    }

    /**
     * This function manages whether the application allows multiple instances of the application.
     * This application only allows one instance. It returns false at all the time.
     * @return false for always.
     */
    bool moreThanOneInstanceAllowed() override {
        return false;
    }

    //==============================================================================
    /**
     * This method is where you should put your application's initialisation code
     * @param command line arguments to be handled
     */
    void initialise (const String& commandLine) override {
        this->mainWindow = std::make_unique<MainWindow> (this->getApplicationName());
    }

    /**
     * This function is called when the app is shutting down.
     */
    void shutdown() override {
        this->mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================

    /**
     * Helper function to close all the displaying windows.
     * It creates a unique pointer to each instance of the opening windows and push it to a container,
     * so that each instance of window can be closed by itself.
     */
    static void closeAllWindows() {
        std::vector<std::unique_ptr<AlertWindow>> alertWindows;
        std::vector<std::unique_ptr<DialogWindow>> dialogWindows;
        Desktop& desktop = Desktop::getInstance();
        for (int i = 0; i < desktop.getNumComponents(); i++) {
            if (auto alertWindow = dynamic_cast<AlertWindow*> (desktop.getComponent(i))) {
                alertWindows.push_back(std::unique_ptr<AlertWindow>(alertWindow));
            } else if (auto dialogWindow = dynamic_cast<DialogWindow*>(desktop.getComponent(i))) {
                dialogWindows.push_back(std::unique_ptr<DialogWindow>(dialogWindow));
            }
        }
    }

    /**
     * This is called when the app is being asked to quit
     */
    void systemRequestedQuit() override {
        MIDISynthApplication::closeAllWindows();
        MIDISynthApplication::quit();
    }

    /**
     * When another instance of the app is launched while this one is running, this method is invoked,
     * and the commandLine parameter tells you what the other instance's command-line arguments were.
     * As we don't allow a second instance of the application to be started, this function simply does nothing.
     * @param commandLine command line arguments to be handled.
     */
    void anotherInstanceStarted (const String& commandLine) override {
        /* Do nothing */
    }

    //==============================================================================

    /**
     * This class implements the desktop window that contains an instance of our MainComponent class.
     */
    class MainWindow : public DocumentWindow {
    public:
        /**
         * The constructor of our main window.
         * Our window use the native title bar and own the main component we declared.
         * Our window cannot be resized on desktop operating system.
         * @param name the name of the main window.
         */
        explicit MainWindow (const String& name) : DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                .findColour (ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            this->setUsingNativeTitleBar (true);
            this->setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            this->setFullScreen (true);
           #else
            this->setResizable (false, false);
            this->centreWithSize (this->getWidth(), this->getHeight());
           #endif

            this->setVisible (true);
        }

        /**
         * This is called when the user tries to close this window.
         */
        void closeButtonPressed() override {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    /** The main window pointer being hold by the main application. */
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MIDISynthApplication)
