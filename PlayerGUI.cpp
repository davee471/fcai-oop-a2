#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
	: thumbnail(playerAudio.getThumbnail())
{
    // Add buttons
    for (auto* btn : { 
		&loadButton,
		&toStartButton, 
 		&stopPlayButton, 
		&loopButton,
		&muteButton, 
		&toEndButton, 
		&jumpBackButton, 
		&jumpForwardButton, 
		&saveSessionButton,  
		&addMarkerButton, 
		&jumpToMarkerButton,
        &loadSessionButton
				})
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
	


    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Speed slider
    speedSlider.setRange(0.25, 2.0, 0.25);
    speedSlider.setValue(1);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.addListener(this);
	addAndMakeVisible(speedSlider);

    // Tracking last volume before muting
    prevVolume = (float)volumeSlider.getValue();

    // Start listening to waveform changes
	thumbnail.addChangeListener(this);

    // Starting a timer to keep redrawing where the waveform tracker is, can also be used for loop
    startTimerHz(30);

}

void PlayerGUI::resized()
{
    int y = 20;
    int m = 80;
    // Adds the buttons to gui

    loadButton.setBounds(20, y, 100, 40);
    toStartButton.setBounds(140, y, 80, 40);
    stopPlayButton.setBounds(240, y, 80, 40);
    toEndButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    loopButton.setBounds(540, y, 80, 40);
    jumpBackButton.setBounds(640, y, 80, 40);
    jumpForwardButton.setBounds(740, y, 80, 40);
    saveSessionButton.setBounds(20, m, 100, 40);
    loadSessionButton.setBounds(140, m, 100, 40);
    addMarkerButton.setBounds(260, m, 100, 40);
    jumpToMarkerButton.setBounds(380, m, 100, 40);

    volumeSlider.setBounds(20, 150, getWidth() - 40, 30);
	speedSlider.setBounds(getWidth() - 50, 300, 30, 100);
}

PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    juce::Rectangle<int> thumbnailArea(100, 200, getWidth() - 200, getHeight() - 350);

    // Colour the rectangle in black
    g.setColour(juce::Colours::black);
    g.fillRect(thumbnailArea);
	auto& thumbnail = playerAudio.getThumbnail();

    // Colour the waveform in orange
    g.setColour(juce::Colours::orange);
    thumbnail.drawChannel(g, thumbnailArea, 0.0, thumbnail.getTotalLength(), 0 , 0.5f);

	double length = playerAudio.getLength();
    if (length > 0.0)
    {
        double currPosition = playerAudio.getPosition();
        float x = thumbnailArea.getX() + ((float)(currPosition / length) * thumbnailArea.getWidth());

        // Draw a vertical line at the current position
        g.setColour(juce::Colours::red);
        g.drawVerticalLine((int)x, (float)thumbnailArea.getY(), (float)thumbnailArea.getBottom());
	}
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                }
            });
    }

    if (button == &toStartButton)
    {
         playerAudio.setPosition(0.0);
         playerAudio.play();

         if (playerAudio.toggleState())
         {
             playerAudio.toggle();
             stopPlayButton.setButtonText("Stop");
         }

     }

    if (button == &muteButton)
    {
        // Mimics toggle functionality
        Mute = !Mute;

        // If muted, store last volume then set it to 0
        // if (muteButton.getToggleState())
        if (Mute)
        {
            muteButton.setButtonText("Unmute");
            prevVolume = (float)volumeSlider.getValue();
            volumeSlider.setValue(0.0);
        }

        // If unmuted, set volume back to last value
        else
        {
            muteButton.setButtonText("Mute");
            volumeSlider.setValue(prevVolume);
        }
    }

    if (button == &loopButton)
    {
        Loop = !Loop;

        if (Loop)
        {
            loopButton.setButtonText("Loop Off");
        }
        else
        {
            loopButton.setButtonText("Loop");
        }
    }

    if (button == &toEndButton)
    {
        playerAudio.setPosition(playerAudio.getLength());
        if (playerAudio.toggleState())
        {
            playerAudio.toggle();
            stopPlayButton.setButtonText("Play");

        }
    }

    if (button == &stopPlayButton)
    {
        playerAudio.toggle();

        if (playerAudio.toggleState())
        {
            playerAudio.setCurrentPos();
            playerAudio.stop();
            stopPlayButton.setButtonText("Play");
        }
        else
        {
            playerAudio.setPosition(playerAudio.getCurrentPos());
            playerAudio.play();
            stopPlayButton.setButtonText("Stop");
        }
    }

    if (button == &addMarkerButton)
    {
        playerAudio.addMarker();
    }

    if (button == &jumpToMarkerButton)
    {
        playerAudio.jumpToMarker();
    }

    if (button == &jumpBackButton)
    {
        playerAudio.jumpBackward10s();
    }

    if (button == &jumpForwardButton)
    {
        playerAudio.jumpForward10s();
    }

    if (button == &saveSessionButton)
    {
        playerAudio.saveSession();
    }

    if (button == &loadSessionButton)
    {
        playerAudio.loadSession();
        volumeSlider.setValue(playerAudio.getGain());
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{ 
    if (slider == &volumeSlider)
    {
        // Check what value the user set the slider to and put the volume to it
        float newVolume = (float)slider->getValue();
        playerAudio.setGain(newVolume);

        // Then if it's greater than 0, unmute the audio (as in set the toggle to off)
        if (newVolume > 0.0f)
        {
            Mute = false;
        }
    }

    if (slider == &speedSlider)
    {
		float newSpeed = (float)slider->getValue();
		playerAudio.playbackSpeed(newSpeed);
    }

}

void PlayerGUI::timerCallback()
{
    if (Loop && playerAudio.timefinished())
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
    
    repaint();
}

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &playerAudio.getThumbnail())
    {
        // redraw the waveform when 'thumbnail' finally loads
        repaint();
	}
}