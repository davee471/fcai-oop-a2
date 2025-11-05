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
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
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



