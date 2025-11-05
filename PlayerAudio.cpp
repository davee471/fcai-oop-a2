#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        currentFile = file;

        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

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

    if (lines.size() < 3) return;

    playlist.clear();
    currentIndex = -1;

    int savedIndex = lines[0].getIntValue();
    double savedPos = lines[1].getDoubleValue();
    float savedGain = lines[2].getFloatValue();

    for (int i = 3; i < lines.size(); ++i)
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
    Mute = !Mute;
    setGain(Mute ? 0.0f : prevVolume);
}

void PlayerAudio::loop()
{
    Loop = !Loop;
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
        stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        title = "";
        currentIndex = -1;
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
}

bool PlayerAudio::shuffleState() const
{
    return Shuffle;
}


