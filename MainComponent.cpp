#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(800, 400);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();


    juce::AudioBuffer<float> tempBuffer1(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioBuffer<float> tempBuffer2(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);

    juce::AudioSourceChannelInfo info1(&tempBuffer1, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo info2(&tempBuffer2, 0, bufferToFill.numSamples);


    player1.getNextAudioBlock(info1);
    player2.getNextAudioBlock(info2);


    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* channelData1 = tempBuffer1.getReadPointer(channel);
        auto* channelData2 = tempBuffer2.getReadPointer(channel);
        auto* outputData = bufferToFill.buffer->getWritePointer(channel);

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outputData[sample] = channelData1[sample] + channelData2[sample];
        }
    }
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    player1.setBounds(area.removeFromLeft(getWidth() / 2));
    player2.setBounds(area);
}