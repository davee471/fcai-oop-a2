#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

// this is the constructor
// it initializes the player id
// it initializes the thumbnail cache (5 files) and thumbnail object
PlayerAudio::PlayerAudio(juce::String id) : playerID(id), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)

{
    // tell the juce format manager to understand basic formats (wav mp3)
    formatManager.registerBasicFormats();
}

// this is the destructor (nothing special to do here)
PlayerAudio::~PlayerAudio()
{
}

// this is called by maincomponent's preparetoplay
// we store these values so we can pass them to a new file
// when it's loaded
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // storing these values so i can pass them to the resampled source
    currentSamplesPerBlock = samplesPerBlockExpected;
    currentSampleRate = sampleRate;

    // if the resampled source exists (a file is loaded)
    // pass the call to it
    if (resampledSource)
    {
        resampledSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

// this is the main audio callback for this player
// it's called by maincomponent's getnextaudioblock
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // if the resampled source exists (a file is loaded)
    if (resampledSource)
    {
        // get the next block of audio from it
        resampledSource->getNextAudioBlock(bufferToFill);
    }
    else
    {
        // if no file is loaded (resampled source is null)
        // we must clear the buffer
        // this prevents silent noise from messing up the mix
        bufferToFill.clearActiveBufferRegion();
    }
}

// pass this call down to the resampled source
// (called by maincomponent)
void PlayerAudio::releaseResources()
{
    if (resampledSource)
    {
        resampledSource->releaseResources();
    }
}

// this function loads a new file into the player
bool PlayerAudio::loadFile(const juce::File& file)
{
    // check if the file is valid
    if (file.existsAsFile())
    {
        // store the file path
        currentFile = file;

        // ask the juce format manager to create a reader for this file
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // stop any current playback
            // disconnect the old sources
            // this is important to prevent crashes
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();
            resampledSource.reset();

            // create a new juce reader source from the file reader
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // connect the new reader source to the transport source
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            // create the juce resampled source (for speed control)
            // we pass it our transport source to read from
            // false means it won't try to delete the transport source
            // 2 means we want stereo audio
            resampledSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);

            // prepare the new resampled source using our stored values
            if (resampledSource)
            {
                resampledSource->prepareToPlay(currentSamplesPerBlock, currentSampleRate);
            }

            // set the default speed to 1.0 (normal)
            resampledSource->setResamplingRatio(1.0);

            // tell the thumbnail to load this file's waveform
            thumbnail.setSource(new juce::FileInputSource(file));

            // start transport (this just means the transport is ready)
            transportSource.start();


            // set default metadata values
            title = file.getFileNameWithoutExtension();
            artist = "Unknown Artist";
            album = "Unknown Album";
            year = ""; 

            // getting file path as a string
            juce::String path = file.getFullPathName();

            // making taglib FileRef object
            TagLib::FileRef f(path.toWideCharPointer());

            // check if the file and tags are valid
            if (!f.isNull() && f.tag())
            {
                TagLib::Tag* tag = f.tag();

                // get metadata, chek if empty then convert to string
                if (!tag->title().isEmpty())
                {
                    title = tag->title().toCString(true);
                }
                    
                if (!tag->artist().isEmpty())
                {
                    artist = tag->artist().toCString(true);
                }
                    
                if (!tag->album().isEmpty())
                {
                    album = tag->album().toCString(true);
                }
                   
                if (tag->year() != 0)
                {
                    year = juce::String(tag->year());
                }
                    
            }
        }
    }
    return true;
}

// just tells the juce transport source to start playing
void PlayerAudio::play()
{
    transportSource.start();
}

// go to the beginning of the track (position 0)
// start playing
void PlayerAudio::toStart()
{
    setPosition(0.0);
    play();
}

// go to the end of the track
void PlayerAudio::toEnd()
{
    setPosition(getLength());
}

// this is a toggle function for play/pause
// if it's playing then stop it
// otherwise play it
void PlayerAudio::stop()
{
    if (playingState()) transportSource.stop();
    else play();

    // tell the gui that the state has changed (to update button icon)
    // this is a juce changebroadcaster function
    sendChangeMessage();
}

