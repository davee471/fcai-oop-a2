#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
	: thumbnail(playerAudio.getThumbnail())
{
    for (auto* btn : { &loadButton, &saveSessionButton, &loadSessionButton, 
            &loadFolderButton, &deleteButton, &set_A_pos, 
            &set_B_pos, &set_AB_loop})
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    for(auto* btn : {&jumpBackButton, &jumpForwardButton, &addMarkerButton, 
            &jumpToMarkerButton,&prevButton, &nextButton, &playPauseButton,
            &loopButton, &muteButton, &shuffleButton})
    {
        btn->addListener(this);
		addAndMakeVisible(btn);
	}

    auto rw10Img = juce::ImageCache::getFromMemory(BinaryData::rw10Small_png, BinaryData::rw10Small_pngSize);
    jumpBackButton.setImages(true, false, true,
        rw10Img, 1.0f, juce::Colours::transparentBlack,
        rw10Img, 0.8f, juce::Colours::transparentBlack,
        rw10Img, 0.7f, juce::Colours::transparentBlack);

    auto ffw10Img = juce::ImageCache::getFromMemory(BinaryData::ffw10Small_png, BinaryData::ffw10Small_pngSize);
    jumpForwardButton.setImages(true, false, true,
        ffw10Img, 1.0f, juce::Colours::transparentBlack,
        ffw10Img, 0.8f, juce::Colours::transparentBlack,
        ffw10Img, 0.7f, juce::Colours::transparentBlack);

    auto markerImg = juce::ImageCache::getFromMemory(BinaryData::markerSmall_png, BinaryData::markerSmall_pngSize);
    addMarkerButton.setImages(true, false, true,
        markerImg, 1.0f, juce::Colours::transparentBlack,
        markerImg, 0.8f, juce::Colours::transparentBlack,
        markerImg, 0.7f, juce::Colours::transparentBlack);

    auto goMarkerImg = juce::ImageCache::getFromMemory(BinaryData::goMarkerSmall_png, BinaryData::goMarkerSmall_pngSize);
    jumpToMarkerButton.setImages(true, false, true,
        goMarkerImg, 1.0f, juce::Colours::transparentBlack,
        goMarkerImg, 0.8f, juce::Colours::transparentBlack,
        goMarkerImg, 0.7f, juce::Colours::transparentBlack);

    auto prevImg = juce::ImageCache::getFromMemory(BinaryData::toStartSmall_png, BinaryData::toStartSmall_pngSize);
    prevButton.setImages(true, false, true,
        prevImg, 1.0f, juce::Colours::transparentBlack,
        prevImg, 0.8f, juce::Colours::transparentBlack,
        prevImg, 0.7f, juce::Colours::transparentBlack);

    auto nextImg = juce::ImageCache::getFromMemory(BinaryData::endSmall_png, BinaryData::endSmall_pngSize);
    nextButton.setImages(true, false, true,
        nextImg, 1.0f, juce::Colours::transparentBlack,
        nextImg, 0.8f, juce::Colours::transparentBlack,
        nextImg, 0.7f, juce::Colours::transparentBlack);

    auto playImg = juce::ImageCache::getFromMemory(BinaryData::playSmall_png, BinaryData::playSmall_pngSize);
    auto pauseImg = juce::ImageCache::getFromMemory(BinaryData::pauseSmall_png, BinaryData::pauseSmall_pngSize);
    playPauseButton.setImages(true, true, true,
        playImg, 1.0f, juce::Colours::transparentBlack, 
        playImg, 0.8f, juce::Colours::transparentBlack,
        pauseImg, 1.0f, juce::Colours::transparentBlack);

    auto muteImg = juce::ImageCache::getFromMemory(BinaryData::muteSmall_png, BinaryData::muteSmall_pngSize);
    auto unmuteImg = juce::ImageCache::getFromMemory(BinaryData::unmuteSmall_png, BinaryData::unmuteSmall_pngSize);

    muteButton.setImages(true, true, true,
        muteImg, 1.0f, juce::Colours::transparentBlack,
        muteImg, 0.8f, juce::Colours::transparentBlack, 
        unmuteImg, 1.0f, juce::Colours::transparentBlack); 

    auto loopImg = juce::ImageCache::getFromMemory(BinaryData::loopSmall_png, BinaryData::loopSmall_pngSize);
    loopButton.setImages(true, true, true,
        loopImg, 1.0f, juce::Colours::transparentBlack, 
        loopImg, 0.6f, juce::Colours::transparentBlack,  
        loopImg, 0.7f, juce::Colours::transparentBlack); 

    auto shuffleImg = juce::ImageCache::getFromMemory(BinaryData::shuffleSmall_png, BinaryData::shuffleSmall_pngSize);
    shuffleButton.setImages(true, true, true,
        shuffleImg, 1.0f, juce::Colours::transparentBlack,  
        shuffleImg, 0.6f, juce::Colours::transparentBlack, 
        shuffleImg, 0.7f, juce::Colours::transparentBlack);
	

	speedLabel.setText("Speed", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataLabel);

    // Volume slider
    volumeSlider.setRange(0.0, 1, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
	volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
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


    // Speed slider
    speedSlider.setRange(0.25, 2.0, 0.25);
    speedSlider.setValue(1);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	addAndMakeVisible(speedSlider);

    // Tracking last volume before muting
    playerAudio.setGain((float)volumeSlider.getValue());

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);

    playerAudio.addChangeListener(this);

    jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());
    deleteButton.setEnabled(false);

    // Start listening to waveform changes
	thumbnail.addChangeListener(this);

    // Starting a timer to keep redrawing where the waveform tracker is, can also be used for loop
    startTimerHz(30);

}

