#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
// Initializing the thumbnail cache to hold 5 thumbnails and the thumbnail with 512 samples per thumbnail
    : thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
}

// Replaced the next 3 transport sources with the resampled source when it comes to audio processing
// so that if left untouched it'll play at normal rate but if modified it'll automatically play the resampled version

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	// Storing these values so I can pass them to the resampled source when loading a new file
	currentSamplesPerBlock = samplesPerBlockExpected;
	currentSampleRate = sampleRate;

    if (resampledSource)
    {

        resampledSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resampledSource)
    {
		resampledSource->getNextAudioBlock(bufferToFill);
    }
    else
    {
        // Frosty's bonus task tries to add both volumes
        // so I need to clear the buffer when no file is loaded so that the music actually plays
        // if I only load one player and not the other
		bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    if (resampledSource)
    {
		resampledSource->releaseResources();
    }
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        currentFile = file;

        if (auto* reader = formatManager.createReaderFor(file))
        {
            // Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();
            resampledSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely to transport
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            // Create resampled source and attach it to transport so transport reads from the speed modified source
            // Passing the audio source I'm reading from
            // second paramter false makes it so that it doesn't try to delete the source because it already delets itself
            // third one is just telling it I want stereo audio (2 channels)
            resampledSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);

			// Prepare the resampled source with the same info as before that we stored
            if (resampledSource)
            {
                resampledSource->prepareToPlay(currentSamplesPerBlock, currentSampleRate);
            }

            resampledSource->setResamplingRatio(1.0);

            thumbnail.setSource(new juce::FileInputSource(file));

            // start transport
            transportSource.start();

            title = reader->metadataValues["title"];

            if (title.isEmpty()) 
            {
                title = file.getFileNameWithoutExtension();
            }
        }
    }
    return true;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::toStart()
{
    setPosition(0.0);
    play();
}

void PlayerAudio::toEnd()
{
    setPosition(getLength());
}

void PlayerAudio::stop()
{
    if (playingState()) transportSource.stop();
    else play();

	sendChangeMessage();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);

    bool wasMuted = Mute;
    Mute = (gain == 0.0f);

    if (gain > 0.0f)
    {
        prevVolume = gain;
    }

    if (wasMuted != Mute)
    {
        sendChangeMessage();
    }
}

