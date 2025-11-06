#include "PlayerGUI.h"

// this is the constructor
// it initializes playeraudio with the id
// it initializes our thumbnail reference
PlayerGUI::PlayerGUI(juce::String playerID) : playerAudio(playerID), thumbnail(playerAudio.getThumbnail())
{
    // this loop sets up all the juce textbuttons
    // set this class as their listener
    // make them visible using addandmakevisible
    for (auto* btn : { &loadButton, &saveSessionButton, &loadSessionButton,
            &loadFolderButton, &deleteButton, &set_A_pos,
            &set_B_pos, &set_AB_loop })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // this loop sets up all the juce imagebuttons
    // set this class as their listener
    // make them visible
    for (auto* btn : { &jumpBackButton, &jumpForwardButton, &addMarkerButton,
            &jumpToMarkerButton,&prevButton, &nextButton, &playPauseButton,
            &loopButton, &muteButton, &shuffleButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // this block loads the image for the jumpback button
    // from the binarydata (embedded in the app)
    // using juce's imagecache and setimages
    auto rw10Img = juce::ImageCache::getFromMemory(BinaryData::rw10Small_png, BinaryData::rw10Small_pngSize);
    jumpBackButton.setImages(true, false, true,
        rw10Img, 1.0f, juce::Colours::transparentBlack,
        rw10Img, 0.8f, juce::Colours::transparentBlack,
        rw10Img, 0.7f, juce::Colours::transparentBlack);

    // this block loads the image for the jumpforward button
    auto ffw10Img = juce::ImageCache::getFromMemory(BinaryData::ffw10Small_png, BinaryData::ffw10Small_pngSize);
    jumpForwardButton.setImages(true, false, true,
        ffw10Img, 1.0f, juce::Colours::transparentBlack,
        ffw10Img, 0.8f, juce::Colours::transparentBlack,
        ffw10Img, 0.7f, juce::Colours::transparentBlack);

    // this block loads the image for the addmarker button
    auto markerImg = juce::ImageCache::getFromMemory(BinaryData::markerSmall_png, BinaryData::markerSmall_pngSize);
    addMarkerButton.setImages(true, false, true,
        markerImg, 1.0f, juce::Colours::transparentBlack,
        markerImg, 0.8f, juce::Colours::transparentBlack,
        markerImg, 0.7f, juce::Colours::transparentBlack);

    // this block loads the image for the gomarker button
    auto goMarkerImg = juce::ImageCache::getFromMemory(BinaryData::goMarkerSmall_png, BinaryData::goMarkerSmall_pngSize);
    jumpToMarkerButton.setImages(true, false, true,
        goMarkerImg, 1.0f, juce::Colours::transparentBlack,
        goMarkerImg, 0.8f, juce::Colours::transparentBlack,
        goMarkerImg, 0.7f, juce::Colours::transparentBlack);

    // this block loads the image for the prev button
    auto prevImg = juce::ImageCache::getFromMemory(BinaryData::toStartSmall_png, BinaryData::toStartSmall_pngSize);
    prevButton.setImages(true, false, true,
        prevImg, 1.0f, juce::Colours::transparentBlack,
        prevImg, 0.8f, juce::Colours::transparentBlack,
        prevImg, 0.7f, juce::Colours::transparentBlack);

    // this block loads the image for the next button
    auto nextImg = juce::ImageCache::getFromMemory(BinaryData::endSmall_png, BinaryData::endSmall_pngSize);
    nextButton.setImages(true, false, true,
        nextImg, 1.0f, juce::Colours::transparentBlack,
        nextImg, 0.8f, juce::Colours::transparentBlack,
        nextImg, 0.7f, juce::Colours::transparentBlack);

    // this block loads the play and pause images
    // and sets them for the toggled states of the button
    auto playImg = juce::ImageCache::getFromMemory(BinaryData::playSmall_png, BinaryData::playSmall_pngSize);
    auto pauseImg = juce::ImageCache::getFromMemory(BinaryData::pauseSmall_png, BinaryData::pauseSmall_pngSize);
    playPauseButton.setImages(true, true, true,
        playImg, 1.0f, juce::Colours::transparentBlack,
        playImg, 0.8f, juce::Colours::transparentBlack,
        pauseImg, 1.0f, juce::Colours::transparentBlack);

    // this block loads the mute and unmute images
    // and sets them for the toggled states
    auto muteImg = juce::ImageCache::getFromMemory(BinaryData::muteSmall_png, BinaryData::muteSmall_pngSize);
    auto unmuteImg = juce::ImageCache::getFromMemory(BinaryData::unmuteSmall_png, BinaryData::unmuteSmall_pngSize);
    muteButton.setImages(true, true, true,
        muteImg, 1.0f, juce::Colours::transparentBlack,
        muteImg, 0.8f, juce::Colours::transparentBlack,
        unmuteImg, 1.0f, juce::Colours::transparentBlack);

    // this block loads the loop image
    // and sets it for the toggled states
    auto loopImg = juce::ImageCache::getFromMemory(BinaryData::loopSmall_png, BinaryData::loopSmall_pngSize);
    loopButton.setImages(true, true, true,
        loopImg, 1.0f, juce::Colours::transparentBlack,
        loopImg, 0.6f, juce::Colours::transparentBlack,
        loopImg, 0.7f, juce::Colours::transparentBlack);

    // this block loads the shuffle image
    // and sets it for the toggled states
    auto shuffleImg = juce::ImageCache::getFromMemory(BinaryData::shuffleSmall_png, BinaryData::shuffleSmall_pngSize);
    shuffleButton.setImages(true, true, true,
        shuffleImg, 1.0f, juce::Colours::transparentBlack,
        shuffleImg, 0.6f, juce::Colours::transparentBlack,
        shuffleImg, 0.7f, juce::Colours::transparentBlack);

    // setup the marker juce label
    markerLabel.setText("Marker: --:--", juce::dontSendNotification);
    markerLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    markerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    markerLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(markerLabel);

    // setup the speed and volume labels
    speedLabel.setText("Speed", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    // setup the metadata label (where the waveform will go)
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataLabel);

    // loop to setup the speed and volume labels
    for (auto* label : { &speedLabel, &volumeLabel })
    {
        label->setFont(juce::Font(10.0f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }

    // setup the volume juce slider (vertical)
    volumeSlider.setRange(0.0, 1, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(volumeSlider);

    // setup the timeline juce slider (horizontal) 
    timelineslider.setRange(0.0, 1.0, 0.01);
    timelineslider.setValue(0.0);
    timelineslider.addListener(this);
    timelineslider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(timelineslider);

    // setup the time juce label 
    timeLabel.setText("00:00:00/00:00:00", juce::dontSendNotification);
    timeLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(timeLabel);

    // start the gui timer (for the timeline)
    startTimer(100);


    // setup the speed juce slider (vertical)
    speedSlider.setRange(0.25, 2.0, 0.25);
    speedSlider.setValue(1);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(speedSlider);

    // set the initial gain in the audio engine
    playerAudio.setGain((float)volumeSlider.getValue());

    // setup the juce playlist listbox
    addAndMakeVisible(playlistBox);
    // tell it this class is its 'model' (data source)
    playlistBox.setModel(this);

    // tell playeraudio we want to listen for its changes
    playerAudio.addChangeListener(this);

    // set initial state of buttons
    jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());
    deleteButton.setEnabled(false);

    // tell the thumbnail we want to listen for its changes
    thumbnail.addChangeListener(this);

    // start the gui timer (at 30hz)
    // (this overrides the starttimer(100) call)
    startTimerHz(30);

}

// this juce function lays out all the components using flexbox
void PlayerGUI::resized()
{
    // get the total window area
    auto bounds = getLocalBounds();

    // create the main vertical box (column)
    juce::FlexBox mainFlexBox;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;

    // create a vertical box for the speed slider and its label
    juce::FlexBox speedSliderBox;
    speedSliderBox.flexDirection = juce::FlexBox::Direction::column;
    speedSliderBox.items.add(juce::FlexItem(speedLabel).withHeight(20.0f));
    speedSliderBox.items.add(juce::FlexItem(speedSlider).withFlex(1.0));

    // create a vertical box for the volume slider and its label
    juce::FlexBox volumeSliderBox;
    volumeSliderBox.flexDirection = juce::FlexBox::Direction::column;
    volumeSliderBox.items.add(juce::FlexItem(volumeLabel).withHeight(20.0f));
    volumeSliderBox.items.add(juce::FlexItem(volumeSlider).withFlex(1.0));

    // create the top row box (horizontal)
    juce::FlexBox topRowBox;
    topRowBox.flexDirection = juce::FlexBox::Direction::row;

    // create a vertical box for the file buttons
    juce::FlexBox fileButtonsBox;
    fileButtonsBox.flexDirection = juce::FlexBox::Direction::column;
    // add the file buttons to their box
    fileButtonsBox.items.add(juce::FlexItem(loadButton).withFlex(1).withMargin(5.0f));
    fileButtonsBox.items.add(juce::FlexItem(saveSessionButton).withFlex(1).withMargin(5.0f));
    fileButtonsBox.items.add(juce::FlexItem(loadSessionButton).withFlex(1).withMargin(5.0f));


    // add all components to the top row
    // (file buttons metadata sliders)
    topRowBox.items.add(juce::FlexItem(fileButtonsBox).withWidth(120.0f));
    topRowBox.items.add(juce::FlexItem(metadataLabel).withFlex(1.0));
    topRowBox.items.add(juce::FlexItem(speedSliderBox).withWidth(80.0f));
    topRowBox.items.add(juce::FlexItem(volumeSliderBox).withWidth(80.0f).withMargin(5.0f));

    // create the timeline row (horizontal)
    auto timelineItem = juce::FlexItem(timelineslider).withFlex(1.0);
    // add the time label and timeline slider to it
    juce::FlexBox timelineRowBox;
    timelineRowBox.flexDirection = juce::FlexBox::Direction::row;
    timelineRowBox.items.add(juce::FlexItem(timeLabel).withWidth(100.0f));
    timelineRowBox.items.add(timelineItem.withFlex(1.0));

    // create the main controls row (horizontal)
    juce::FlexBox controlsRowBox;
    controlsRowBox.flexDirection = juce::FlexBox::Direction::row;
    controlsRowBox.alignItems = juce::FlexBox::AlignItems::stretch;

    // create a box for just the playback buttons
    juce::FlexBox playbackBox;
    playbackBox.flexDirection = juce::FlexBox::Direction::row;
    playbackBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    // add all playback buttons to it
    playbackBox.items.add(juce::FlexItem(playPauseButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(prevButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(nextButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(muteButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(loopButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(jumpBackButton).withFlex(1).withMargin(2.0f));
    playbackBox.items.add(juce::FlexItem(jumpForwardButton).withFlex(1).withMargin(2.0f));

    // create a box for just the marker buttons
    juce::FlexBox markerButtonsBox;
    markerButtonsBox.flexDirection = juce::FlexBox::Direction::row;
    markerButtonsBox.items.add(juce::FlexItem(addMarkerButton).withFlex(1).withMargin(2.0f));
    markerButtonsBox.items.add(juce::FlexItem(jumpToMarkerButton).withFlex(1).withMargin(2.0f));

    // create a vertical column for the marker buttons and label
    juce::FlexBox markerColumn;
    markerColumn.flexDirection = juce::FlexBox::Direction::column;
    // add the buttons and label to the column
    markerColumn.items.add(juce::FlexItem(markerButtonsBox).withFlex(1.0));
    markerColumn.items.add(juce::FlexItem(markerLabel).withHeight(20.0f));

    // create a box for the a-b loop buttons
    juce::FlexBox abLoopBox;
    abLoopBox.flexDirection = juce::FlexBox::Direction::row;
    // add all three a-b loop buttons to it
    abLoopBox.items.add(juce::FlexItem(set_A_pos).withFlex(1).withMargin(2.0f));
    abLoopBox.items.add(juce::FlexItem(set_B_pos).withFlex(1).withMargin(2.0f));
    abLoopBox.items.add(juce::FlexItem(set_AB_loop).withFlex(1).withMargin(2.0f));

    // add the three sub-boxes to the main controls row
    // (playback marker a-b loop)
    controlsRowBox.items.add(juce::FlexItem(playbackBox).withFlex(2.5));
    controlsRowBox.items.add(juce::FlexItem(markerColumn).withFlex(1.0));
    controlsRowBox.items.add(juce::FlexItem(abLoopBox).withFlex(1.5));

    // create the playlist controls row (horizontal)
    juce::FlexBox playlistControlsBox;
    playlistControlsBox.flexDirection = juce::FlexBox::Direction::row;
    playlistControlsBox.justifyContent = juce::FlexBox::JustifyContent::flexStart; // align left
    // add the load playlist and shuffle buttons
    playlistControlsBox.items.add(juce::FlexItem(loadFolderButton).withWidth(120.0f).withMargin(5.0f));
    playlistControlsBox.items.add(juce::FlexItem(shuffleButton).withWidth(60.0f).withMargin(5.0f));
    // add a spacer to push the delete button to the right
    playlistControlsBox.items.add(juce::FlexItem().withFlex(1.0));
    // add the delete button
    playlistControlsBox.items.add(juce::FlexItem(deleteButton).withWidth(90.0f).withMargin(5.0f));


    // add all the rows to the main vertical flexbox
    // (top row timeline controls playlist-controls playlist)
    mainFlexBox.items.add(juce::FlexItem(topRowBox).withHeight(150.0f));
    mainFlexBox.items.add(juce::FlexItem(timelineRowBox).withHeight(30.0f));
    mainFlexBox.items.add(juce::FlexItem(controlsRowBox).withHeight(100.0f));
    mainFlexBox.items.add(juce::FlexItem(playlistControlsBox).withHeight(40.0f));
    mainFlexBox.items.add(juce::FlexItem(playlistBox).withFlex(1.0));

    // tell the main flexbox to perform the layout
    mainFlexBox.performLayout(bounds);
}

// this is the destructor
// tell the playlistbox we are no longer its model
// stop the gui timer
PlayerGUI::~PlayerGUI()
{
    playlistBox.setModel(nullptr);
    stopTimer();
}

// pass these calls down to the audio engine
// (they are called by maincomponent)
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

// this juce function draws the component
void PlayerGUI::paint(juce::Graphics& g)
{
    // fill the background with grey
    g.fillAll(juce::Colours::darkgrey);

    // set the waveform area to be the same as the metadata label
    // i did this because metadatalabel is already in the flexbox
    juce::Rectangle<int> thumbnailArea = metadataLabel.getBounds();

    // draw a black background for the waveform
    g.setColour(juce::Colours::black);
    g.fillRect(thumbnailArea);
    // get the thumbnail object from the engine
    auto& thumbnail = playerAudio.getThumbnail();

    // set the waveform color to orange
    g.setColour(juce::Colours::orange);
    // draw the waveform using juce's thumbnail class
    thumbnail.drawChannel(g, thumbnailArea, 0.0, thumbnail.getTotalLength(), 0, 0.5f);

    // check if a song is loaded
    double length = playerAudio.getLength();
    if (length > 0.0)
    {
        // get the current position
        double currPosition = playerAudio.getPosition();
        // calculate the x coordinate for the playhead
        float x = thumbnailArea.getX() + ((float)(currPosition / length) * thumbnailArea.getWidth());

        // draw a red vertical line at the playhead position
        g.setColour(juce::Colours::red);
        g.drawVerticalLine((int)x, (float)thumbnailArea.getY(), (float)thumbnailArea.getBottom());
    }
}

// this is called when any button is pressed
// (from the juce::button::listener)
void PlayerGUI::buttonClicked(juce::Button* button)
{
    // if it was the load button
    if (button == &loadButton)
    {
        // create a juce file chooser
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, "*.wav;*.mp3");

        // open it asynchronously (so it doesn't freeze the app)
        // this lambda function is called when the user chooses a file
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](const juce::FileChooser& fc)
            {
                // check if the file is valid
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    // add it to the playlist (this checks for duplicates)
                    bool added = playerAudio.addToPlaylist(file);
                    // if it was added successfully
                    if (added)
                    {
                        // tell the listbox to refresh
                        playlistBox.updateContent();

                        // if nothing is playing
                        if (!playerAudio.playingState())
                        {
                            // select the new song in the list
                            int newIndex = playerAudio.getPlaylistSize() - 1;
                            playlistBox.selectRow(newIndex);
                            // play the new song
                            playerAudio.playFileAt(newIndex);

                            // update the timeline slider's range and text
                            double length = playerAudio.getLength();
                            timelineslider.setRange(0.0, length, 0.1);
                            timeLabel.setText(formatTime(0.0) + " / " + formatTime(length), juce::dontSendNotification);
                        }
                    }
                }
            });
    }

    // if it was the load folder button
    if (button == &loadFolderButton)
    {
        // create a file chooser for directories
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select a folder with audio files...", juce::File{}, "*.wav;*.mp3");

        // ... (rest of logic is similar to load button) ...
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories, [this](const juce::FileChooser& fc)
            {
                auto directory = fc.getResult();
                if (directory.isDirectory())
                {
                    // loop through all files in the directory
                    auto files = directory.findChildFiles(juce::File::findFiles, false, "*.wav;*.mp3");
                    bool firstFileAdded = false;
                    int firstNewIndex = -1;

                    for (const auto& file : files)
                    {
                        // add each valid file
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

                    // refresh the listbox
                    playlistBox.updateContent();

                    // if not playing play the first new song
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

    // if it was the delete button
    if (button == &deleteButton)
    {
        // get the currently selected row
        int rowToDelete = playlistBox.getSelectedRow();
        // if a row is selected
        if (rowToDelete >= 0)
        {
            // tell the engine to remove it
            playerAudio.removeFromPlaylist(rowToDelete);
            // refresh the listbox
            playlistBox.updateContent();
            // tell the gui to update (in case we deleted the current song)
            playerAudio.sendChangeMessage();
        }
    }

    // if it was the previous button
    if (button == &prevButton)
    {
        // call the engine's function
        playerAudio.playPrevious();
    }

    // if it was the next button
    if (button == &nextButton)
    {
        // call the engine's function
        playerAudio.playNext();
    }

    // if it was the mute button
    if (button == &muteButton)
    {
        // call the engine's toggle function
        playerAudio.muteUnmute();

        // update the volume slider to match the state
        if (playerAudio.mutedState())
        {
            volumeSlider.setValue(0.0);
        }
        else
        {
            volumeSlider.setValue(playerAudio.getGain());
        }
    }

    // if it was the loop button
    if (button == &loopButton)
    {
        // call the engine's toggle function
        playerAudio.loop();
    }

    // if it was the play/pause button
    if (button == &playPauseButton)
    {
        // call the engine's toggle function
        playerAudio.stop();
    }

    // ... (rest of buttons just call their matching engine function) ...
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

    // if it was the load session button
    if (button == &loadSessionButton)
    {
        // tell the engine to load the data
        playerAudio.loadSession();
        // refresh the listbox
        playlistBox.updateContent();
        // update the volume slider
        volumeSlider.setValue(playerAudio.getGain());
        // update the speed slider
        speedSlider.setValue(playerAudio.getPlaybackSpeed());
        // tell the gui to update (buttons metadata etc)
        playerAudio.sendChangeMessage();
        // start playback
        playerAudio.play();
    }

    // if it was the shuffle button
    if (button == &shuffleButton)
    {
        // call the engine's toggle function
        playerAudio.shuffle();
    }

    // if it was the set a button
    if (button == &set_A_pos)
    {
        // call the engine's setpointa function
        playerAudio.setPointA();
        // update the button text
        set_A_pos.setButtonText("A: " + formatTime(playerAudio.getPointA()));

        // if b was set before a reset it
        if (playerAudio.getPointB() < 0)
        {
            set_B_pos.setButtonText("Set B");
            set_AB_loop.setButtonText("A->B Loop");
        }
    }

    // if it was the set b button
    if (button == &set_B_pos)
    {
        // call the engine's setpointb function
        playerAudio.setPointB();
        // update the button text
        if (playerAudio.getPointB() > 0)
            set_B_pos.setButtonText("B: " + formatTime(playerAudio.getPointB()));
        else
            set_B_pos.setButtonText("Set B");
    }

    // if it was the a-b loop button
    if (button == &set_AB_loop)
    {
        // call the engine's toggle function
        playerAudio.toggleABLoop();

        // update all the a-b button text based on the new state
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

// this is called when any slider is moved
// (from the juce::slider::listener)
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    // if it was the volume slider
    if (slider == &volumeSlider)
    {
        // tell the engine to set the gain
        float newVolume = (float)slider->getValue();
        playerAudio.setGain(newVolume);
    }
    // if it was the timeline slider
    else if (slider == &timelineslider)
    {
        // only update if the user is dragging it
        // (this prevents a loop with timercallback)
        if (slider->isMouseButtonDown())
        {
            // tell the engine to set the position
            playerAudio.setPosition(slider->getValue());
        }
    }

    // if it was the speed slider
    if (slider == &speedSlider)
    {
        // tell the engine to set the playback speed
        float newSpeed = (float)slider->getValue();
        playerAudio.playbackSpeed(newSpeed);
    }
}

// this is called by the engine (sendchangemessage)
// (from the juce::changelistener)
void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // if the change came from playeraudio
    if (source == &playerAudio)
    {
        // get all the data (title length etc)
        juce::String title = playerAudio.getTitle();
        double lenSeconds = playerAudio.getLength();
        int minutes = (int)(lenSeconds / 60);
        int seconds = (int)(fmod(lenSeconds, 60.0));
        juce::String duration = juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0', 2);
        // build the metadata string
        juce::String info = "Title: " + title + "\n" + "Duration: " + duration;
        // set the metadata label text
        metadataLabel.setText(info, juce::dontSendNotification);

        // select the correct row in the playlist
        playlistBox.selectRow(playerAudio.getCurrentIndex(), juce::dontSendNotification);

        // update the timeline slider's range
        timelineslider.setRange(0.0, lenSeconds, 0.1);
        // update the time label text
        timeLabel.setText(formatTime(playerAudio.getPosition()) + " / " + formatTime(lenSeconds), juce::dontSendNotification);

        // update the marker button (enabled/disabled)
        jumpToMarkerButton.setEnabled(playerAudio.isMarkerSet());

        // update the marker label text
        if (playerAudio.isMarkerSet())
            markerLabel.setText("Marker: " + formatTime(playerAudio.getMarkerPosition()), juce::dontSendNotification);
        else
            markerLabel.setText("Marker: --:--", juce::dontSendNotification);

        // update all the transport button icons (toggled state)
        playPauseButton.setToggleState(playerAudio.playingState(), juce::dontSendNotification);
        muteButton.setToggleState(playerAudio.mutedState(), juce::dontSendNotification);
        loopButton.setToggleState(playerAudio.loopState(), juce::dontSendNotification);
        shuffleButton.setToggleState(playerAudio.shuffleState(), juce::dontSendNotification);
    }
    // if the change came from the thumbnail
    else if (source == &thumbnail)
    {
        // just repaint the screen (to draw the new waveform)
        repaint();
    }
}

// this is called by the gui timer (starttimerhz(30))
// (from the juce::timer)
void PlayerGUI::timerCallback()
{
    // get the current time
    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();
    // update the timeline slider (if user is not dragging)
    timelineslider.setValue(currentPos, juce::dontSendNotification);
    // update the time label
    juce::String timeText = formatTime(currentPos) + " / " + formatTime(totalLength);
    timeLabel.setText(timeText, juce::dontSendNotification);

    // check the a-b loop logic
    // if we passed point b send us back to point a
    if (playerAudio.abLoopState() && playerAudio.getPointA() >= 0 &&
        playerAudio.getPointB() > playerAudio.getPointA() &&
        currentPos > playerAudio.getPointB())
    {
        playerAudio.setPosition(playerAudio.getPointA());
        playerAudio.play();
    }

    // repaint the screen (to move the red playhead line)
    repaint();
}

// a helper function to turn seconds into a 00:00 string
// it supports hours too
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

// called when the user clicks the timeline
void PlayerGUI::sliderDragStarted(juce::Slider* slider)
{
    if (slider == &timelineslider)
    {
        // remember if we were playing
        wasPlaying = playerAudio.playingState();
        // pause the music
        if (wasPlaying)
            playerAudio.stop();
    }
}

// called when the user releases the timeline
void PlayerGUI::sliderDragEnded(juce::Slider* slider)
{
    if (slider == &timelineslider)
    {
        // set the new position in the engine
        playerAudio.setPosition(slider->getValue());
        // if we were playing before un-pause the music
        if (wasPlaying)
            playerAudio.stop();
    }
}

// listboxmodel function
// returns the number of songs in the playlist
int PlayerGUI::getNumRows()
{
    return playerAudio.getPlaylistSize();
}

// listboxmodel function
// this draws one row of the playlist
void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    // if the row is selected draw a blue background
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightblue.withAlpha(0.5f));
    }

    // set the text color
    g.setColour(juce::Colours::white);

    // get the file name from the engine
    juce::String text = playerAudio.getFileAt(rowNumber).getFileName();

    // draw the text
    g.drawText(text, 5, 0, width - 10, height, juce::Justification::centredLeft, true);
}

// listboxmodel function
// this is our "play" button for the playlist
void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
    // tell the engine to play the song at this row
    playerAudio.playFileAt(row);
}

// listboxmodel function
// called when the user clicks a row
// we use this to enable/disable the delete button
void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    deleteButton.setEnabled(playlistBox.getSelectedRow() != -1);
}