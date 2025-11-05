#pragma once // PlayerAudio.h
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void setGain(float gain);
    float getGain() const;
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    bool timefinished() const;
    void toggle();
    bool toggleState();
    void setCurrentPos();
    double getCurrentPos();
    void jumpForward10s();
    void jumpBackward10s();
    void saveSession();
    void loadSession();
    void addMarker();
    void jumpToMarker();

private:
    bool stopPlayToggle = false;
    double pos = 0.0;
    double markerPosition = -1.0;  // -1 means no marker set	

    juce::AudioFormatManager formatManager;
    std::unique_ptr <juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::File currentFile;	
    
    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};