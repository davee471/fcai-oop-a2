#pragma once
#include <JuceHeader.h>
#include "PlayerGUI.h"

// this class holds our two playerguis
// it inherits from juce's audioappcomponent to handle audio
class MainComponent : public juce::AudioAppComponent
{
public:
    // constructor
    MainComponent();
    // destructor
    ~MainComponent() override;

    // this juce audio function prepares the audio hardware
    // (like setting sample rate and block size)
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // this juce audio function is the main audio loop
    // this is where we mix the sound from both players
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // this juce audio function cleans up audio resources when stopped
    void releaseResources() override;

    // this juce function is called when the window is resized
    // we use this to split the screen for the two players
    void resized() override;

    // this juce function paints the component (just a dark grey background)
    void paint(juce::Graphics& g) override;

private:
    // these are the two instances of our playergui
    PlayerGUI player1;
    PlayerGUI player2;

    // this is a juce macro to prevent copying and detect memory leaks
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};