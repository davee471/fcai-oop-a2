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

            // Create new reader source
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

        }
    }
    return true;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

float PlayerAudio::getGain() const
{
    return transportSource.getGain();
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

void PlayerAudio::toggle() 
{
    stopPlayToggle = !stopPlayToggle;
}

bool PlayerAudio::toggleState()
{
    return stopPlayToggle;
}

void PlayerAudio::setCurrentPos()
{
    pos = transportSource.getCurrentPosition();
}

double PlayerAudio::getCurrentPos()
{
    return pos;
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
	// Create a file in user's Documents folder called "audio_player_session.txt"
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            .getChildFile("audio_player_session.txt");

     // Save 3 pieces of data separated by newlines:
    // 1. Current audio file path
    // 2. Current playback position (in seconds)
    // 3. Current volume level
    sessionFile.replaceWithText(
        currentFile.getFullPathName() + "\n" +
        juce::String(transportSource.getCurrentPosition()) + "\n" +
        juce::String(transportSource.getGain())
    );
}

void PlayerAudio::loadSession()
{
		// Loads data from text file
    juce::File sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            .getChildFile("audio_player_session.txt");
    
    if (sessionFile.existsAsFile())
    {
        juce::StringArray lines;
        sessionFile.readLines(lines);
        if (lines.size() >= 2)
        {
            juce::File savedFile(lines[0]);
            if (savedFile.existsAsFile()) 
                loadFile(savedFile);
            transportSource.setPosition(lines[1].getDoubleValue());
        }
            transportSource.setGain(lines[2].getDoubleValue());    
      }
}

void PlayerAudio::addMarker()
	{
    markerPosition = transportSource.getCurrentPosition();
	}

void PlayerAudio::jumpToMarker()
	{
    if (markerPosition >= 0)  // If marker is set
        transportSource.setPosition(markerPosition);
	}


void PlayerAudio::playbackSpeed(double ratio)
{
    if(resampledSource)
    {
        resampledSource->setResamplingRatio(ratio);
	}
}

juce::AudioThumbnail& PlayerAudio::getThumbnail()
{
    return thumbnail;
}