void PlayerGUI::resized()
{
    // Get total area we have to work with
    auto bounds = getLocalBounds();

    // Making main flexbox
    juce::FlexBox mainFlexBox;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;

    // This is where I'll place the waveform and file related controls (Top Row)
    juce::FlexBox topRowBox;
    topRowBox.flexDirection = juce::FlexBox::Direction::row;

	// Load, Save, Load Session buttons here (First Column)
    juce::FlexBox fileButtonsBox;
    fileButtonsBox.flexDirection = juce::FlexBox::Direction::column;
    fileButtonsBox.items.add(juce::FlexItem(loadButton).withFlex(1).withMargin(5.0f));
    fileButtonsBox.items.add(juce::FlexItem(saveSessionButton).withFlex(1).withMargin(5.0f));
    fileButtonsBox.items.add(juce::FlexItem(loadSessionButton).withFlex(1).withMargin(5.0f));


    // Adding the stuff to the top row box thing
    topRowBox.items.add(juce::FlexItem(fileButtonsBox).withWidth(120.0f));
    topRowBox.items.add(juce::FlexItem(metadataLabel).withFlex(1.0)); 
    topRowBox.items.add(juce::FlexItem(speedSlider).withWidth(50.0f));
	topRowBox.items.add(juce::FlexItem(volumeSlider).withWidth(50.0f).withMargin(5.0f));

    // Timeline slider row
    auto timelineItem = juce::FlexItem(timelineslider).withFlex(1.0);

    // We also add the timeLabel here
    juce::FlexBox timelineRowBox;
    timelineRowBox.flexDirection = juce::FlexBox::Direction::row;
    timelineRowBox.items.add(juce::FlexItem(timeLabel).withWidth(100.0f));
    timelineRowBox.items.add(timelineItem.withFlex(1.0));

    // Playback controls
    juce::FlexBox controlsRowBox;
    controlsRowBox.flexDirection = juce::FlexBox::Direction::row;

    juce::FlexBox playbackBox;
    playbackBox.flexDirection = juce::FlexBox::Direction::row;
    playbackBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

    playbackBox.items.add(juce::FlexItem(playPauseButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(prevButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(nextButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(muteButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(loopButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(jumpBackButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(jumpForwardButton).withFlex(1).withMargin(2.0f));


    // Markers controls
    juce::FlexBox markerBox;
    markerBox.flexDirection = juce::FlexBox::Direction::row; // <-- CHANGED to row
    markerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

    markerBox.items.add(juce::FlexItem(addMarkerButton).withFlex(1).withMargin(2.0f));
    markerBox.items.add(juce::FlexItem(jumpToMarkerButton).withFlex(1).withMargin(2.0f));
    markerBox.items.add(juce::FlexItem(set_A_pos).withFlex(1).withMargin(2.0f));
    markerBox.items.add(juce::FlexItem(set_B_pos).withFlex(1).withMargin(2.0f));
    markerBox.items.add(juce::FlexItem(set_AB_loop).withFlex(1).withMargin(2.0f));

    // Add markers and playback to the controlsRowBox
    controlsRowBox.items.add(juce::FlexItem(playbackBox).withFlex(1.0));
    controlsRowBox.items.add(juce::FlexItem(markerBox).withFlex(1.0));

    // Playlist controls
    juce::FlexBox playlistControlsBox;
    playlistControlsBox.flexDirection = juce::FlexBox::Direction::row;
    playlistControlsBox.justifyContent = juce::FlexBox::JustifyContent::flexStart; // Align left

    playlistControlsBox.items.add(juce::FlexItem(loadFolderButton).withWidth(120.0f).withMargin(5.0f));
    playlistControlsBox.items.add(juce::FlexItem(shuffleButton).withWidth(100.0f).withMargin(5.0f));

    // Adding empty item to make delete button more on the right side
    playlistControlsBox.items.add(juce::FlexItem().withFlex(1.0)); // Spacer
    playlistControlsBox.items.add(juce::FlexItem(deleteButton).withWidth(80.0f).withMargin(5.0f));


    // The playlist box
    // ### Add All Rows to the Main Vertical FlexBox ###
    mainFlexBox.items.add(juce::FlexItem(topRowBox).withHeight(150.0f));  // 1. Top Row (fixed height)
    mainFlexBox.items.add(juce::FlexItem(timelineRowBox).withHeight(30.0f)); // 2. Timeline (fixed height)
    mainFlexBox.items.add(juce::FlexItem(controlsRowBox).withHeight(80.0f)); // 3. Controls (fixed height)
    mainFlexBox.items.add(juce::FlexItem(playlistControlsBox).withHeight(40.0f)); // 4. Playlist Controls (fixed height)
    mainFlexBox.items.add(juce::FlexItem(playlistBox).withFlex(1.0));  // 5. Playlist (flexible height)

    // Apply the layout
    mainFlexBox.performLayout(bounds);
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

    // Setting the thumbnail area to metadatalabel cuz it's already in flexbox
    // and I cba to make another thing just for the waveform
    juce::Rectangle<int> thumbnailArea = metadataLabel.getBounds();

    // Colour the rectangle in black
    g.setColour(juce::Colours::black);
    g.fillRect(thumbnailArea);
    auto& thumbnail = playerAudio.getThumbnail();

    // Colour the waveform in orange
    g.setColour(juce::Colours::orange);
    thumbnail.drawChannel(g, thumbnailArea, 0.0, thumbnail.getTotalLength(), 0, 0.5f);

    double length = playerAudio.getLength();
    if (length > 0.0)
    {
        double currPosition = playerAudio.getPosition();
        // Calculate X relative to the thumbnailArea's left edge
        float x = thumbnailArea.getX() + ((float)(currPosition / length) * thumbnailArea.getWidth());

        // Draw vertical line at current position
        g.setColour(juce::Colours::red);
        g.drawVerticalLine((int)x, (float)thumbnailArea.getY(), (float)thumbnailArea.getBottom());
    }
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
        }
        else
        {
            volumeSlider.setValue(playerAudio.getGain());
        }
    }

    if (button == &loopButton)
    {
        playerAudio.loop();
    }

    if (button == &playPauseButton)
    {
        playerAudio.stop();

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
        speedSlider.setValue(playerAudio.getPlaybackSpeed());
        playerAudio.sendChangeMessage();
        playerAudio.play();

    }

    if (button == &shuffleButton)
    {
        playerAudio.shuffle();
    }

    if (button == &set_A_pos)
    {
        playerAudio.setPointA();
        set_A_pos.setButtonText("A: " + formatTime(playerAudio.getPointA()));

        if (playerAudio.getPointB() < 0)
        {
            set_B_pos.setButtonText("Set B");
            set_AB_loop.setButtonText("A->B Loop");
        }
    }

    if (button == &set_B_pos)
    {
        playerAudio.setPointB();
        if (playerAudio.getPointB() > 0)
            set_B_pos.setButtonText("B: " + formatTime(playerAudio.getPointB()));
        else
            set_B_pos.setButtonText("Set B");
    }

    if (button == &set_AB_loop)
    {
        playerAudio.toggleABLoop();

        if (playerAudio.abLoopState())
        {
            set_AB_loop.setButtonText("End AB Loop");
        }
        else {
            set_AB_loop.setButtonText("A->B Loop");
            set_A_pos.setButtonText("Set A");
            set_B_pos.setButtonText("Set B");
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

    if (slider == &speedSlider)
    {
		float newSpeed = (float)slider->getValue();
		playerAudio.playbackSpeed(newSpeed);
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


        jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());

        // changing toggle state to give proper icons when pressed
        playPauseButton.setToggleState(playerAudio.playingState(), juce::dontSendNotification);
        muteButton.setToggleState(playerAudio.mutedState(), juce::dontSendNotification);
        loopButton.setToggleState(playerAudio.loopState(), juce::dontSendNotification);
        shuffleButton.setToggleState(playerAudio.shuffleState(), juce::dontSendNotification);
    }
    else if (source == &thumbnail)
    {
        // redraw the waveform when 'thumbnail' finally loads
        repaint();
    }
}     

void PlayerGUI::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();
    timelineslider.setValue(currentPos, juce::dontSendNotification);
    juce::String timeText = formatTime(currentPos) + " / " + formatTime(totalLength);
    timeLabel.setText(timeText, juce::dontSendNotification);

    if (playerAudio.abLoopState() && playerAudio.getPointA() >= 0 &&
        playerAudio.getPointB() > playerAudio.getPointA() &&
        currentPos > playerAudio.getPointB())
    {
        playerAudio.setPosition(playerAudio.getPointA());
        playerAudio.play();
    }
    
    repaint();
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

