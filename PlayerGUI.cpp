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
        &loadSessionButton,
        & set_A_pos,
        & set_B_pos,
        & set_AB_loop,

				})
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
	

    // Volume slider
    volumeSlider.setRange(0.0, 1, 0.01);
    volumeSlider.setValue(5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    //Time line slider  
    timelineslider.setRange(0.0, 1.0, 0.01);
    timelineslider.setValue(0.0);
    timelineslider.addListener(this);
    timelineslider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(timelineslider);
    //Time lable    
    timeLabel.setText("00:00:00/00:00:00", juce::dontSendNotification);

    timeLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(timeLabel);
    startTimer(100);
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
    y += 50;
    set_AB_loop.setBounds(20, y, 100, 40);
    set_A_pos.setBounds(140, y, 80, 40);
    set_B_pos.setBounds(240, y, 80, 40);
    
    
  
    
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/
    y += 50;
    volumeSlider.setBounds(20, 120, getWidth() - 40, 30);
    timelineslider.setBounds(150, 170, getWidth() - 190, 30);//new
    timeLabel.setBounds(0, 170, 120, 25);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
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
            
                    double length = playerAudio.getLength();
                    timelineslider.setRange(0.0, length, 0.1);
                    timeLabel.setText("00:00 / " + formatTime(length), juce::dontSendNotification);
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
    if (button == &set_A_pos)
    {
        pointA = playerAudio.getPosition();
        set_A_pos.setButtonText("A: " + formatTime(pointA));

    }
    if (button == &set_B_pos)
    {
        pointB = playerAudio.getPosition();
        if (pointA >= 0 && pointB <= pointA)
        {
            set_B_pos.setButtonText("set B");
        }
        else
        {
            set_B_pos.setButtonText("B: " + formatTime(pointB));

        }
    }
    if (button == &set_AB_loop)
    {
        if (pointA >= 0 && pointB > pointA)
        {
            AB_loop = !AB_loop;

        }
        if (AB_loop)

        {
            set_AB_loop.setButtonText("end_AB_loop");
        }
        else {
            set_AB_loop.setButtonText("A=>B_loop");
            pointA = -1.0;
            pointB = -1.0;
            AB_loop = false;
            set_A_pos.setButtonText("set A");
            set_B_pos.setButtonText("set B");
        }
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
    else if (slider == &timelineslider)
    {
       
        if (!slider->isMouseButtonDown())
        {
            return; 

            playerAudio.setPosition(slider->getValue());
        }
        else
            playerAudio.setPosition((float)slider->getValue());
    }


}

void PlayerGUI::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();
    timelineslider.setValue(currentPos, juce::dontSendNotification);
    juce::String timeText = formatTime(currentPos) + " / " + formatTime(totalLength);
    timeLabel.setText(timeText, juce::dontSendNotification);
    if (AB_loop && pointA >= 0 && pointA < pointB && pointB < currentPos)
    {
        playerAudio.setPosition(pointA);
        playerAudio.play();
    }


    if (Loop && playerAudio.timefinished())
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
}
juce::String PlayerGUI::formatTime(double seconds)
{
    int hours = (int)seconds / 3600;
    int mins = ((int)seconds % 3600) / 60;
    int secs = (int)seconds % 60;
    if (hours > 0)
        return juce::String::formatted("%02d:%02d:%02d", hours, mins, secs);
    else
        return juce::String::formatted("%02d:%02d", mins, secs);
  

}