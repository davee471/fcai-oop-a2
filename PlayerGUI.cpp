#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &playButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Adding mute button cuz VS throws a fit if I put it in the above loop
    muteButton.addListener(this);
    addAndMakeVisible(muteButton);

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
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
	playButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
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

    if (button == &restartButton)
    {
        playerAudio.play();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }

    if (button == &playButton)
    {
        playerAudio.play();
    }

    if (button == &muteButton)
    {
        // If muted, store last volume then set it to 0
        if(muteButton.getToggleState()) 
        {
            prevVolume = (float)volumeSlider.getValue();
            volumeSlider.setValue(0.0);
        }

        // If unmuted, set volume back to last value
        else 
        {
            volumeSlider.setValue(prevVolume); 
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
            // dontsendnotification basically doesn't alert the functions that check if the button was clicked
            // and we don't really want that to happen here so

            muteButton.setToggleState(false, juce::dontSendNotification);
        }
    }

}