// sets the volume using the juce transport source
void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);

    // check if the mute state *changed*
    bool wasMuted = Mute;
    // muted is true if gain is exactly 0
    Mute = (gain == 0.0f);

    // if we are not muted store this as the last "on" volume
    if (gain > 0.0f)
    {
        prevVolume = gain;
    }

    // if the mute state changed tell the gui
    if (wasMuted != Mute)
    {
        sendChangeMessage();
    }
}

// if muted return the volume we had *before* muting
// otherwise return the current volume
float PlayerAudio::getGain() const
{
    if (Mute) return prevVolume;
    else return transportSource.getGain();
}

// sets the playhead position using the juce transport source
void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

// gets the current playhead position in seconds
double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

// gets the total song length in seconds
double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

// checks if the playhead is at the end
bool PlayerAudio::timefinished() const
{
    return transportSource.hasStreamFinished();
}

// checks if the transport is currently playing
bool PlayerAudio::playingState() const
{
    return transportSource.isPlaying();
}

// returns our mute bool
bool PlayerAudio::mutedState() const
{
    return Mute;
}

// returns our loop bool
bool PlayerAudio::loopState() const
{
    return Loop;
}

// moves the playhead forward 10 seconds
// checks to make sure we don't go past the end
void PlayerAudio::jumpForward10s()
{
    double current = transportSource.getCurrentPosition();
    double length = transportSource.getLengthInSeconds();
    double newPos = current + 10.0;
    if (newPos > length) newPos = length;
    transportSource.setPosition(newPos);
}

// moves the playhead back 10 seconds
// checks to make sure we don't go past the beginning
void PlayerAudio::jumpBackward10s()
{
    double current = transportSource.getCurrentPosition();
    double newPos = current - 10.0;
    if (newPos < 0) newPos = 0;
    transportSource.setPosition(newPos);
}

// this function saves all the player's data to a text file
void PlayerAudio::saveSession()
{
    // create a unique file name using the playerid ("1" or "2")
    // uses juce's file class to find the documents directory
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session_" + playerID + ".txt");

    // create a juce stringarray to hold our data as strings
    juce::StringArray data;

    // add all the state variables
    // (current song index position volume)
    data.add(juce::String(currentIndex));
    data.add(juce::String(transportSource.getCurrentPosition()));
    data.add(juce::String(transportSource.getGain()));

    // (a-b loop points and state)
    data.add(juce::String(pointA));
    data.add(juce::String(pointB));
    data.add(juce::String(AB_loop ? 1 : 0));

    // (playback speed)
    if (resampledSource)
    {
        data.add(juce::String(resampledSource->getResamplingRatio()));
    }
    else
    {
        data.add("1.0");
    }

    // loop through the playlist and add every file path
    for (const auto& file : playlist)
    {
        data.add(file.getFullPathName());
    }

    // write all the data to the text file separated by newlines
    sessionFile.replaceWithText(data.joinIntoString("\n"));
}

// this function loads all the player's data from a text file
void PlayerAudio::loadSession()
{
    // find the session file using our playerid
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session_" + playerID + ".txt");

    // if it doesn't exist just stop
    if (!sessionFile.existsAsFile()) return;

    // read all lines from the file into a juce stringarray
    juce::StringArray lines;
    sessionFile.readLines(lines);

    // if the file is old or corrupt (too short) stop
    if (lines.size() < 7) return;

    // clear the current playlist
    playlist.clear();
    currentIndex = -1;

    // read all the state variables from the first lines
    // (index position gain a-b loop speed)
    int savedIndex = lines[0].getIntValue();
    double savedPos = lines[1].getDoubleValue();
    float savedGain = lines[2].getFloatValue();

    pointA = lines[3].getFloatValue();
    pointB = lines[4].getFloatValue();
    AB_loop = lines[5].getIntValue() == 1;
    double savedSpeed = lines[6].getDoubleValue();

    // loop through the *rest* of the lines (the file paths)
    for (int i = 7; i < lines.size(); ++i)
    {
        // add each valid file to our playlist
        juce::File file(lines[i]);
        if (file.existsAsFile())
        {
            playlist.add(file);
        }
    }

    // if the saved index is valid load that file
    // and set its position
    if (savedIndex >= 0 && savedIndex < playlist.size())
    {
        currentIndex = savedIndex;
        loadFile(playlist[currentIndex]);
        transportSource.setPosition(savedPos);
    }

    // set the gain and playback speed
    transportSource.setGain(savedGain);
    if (!Mute) prevVolume = savedGain;
    playbackSpeed(savedSpeed);
}

