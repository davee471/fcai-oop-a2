#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &jumpBackButton, &jumpForwardButton,
            &saveSessionButton, &addMarkerButton, &jumpToMarkerButton,
            &loadSessionButton, &loadFolderButton, &prevButton,
            &nextButton, &playPauseButton, &loopButton, &muteButton,
            &deleteButton, &shuffleButton, & set_A_pos, & set_B_pos,
            & set_AB_loop
        })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
	
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(metadataLabel);

    // Volume slider
    volumeSlider.setRange(0.0, 1, 0.01);
    volumeSlider.setValue(0.5);
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

    startTimer(100);//recall timercallback


    // Tracking last volume before muting
    playerAudio.setGain((float)volumeSlider.getValue());

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);

    playerAudio.addChangeListener(this);

    jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());
    deleteButton.setEnabled(false);

}

void PlayerGUI::resized()
{
    int y = 20;

    loadButton.setBounds(20, y, 100, 40);
    prevButton.setBounds(140, y, 80, 40);
    playPauseButton.setBounds(240, y, 80, 40);
    nextButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    loopButton.setBounds(540, y, 80, 40);
    jumpBackButton.setBounds(640, y, 80, 40);
    jumpForwardButton.setBounds(740, y, 80, 40);
    saveSessionButton.setBounds(840, y, 100, 40);
    loadSessionButton.setBounds(960, y, 100, 40);
    addMarkerButton.setBounds(1080, y, 100, 40);
    jumpToMarkerButton.setBounds(1200, y, 100, 40);
    loadFolderButton.setBounds(1320, y, 100, 40);
    deleteButton.setBounds(1440, y, 80, 40);
    shuffleButton.setBounds(1540, y, 80, 40);
  
    y += 50;
    set_AB_loop.setBounds(20, y, 100, 40);
    set_A_pos.setBounds(140, y, 80, 40);
    set_B_pos.setBounds(240, y, 80, 40);

    volumeSlider.setBounds(40, 120, getWidth() - 120, 30);

    timelineslider.setBounds(120, 170, getWidth() - 200, 30);

    timeLabel.setBounds(40, 170, 120, 30);

    metadataLabel.setBounds(20, 200, getWidth() - 40, 100);

    playlistBox.setBounds(20, 300, getWidth() - 40, 300);

}