float PlayerAudio::getGain() const
{
    if (Mute) return prevVolume;
    else return transportSource.getGain();
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

bool PlayerAudio::timefinished() const
{
    return transportSource.hasStreamFinished();
}

bool PlayerAudio::playingState() const
{
    return transportSource.isPlaying();
}

bool PlayerAudio::mutedState() const
{
    return Mute;
}

bool PlayerAudio::loopState() const
{
    return Loop;
}

void PlayerAudio::jumpForward10s()
{
    double current = transportSource.getCurrentPosition();
    double length = transportSource.getLengthInSeconds();
    double newPos = current + 10.0;
    if (newPos > length) newPos = length;
    transportSource.setPosition(newPos);
}

void PlayerAudio::jumpBackward10s()
{
    double current = transportSource.getCurrentPosition();
    double newPos = current - 10.0;
    if (newPos < 0) newPos = 0;
    transportSource.setPosition(newPos);
}

void PlayerAudio::saveSession()
{
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session.txt");

    juce::StringArray data;

    data.add(juce::String(currentIndex));
    data.add(juce::String(transportSource.getCurrentPosition()));
    data.add(juce::String(transportSource.getGain()));

    data.add(juce::String(pointA));
    data.add(juce::String(pointB));
    data.add(juce::String(AB_loop ? 1 : 0));
    
    if (resampledSource)
    {
        data.add(juce::String(resampledSource->getResamplingRatio()));
    }

    else
    {
        data.add("1.0");
    }

    for (const auto& file : playlist)
    {
        data.add(file.getFullPathName());
    }

    sessionFile.replaceWithText(data.joinIntoString("\n"));
}

void PlayerAudio::loadSession()
{
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("audio_player_session.txt");

    if (!sessionFile.existsAsFile()) return;

    juce::StringArray lines;
    sessionFile.readLines(lines);

    if (lines.size() < 7) return;

    playlist.clear();
    currentIndex = -1;

    int savedIndex = lines[0].getIntValue();
    double savedPos = lines[1].getDoubleValue();
    float savedGain = lines[2].getFloatValue();

    pointA = lines[3].getFloatValue();
    pointB = lines[4].getFloatValue();
    AB_loop = lines[5].getIntValue() == 1;
    double savedSpeed = lines[6].getDoubleValue();

    for (int i = 7; i < lines.size(); ++i)
    {
        juce::File file(lines[i]);
        if (file.existsAsFile())
        {
            playlist.add(file);
        }
    }

    if (savedIndex >= 0 && savedIndex < playlist.size())
    {
        currentIndex = savedIndex;
        loadFile(playlist[currentIndex]);
        transportSource.setPosition(savedPos);
    }

    transportSource.setGain(savedGain);
    if (!Mute) prevVolume = savedGain;
    playbackSpeed(savedSpeed);
}

bool PlayerAudio::isMarkerSet() const
{
    return (markerPosition >= 0.0);
}

void PlayerAudio::addMarker()
{
    markerPosition = transportSource.getCurrentPosition();
    sendChangeMessage();
}

void PlayerAudio::jumpToMarker()
{
    if (markerPosition >= 0) 
        transportSource.setPosition(markerPosition);
}

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

void PlayerAudio::loop()
{
    Loop = !Loop;

    sendChangeMessage();
}

void PlayerAudio::timerCallback() 
{
    if (timefinished())
    {
        if (Loop)
        {
            setPosition(0.0);
            play();
        }

        else if (Shuffle && playlist.size() > 0)
        {
            int nextIndex = currentIndex;

            if (playlist.size() > 1) 
            {
                while (nextIndex == currentIndex) 
                {
                    nextIndex = juce::Random::getSystemRandom().nextInt(playlist.size());
                }
            }

            else {
                nextIndex = 0;
            }
            playFileAt(nextIndex);
        }

        else if (currentIndex >= 0 && currentIndex < playlist.size() - 1)
        {
            playFileAt(currentIndex + 1);
        }

        else
        {
            stopTimer();
            clear();
        }
    }
}

juce::String PlayerAudio::getTitle() const
{
    return title;
}

bool PlayerAudio::addToPlaylist(const juce::File& file)
{
    if (!file.existsAsFile()) return false;

    if (playlist.contains(file))
    {
        return false; 
    }

    playlist.add(file);
    return true; 
}

void PlayerAudio::removeFromPlaylist(int index)
{
    if (index < 0 || index >= playlist.size()) return;

    if (index == currentIndex)
    {
        clear();
    }

    else if (index < currentIndex)
    {
        currentIndex--;
    }

    playlist.remove(index);
}

void PlayerAudio::playFileAt(int i)
{
    if (i >= 0 && i < playlist.size())
    {
        currentIndex = i;
        loadFile(playlist[i]);
        play();
        sendChangeMessage(); 
        startTimer(200);     
    }
}

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

void PlayerAudio::playPrevious()
{
    if (currentIndex > 0)
    {

        playFileAt(currentIndex - 1);
    }

    else
    {

        toStart();
    }
}

void PlayerAudio::playNext()
{
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

    else if (currentIndex < playlist.size() - 1)
    {
        playFileAt(currentIndex + 1);
    }

    else
    {
        toEnd();
    }
}

void PlayerAudio::shuffle()
{
    Shuffle = !Shuffle;

    sendChangeMessage();
}

bool PlayerAudio::shuffleState() const
{
    return Shuffle;
}

void PlayerAudio::playbackSpeed(double ratio)
{
    if(resampledSource)
    {
        resampledSource->setResamplingRatio(ratio);
	}
}

double PlayerAudio::getPlaybackSpeed() const
{
    if (resampledSource)
        return resampledSource->getResamplingRatio();
    return 1.0; // Default speed
}

juce::AudioThumbnail& PlayerAudio::getThumbnail()
{
    return thumbnail;
}

void PlayerAudio::setPointA()
{
    pointA = getPosition();
    if (pointB >= 0 && pointA > pointB)
    {
        pointB = -1.0;
        AB_loop = false;
    }
}

void PlayerAudio::setPointB()
{
    if (pointA >= 0 && getPosition() > pointA)
    {
        pointB = getPosition();
    }
}

void PlayerAudio::toggleABLoop()
{
    if (pointA >= 0 && pointB > pointA)
    {
        AB_loop = !AB_loop;
    }
    if (!AB_loop)
    {
        resetABLoop();
    }
}

void PlayerAudio::resetABLoop()
{
    AB_loop = false;
    pointA = -1.0;
    pointB = -1.0;
}

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

void PlayerAudio::clear()
{
    stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    resampledSource.reset();
    title = "";
    currentFile = juce::File();
    currentIndex = -1;
    thumbnail.clear();
    markerPosition = -1.0;
    pointA = -1.0;
    pointB = -1.0;
    AB_loop = false;
    sendChangeMessage();
}