// simple check for the marker (if it's -1.0 it's not set)
bool PlayerAudio::isMarkerSet() const
{
    return (markerPosition >= 0.0);
}

// set the marker to the current time
// tell the gui to update (to show the marker time)
void PlayerAudio::addMarker()
{
    markerPosition = transportSource.getCurrentPosition();
    sendChangeMessage();
}

// move playhead to the marker if it's set
void PlayerAudio::jumpToMarker()
{
    if (markerPosition >= 0)
        transportSource.setPosition(markerPosition);
}

// toggle function for mute
// if we are currently muted set gain to previous volume
// otherwise set gain to 0
void PlayerAudio::muteUnmute()
{
    if (Mute)
    {
        setGain(prevVolume);
    }
    else
    {
        setGain(0.0f);
    }
}

// toggle the loop bool
// tell the gui to update (to change button icon)
void PlayerAudio::loop()
{
    Loop = !Loop;

    sendChangeMessage();
}

// this is called by our internal juce timer (from starttimer)
void PlayerAudio::timerCallback()
{
    // check if the song has finished
    if (timefinished())
    {
        // if loop is on just restart the song
        if (Loop)
        {
            setPosition(0.0);
            play();
        }
        // if shuffle is on pick a new random song
        // (this is a simple shuffle that can repeat)
        else if (Shuffle && playlist.size() > 0)
        {
            int nextIndex = currentIndex;

            if (playlist.size() > 1)
            {
                // keep picking until we get a different song
                while (nextIndex == currentIndex)
                {
                    // use juce's random class
                    nextIndex = juce::Random::getSystemRandom().nextInt(playlist.size());
                }
            }
            else
            {
                // if only one song just play it
                nextIndex = 0;
            }
            playFileAt(nextIndex);
        }
        // if no loop or shuffle just play the next song in the list
        else if (currentIndex >= 0 && currentIndex < playlist.size() - 1)
        {
            playFileAt(currentIndex + 1);
        }
        // if we're at the end of the list
        else
        {
            // stop the timer
            stopTimer();
            // clear the player (resets waveform etc)
            clear();
        }
    }
}

// metadata getters
juce::String PlayerAudio::getTitle() const
{
    return title;
}

juce::String PlayerAudio::getArtist() const
{
    return artist;
}

juce::String PlayerAudio::getAlbum() const
{
    return album;
}

juce::String PlayerAudio::getYear() const
{
    return year;
}

// adds a file to the playlist
bool PlayerAudio::addToPlaylist(const juce::File& file)
{
    if (!file.existsAsFile()) return false;

    // check if it's a duplicate
    // using juce array's "contains" function
    if (playlist.contains(file))
    {
        // if it is a duplicate return false
        return false;
    }

    // otherwise add it and return true
    playlist.add(file);
    return true;
}

// removes a song from the list by its index
void PlayerAudio::removeFromPlaylist(int index)
{
    if (index < 0 || index >= playlist.size()) return;

    // if we are deleting the song that is currently playing
    if (index == currentIndex)
    {
        // clear the player completely
        clear();
    }
    // if we are deleting a song *before* the current one
    else if (index < currentIndex)
    {
        // we need to move the current index down by one
        // so it still points to the same song
        currentIndex--;
    }

    // finally remove the file from the juce array
    playlist.remove(index);
}

// this is called by the gui (double-click or load)
void PlayerAudio::playFileAt(int i)
{
    if (i >= 0 && i < playlist.size())
    {
        // sets the current index
        currentIndex = i;
        // loads the file
        loadFile(playlist[i]);
        // plays the file
        play();
        // tells the gui to update (metadata buttons etc)
        sendChangeMessage();
        // starts the timer (for autoplay check)
        startTimer(200);
    }
}

