#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
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

    // Tracking last volume before muting
    prevVolume = (float)volumeSlider.getValue();

}

void PlayerGUI::resized()
{
    int y = 20;
    // Adds the buttons to gui

    loadButton.setBounds(20, y, 100, 40);
    toStartButton.setBounds(140, y, 80, 40);
    stopPlayButton.setBounds(240, y, 80, 40);
    toEndButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    loopButton.setBounds(540, y, 80, 40);
    jumpBackButton.setBounds(640, y, 80, 40);
    jumpForwardButton.setBounds(740, y, 80, 40);
    saveSessionButton.setBounds(840, y, 100, 40);
    loadSessionButton.setBounds(960, y, 100, 40);
    addMarkerButton.setBounds(1080, y, 100, 40);
    jumpToMarkerButton.setBounds(1200, y, 100, 40);
  
    
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
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

    else if (button == &toStartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
        if (playerAudio.toggleState())
        {
            playerAudio.toggle();
            stopPlayButton.setButtonText("Stop");

        }
    }

    else if (button == &muteButton)
    {
        // Mimics toggle functionality
        Mute = !Mute;

        // If muted, store last volume then set it to 0
        if (muteButton.getToggleState())
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

    else if (button == &loopButton)
    {
        Loop = !Loop;

        if (Loop)
        {
            loopButton.setButtonText("Loop Off");
            startTimer(200); // call timercallback every 20ms
        }
        else
        {
            loopButton.setButtonText("Loop");
            stopTimer();
        }
    }

    else if (button == &toEndButton)
    {
        playerAudio.setPosition(playerAudio.getLength());
        if (playerAudio.toggleState())
        {
            playerAudio.toggle();
            stopPlayButton.setButtonText("Play");

        }
    }

    else if (button == &stopPlayButton)
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

    else if (button == &addMarkerButton)
    {
        playerAudio.addMarker();
    }

    else if (button == &jumpToMarkerButton)
    {
        playerAudio.jumpToMarker();
    }

    else if (button == &jumpBackButton)
    {
        playerAudio.jumpBackward10s();
    }

    else if (button == &jumpForwardButton)
    {
        playerAudio.jumpForward10s();
    }

    else if (button == &saveSessionButton)
    {
        playerAudio.saveSession();
    }

    else if (button == &loadSessionButton)
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

}

void PlayerGUI::timerCallback()
{
    if (Loop && playerAudio.timefinished())
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
}