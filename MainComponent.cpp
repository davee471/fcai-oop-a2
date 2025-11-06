#include "MainComponent.h"

// this is the constructor
// we initialize player1 with id "1" and player2 with id "2"
// this is important for their separate save files
MainComponent::MainComponent()
    : player1("1"), player2("2")
{
    // addandmakevisible is a juce function to show the players on screen
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);

    // setsize is the default size but main cpp will override it
    setSize(800, 400);

    // setaudiochannels tells juce we want 2 output channels (stereo)
    // and 0 input channels
    setAudioChannels(0, 2);
}

// this is the destructor
// shutdownaudio is a juce function to release the audio hardware
MainComponent::~MainComponent()
{
    shutdownAudio();
}

// this function gets called by juce to setup audio
// we must pass this call down to both of our players
// so they can prepare their own audio sources
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// this juce function just fills the background with dark grey
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

// this is the core of the mixer
// it's the main juce audio callback
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // first clear the main output buffer
    // this prevents random noise if no players are playing
    bufferToFill.clearActiveBufferRegion();

    // create two temporary buffers one for each player
    // this is necessary to mix their audio together
    juce::AudioBuffer<float> tempBuffer1(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioBuffer<float> tempBuffer2(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);

    // create channel info objects for the temp buffers
    juce::AudioSourceChannelInfo info1(&tempBuffer1, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo info2(&tempBuffer2, 0, bufferToFill.numSamples);

    // tell each player to fill its own temp buffer with audio
    player1.getNextAudioBlock(info1);
    player2.getNextAudioBlock(info2);

    // now we loop through all the audio channels (left and right)
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        // get a pointer to the audio data for player 1
        auto* channelData1 = tempBuffer1.getReadPointer(channel);
        // get a pointer to the audio data for player 2
        auto* channelData2 = tempBuffer2.getReadPointer(channel);
        // get a pointer to the final output buffer
        auto* outputData = bufferToFill.buffer->getWritePointer(channel);

        // loop through every single audio sample in the block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            // add the audio from player 1 and player 2 together
            // this sum is the final mixed audio
            outputData[sample] = channelData1[sample] + channelData2[sample];
        }
    }
}

// this function gets called by juce to release audio resources
// we must pass this call down to both players
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

// this juce function is called when the window size changes
void MainComponent::resized()
{
    // get the total area of this component
    auto area = getLocalBounds();

    // set a small width for the dividing line
    int splitLineWidth = 2;

    // give player1 the left half of the area
    player1.setBounds(area.removeFromLeft(getWidth() / 2 - splitLineWidth / 2));

    // remove the dividing line area from the middle
    area.removeFromLeft(splitLineWidth);

    // give player2 the remaining area (the right half)
    player2.setBounds(area);
}