// simple "getter" functions for the gui
int PlayerAudio::getPlaylistSize() const
{
    return playlist.size();
}

juce::File PlayerAudio::getFileAt(int i) const
{
    return playlist[i];
}

int PlayerAudio::getCurrentIndex() const
{
    return currentIndex;
}

// plays the previous song in the list
void PlayerAudio::playPrevious()
{
    // if we're not at the first song
    if (currentIndex > 0)
    {
        // play the one before it
        playFileAt(currentIndex - 1);
    }
    else
    {
        // otherwise just restart the current song
        toStart();
    }
}

// plays the next song
void PlayerAudio::playNext()
{
    // (this is the simple shuffle)
    if (Shuffle && playlist.size() > 0)
    {
        int nextIndex = currentIndex;
        if (playlist.size() > 1)
        {
            while (nextIndex == currentIndex)
            {
                nextIndex = juce::Random::getSystemRandom().nextInt(playlist.size());
            }
        }
        else
        {
            nextIndex = 0;
        }
        playFileAt(nextIndex);
    }
    // if shuffle is off play the next song in the list
    else if (currentIndex < playlist.size() - 1)
    {
        playFileAt(currentIndex + 1);
    }
    // if we're at the end just go to the end position
    else
    {
        toEnd();
    }
}

// toggle the shuffle bool
// tell the gui to update the button icon
void PlayerAudio::shuffle()
{
    Shuffle = !Shuffle;

    sendChangeMessage();
}

// returns our shuffle bool
bool PlayerAudio::shuffleState() const
{
    return Shuffle;
}

// sets the resampling ratio (the speed)
void PlayerAudio::playbackSpeed(double ratio)
{
    if (resampledSource)
    {
        resampledSource->setResamplingRatio(ratio);
    }
}

// gets the current playback speed
double PlayerAudio::getPlaybackSpeed() const
{
    if (resampledSource)
        return resampledSource->getResamplingRatio();
    return 1.0; // default speed
}

// returns a reference to the thumbnail object for the gui
juce::AudioThumbnail& PlayerAudio::getThumbnail()
{
    return thumbnail;
}

// these are the logic functions for the a-b loop
// they manage the pointa pointb and ab_loop variables

// sets point a to the current time
// if b was set before a reset it
void PlayerAudio::setPointA()
{
    pointA = getPosition();
    if (pointB >= 0 && pointA > pointB)
    {
        pointB = -1.0;
        AB_loop = false;
    }
}

// sets point b to the current time
// only if it's after point a
void PlayerAudio::setPointB()
{
    if (pointA >= 0 && getPosition() > pointA)
    {
        pointB = getPosition();
    }
}

// toggles the a-b loop on or off
// only works if both a and b are set correctly
void PlayerAudio::toggleABLoop()
{
    if (pointA >= 0 && pointB > pointA)
    {
        AB_loop = !AB_loop;
    }
    // if we just turned it off reset the points
    if (!AB_loop)
    {
        resetABLoop();
    }
}

// helper function to reset all a-b variables
void PlayerAudio::resetABLoop()
{
    AB_loop = false;
    pointA = -1.0;
    pointB = -1.0;
}

// "getter" functions for the a-b loop state
float PlayerAudio::getPointA() const
{
    return pointA;
}

float PlayerAudio::getPointB() const
{
    return pointB;
}

bool PlayerAudio::abLoopState() const
{
    return AB_loop;
}

// returns the marker time
double PlayerAudio::getMarkerPosition() const
{
    return markerPosition;
}

// this is a full reset for the player
void PlayerAudio::clear()
{
    // stops audio
    stop();
    // disconnects all juce audio sources
    transportSource.setSource(nullptr);
    readerSource.reset();
    resampledSource.reset();
    // resets all variables to default
    title = "";
    artist = "";
    album = "";
    year = "";
    currentFile = juce::File();
    currentIndex = -1;
    // clears the juce thumbnail waveform
    thumbnail.clear();
    markerPosition = -1.0;
    pointA = -1.0;
    pointB = -1.0;
    AB_loop = false;
    // tells the gui to update (to show "no file loaded")
    sendChangeMessage();
}