PlayerGUI::~PlayerGUI()
{
    playlistBox.setModel(nullptr);
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
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                double length = playerAudio.getLength();
                timelineslider.setRange(0.0, length, 0.1);
                timeLabel.setText("00:00 / " + formatTime(length), juce::dontSendNotification);

                bool added = playerAudio.addToPlaylist(file);
                if (added)
                {
                    playlistBox.updateContent();

                    if (!playerAudio.playingState())
                    {
                        int newIndex = playerAudio.getPlaylistSize() - 1;
                        playlistBox.selectRow(newIndex);
                        playerAudio.playFileAt(newIndex);

                        double length = playerAudio.getLength();
                        timelineslider.setRange(0.0, length, 0.1);
                        timeLabel.setText(formatTime(0.0) + " / " + formatTime(length), juce::dontSendNotification);
                    }
                }
            }
        });
    }

    if (button == &loadFolderButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select a folder with audio files...", juce::File{}, "*.wav;*.mp3");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories, [this](const juce::FileChooser& fc)
        {
            auto directory = fc.getResult();
            if (directory.isDirectory())
            {
                auto files = directory.findChildFiles(juce::File::findFiles, false, "*.wav;*.mp3");
                bool firstFileAdded = false;
                int firstNewIndex = -1;

                for (const auto& file : files)
                {
                    bool added = playerAudio.addToPlaylist(file);
                    if (added)
                    {
                        if (!firstFileAdded)
                        {
                            firstFileAdded = true;
                            firstNewIndex = playerAudio.getPlaylistSize() - 1;
                        }
                    }
                }

                playlistBox.updateContent();

                if (!playerAudio.playingState() && firstFileAdded)
                {
                    playlistBox.selectRow(firstNewIndex);
                    playerAudio.playFileAt(firstNewIndex);
               
                    double length = playerAudio.getLength();
                    timelineslider.setRange(0.0, length, 0.1);
                    timeLabel.setText(formatTime(0.0) + " / " + formatTime(length), juce::dontSendNotification);
                }
            }
        });
    }

    if (button == &deleteButton)
    {
        int rowToDelete = playlistBox.getSelectedRow();
        if (rowToDelete >= 0)
        {
            playerAudio.removeFromPlaylist(rowToDelete);
            playlistBox.updateContent();
            playerAudio.sendChangeMessage();
        }
    }

    if (button == &prevButton)
    {
        playerAudio.playPrevious();
    }

    if (button == &nextButton)
    {
        playerAudio.playNext();
    }

    if (button == &muteButton)
    {
        playerAudio.muteUnmute();

        if (playerAudio.mutedState())
        {
            volumeSlider.setValue(0.0);
            muteButton.setButtonText("Unmute");
        }
        else
        {
            volumeSlider.setValue(playerAudio.getGain());
            muteButton.setButtonText("Mute");
        }
    }

    if (button == &loopButton)
    {
        playerAudio.loop();
        loopButton.setButtonText(playerAudio.loopState() ? "Loop Off" : "Loop");
    }

    if (button == &playPauseButton)
    {
        playerAudio.stop();

        if (playerAudio.playingState()) playPauseButton.setButtonText("Pause");
        else playPauseButton.setButtonText("Play");
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

        playlistBox.updateContent();

        volumeSlider.setValue(playerAudio.getGain());
        playerAudio.sendChangeMessage();
        playerAudio.play();
    }

    if (button == &shuffleButton)
    {
        playerAudio.shuffle();
        shuffleButton.setButtonText(playerAudio.shuffleState() ? "Shuffle Off" : "Shuffle");
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
        float newVolume = (float)slider->getValue();
        playerAudio.setGain(newVolume);
    }

    else if (slider == &timelineslider)
    {
        if (slider->isMouseButtonDown())
        {
            playerAudio.setPosition(slider->getValue());
        }
    }
}

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &playerAudio)
    {
        juce::String title = playerAudio.getTitle();
        double lenSeconds = playerAudio.getLength();
        int minutes = (int)(lenSeconds / 60);
        int seconds = (int)(fmod(lenSeconds, 60.0));
        juce::String duration = juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0', 2);
        juce::String info = "Title: " + title + "\n" + "Duration: " + duration;
        metadataLabel.setText(info, juce::dontSendNotification);

        playlistBox.selectRow(playerAudio.getCurrentIndex(), juce::dontSendNotification);

        timelineslider.setRange(0.0, lenSeconds, 0.1);
        timeLabel.setText(formatTime(playerAudio.getPosition()) + " / " + formatTime(lenSeconds), juce::dontSendNotification);

        if (playerAudio.getCurrentIndex() > 0) 
        {
            prevButton.setButtonText("Previous");
        }

        else prevButton.setButtonText("Start");

        if (playerAudio.getCurrentIndex() >= 0 && playerAudio.getCurrentIndex() < playerAudio.getPlaylistSize() - 1)
        {
            nextButton.setButtonText("Next");
        }

        else nextButton.setButtonText("End");

        jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());

        playPauseButton.setButtonText(playerAudio.playingState() ? "Pause" : "Play");
        muteButton.setButtonText(playerAudio.mutedState() ? "Unmute" : "Mute");
        loopButton.setButtonText(playerAudio.loopState() ? "Loop Off" : "Loop");
        shuffleButton.setButtonText(playerAudio.shuffleState() ? "Shuffle Off" : "Shuffle");
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

void PlayerGUI::sliderDragStarted(juce::Slider* slider)
{
    if (slider == &timelineslider)
    {
        wasPlaying = playerAudio.playingState();
        if (wasPlaying)
            playerAudio.stop();
    }
}
void PlayerGUI::sliderDragEnded(juce::Slider * slider)
{
    if (slider == &timelineslider)
    {
        playerAudio.setPosition(slider->getValue());
        if (wasPlaying)
            playerAudio.stop();
    }
}
int PlayerGUI::getNumRows()
{
    return playerAudio.getPlaylistSize();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightblue.withAlpha(0.5f));
    }

    g.setColour(juce::Colours::white);

    juce::String text = playerAudio.getFileAt(rowNumber).getFileName();

    g.drawText(text,5,0, width - 10, height,juce::Justification::centredLeft,true);
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
    playerAudio.playFileAt(row);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{

    deleteButton.setEnabled(playlistBox.getSelectedRow() != -1);
}

