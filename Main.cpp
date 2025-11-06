#include <JuceHeader.h>
#include "MainComponent.h"

// this is the main class for the whole application
// it inherits from the juce application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    // this juce function returns the application name
    const juce::String getApplicationName() override { return "Enhanced Audio Player"; }
    // this juce function returns the application version
    const juce::String getApplicationVersion() override { return "6.7"; }

    // this juce function is called when the app starts up
    // it creates the main window for the application
    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    // this juce function is called when the app shuts down
    // it destroys the main window
    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    // this is a private helper class for the main window
    // it inherits from juce's documentwindow class
    class MainWindow : public juce::DocumentWindow
    {
    public:
        // this is the window's constructor
        // it sets the window's name background color and buttons
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            // these juce functions set up the window properties
            // setusingnativetitlebar makes it look like a normal os window
            // setcontentowned creates our maincomponent and puts it inside
            // setresizable allows the user to resize the window
            // setresizelimits sets the minimum width and height
            // centresize moves the window to the middle of the screen
            // setvisible shows the window to the user
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            setResizeLimits(1280, 720, 10000, 10000);
            centreWithSize(1280, 720);
            setVisible(true);
        }

        // this juce function is called when the user clicks the x button
        // it tells the application to quit
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    // this holds the one and only main window
    std::unique_ptr<MainWindow> mainWindow;
};

// this is a juce macro that creates the entry point for the app
// it runs our simpleaudioplayer class
START_JUCE_APPLICATION(SimpleAudioPlayer)