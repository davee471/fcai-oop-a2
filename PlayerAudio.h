#pragma once
#include <JuceHeader.h>

// this is the main audio engine class
// it inherits from juce's timer to run its own clock for autoplay
// it inherits from juce's changebroadcaster to send updates to the gui
class PlayerAudio : public juce::Timer, public juce::ChangeBroadcaster
{
public:
    // constructor takes a player id (like "1" or "2") for save files
    PlayerAudio(juce::String id);
    // destructor
    ~PlayerAudio();

    // standard juce audio functions
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    // loads a file from disk into the transport source
    bool loadFile(const juce::File& file);

    // these are all the transport controls
    // (play pause volume position etc)
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

    // functions to save and load the playlist and player state
    void saveSession();
    void loadSession();

    // functions for the marker feature
    void addMarker();
    void jumpToMarker();
    void toStart();
    void toEnd();
    void muteUnmute();
    void loop();

    // this is called by our internal juce timer
    // we use it to check if a song has finished (for autoplay)
    void timerCallback() override;
    void playPrevious();
    void playNext();
    bool isMarkerSet() const;

    // gets the song title
    juce::String getTitle() const;

    // all functions for managing the playlist
    bool addToPlaylist(const juce::File& file);
    void removeFromPlaylist(int index);
    void playFileAt(int i);
    int getPlaylistSize() const;
    juce::File getFileAt(int i) const;
    int getCurrentIndex() const;

    // functions for the shuffle toggle
    void shuffle();
    bool shuffleState() const;

    // functions to control the speed (using a resampler)
    void playbackSpeed(double ratio);

    // gives the gui access to the waveform thumbnail
    juce::AudioThumbnail& getThumbnail();
    double getPlaybackSpeed() const;

    // all functions for the a-b loop feature
    void setPointA();
    void setPointB();
    void toggleABLoop();
    float getPointA() const;
    float getPointB() const;
    bool abLoopState() const;
    void resetABLoop();

    // gets the time of the marker for the gui
    double getMarkerPosition() const;

    // this is to clear everything when a song is over or deleted
    void clear();

private:

    // playerid for save files ("1" or "2")
    juce::String playerID;

    // bools to track the player's state (mute loop shuffle)
    bool Mute = false;
    bool Loop = false;
    bool Shuffle = false;
    double markerPosition = -1.0;

    // floats and bool for a-b loop state
    bool AB_loop = false;
    float pointA = -1.0;
    float pointB = -1.0;

    // string to hold the song title (from metadata or file name)
    juce::String title;

    // float to remember volume before muting
    float prevVolume = 0.5f;

    // storing information from preparetoplay for the resampled source
    int currentSamplesPerBlock = 0;
    double currentSampleRate = 0.0;

    // variables for the waveform thumbnail
    // these are all juce classes
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    // standard juce audio pipeline objects
    juce::AudioFormatManager formatManager;
    std::unique_ptr <juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::ResamplingAudioSource> resampledSource;
    juce::AudioTransportSource transportSource;
    juce::File currentFile;

    // variables for the playlist
    // juce's array class to hold the files
    juce::Array<juce::File> playlist;
    // int to track the currently playing song index
    int currentIndex = -1;

    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};