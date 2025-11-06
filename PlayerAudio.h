#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::Timer, public juce::ChangeBroadcaster
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
    bool playingState() const;
    bool mutedState() const;
    bool loopState() const;
    void jumpForward10s();
    void jumpBackward10s();
    void saveSession();
    void loadSession();
    void addMarker();
    void jumpToMarker();
    void toStart();
    void toEnd();
    void muteUnmute();
    void loop();
    void timerCallback() override;
    void playPrevious();
    void playNext();
    bool isMarkerSet() const;

    juce::String getTitle() const;

    bool addToPlaylist(const juce::File& file);
    void removeFromPlaylist(int index);
    void playFileAt(int i);
    int getPlaylistSize() const;
    juce::File getFileAt(int i) const;
    int getCurrentIndex() const;

    void shuffle();
    bool shuffleState() const;
    
    void playbackSpeed(double ratio);
    juce::AudioThumbnail& getThumbnail();

    double getPlaybackSpeed() const;

    void setPointA();
    void setPointB();
    void toggleABLoop();
    float getPointA() const;
    float getPointB() const;
    bool abLoopState() const;
    void resetABLoop();

    // This is to clear everything when a song is over or deleted audio from playlist (mainly did this for waveform)
    void clear();

private:
    bool Mute = false;
    bool Loop = false;
    bool Shuffle = false;
    double markerPosition = -1.0;

    bool AB_loop = false;
    float pointA = -1.0;
    float pointB = -1.0;

    juce::String title;

    float prevVolume = 0.5f;
    
        // Storing information from preparetoplay so I can transfer it to the resampled source without losing anything
    int currentSamplesPerBlock = 0; 
    double currentSampleRate = 0.0;

    // Creating the audiothumbnail stuff for waveform
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    juce::AudioFormatManager formatManager;
    std::unique_ptr <juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::ResamplingAudioSource> resampledSource;
    juce::AudioTransportSource transportSource;
    juce::File currentFile;	

    juce::Array<juce::File> playlist;
    int currentIndex = -1;
    
    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};