#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

// this is our main player user interface class
// it inherits from juce's component (base juce class)
// button listener for button clicks
// slider listener for slider moves
// change listener to get updates from playeraudio
// listboxmodel to provide data to our playlist
// timer to run our own clock (for the timeline slider)
class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::ChangeListener,
	public juce::ListBoxModel,
	public juce::Timer

{
public:
	// constructor takes the player id ("1" or "2")
	PlayerGUI(juce::String playerID);
	// destructor
	~PlayerGUI() override;

	// standard juce component functions
	void resized() override;
	void paint(juce::Graphics& g) override;

	// standard juce audio functions (passed to maincomponent)
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	// this is called by the gui timer (from starttimerhz)
	void timerCallback() override;

	// (this seems unused) lets the mixer get gain
	float getGain() const { return playerAudio.getGain(); }

	// called by playeraudio when something changes (using sendchangemessage)
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	// callbacks for when the user clicks/releases the timeline
	void sliderDragStarted(juce::Slider* slider) override;//me
	void sliderDragEnded(juce::Slider* slider) override;//me

	// these are the functions for the juce listboxmodel
	// they tell the playlist *what* to draw
	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
	void selectedRowsChanged(int lastRowSelected) override;

private:
	// our instance of the audio engine
	PlayerAudio playerAudio;

	// a reference to the engine's thumbnail object
	juce::AudioThumbnail& thumbnail;

	// gui elements
	// we use juce imagebuttons for the transport controls
	juce::ImageButton muteButton;
	juce::ImageButton loopButton;
	juce::ImageButton playPauseButton;
	// and juce textbuttons for the file/session controls
	juce::TextButton loadButton{ "Load File" };
	juce::ImageButton jumpBackButton;
	juce::ImageButton jumpForwardButton;
	juce::TextButton saveSessionButton{ "Save Session" };
	juce::TextButton loadSessionButton{ "Load Session" };
	juce::ImageButton addMarkerButton;
	juce::ImageButton jumpToMarkerButton;


	juce::Slider timelineslider;//me
	juce::Label timeLabel;
	// helper function to format time strings
	juce::String formatTime(double seconds);
	juce::TextButton set_A_pos{ "Set A" };
	juce::TextButton set_B_pos{ "Set B" };
	juce::TextButton set_AB_loop{ "A->B Loop" };
	//juce::TextButton clearAB;

	// a bool to remember if we were playing before dragging the slider
	bool wasPlaying = false;

	juce::TextButton loadFolderButton{ "Load Playlist" };
	juce::ImageButton prevButton;
	juce::ImageButton nextButton;
	juce::TextButton deleteButton{ "Delete" };
	juce::ImageButton shuffleButton;

	// the juce listbox component for the playlist
	juce::ListBox playlistBox;

	// juce labels for metadata speed volume and marker
	juce::Label metadataLabel;
	juce::Label speedLabel;
	juce::Label volumeLabel;
	juce::Label markerLabel;

	// juce sliders for volume and speed
	juce::Slider volumeSlider;
	juce::Slider speedSlider;

	// a juce filechooser for the load buttons
	std::unique_ptr <juce::FileChooser> fileChooser;

	// handler functions for events from juce listeners
	// (button clicks slider moves etc)
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;

	// juce macro to prevent copying and detect leaks
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};