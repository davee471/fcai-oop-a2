#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::ChangeListener,
	public juce::ListBoxModel,
	public juce::Timer

{
public:
	PlayerGUI();
	~PlayerGUI() override;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	void timerCallback() override;
	float getGain() const { return playerAudio.getGain(); }
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void sliderDragStarted(juce::Slider* slider) override;//me
	void sliderDragEnded(juce::Slider* slider) override;//me

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
	void selectedRowsChanged(int lastRowSelected) override;

private:
	PlayerAudio playerAudio;

	// to initialize as thumbnail in the playerGUI constructor so I don't have to keep using the getter
	juce::AudioThumbnail& thumbnail;

	// GUI elements
	juce::ImageButton muteButton;
	juce::ImageButton loopButton;
	juce::ImageButton playPauseButton;
	juce::TextButton loadButton{"Load File"};
	juce::ImageButton jumpBackButton;
	juce::ImageButton jumpForwardButton;
	juce::TextButton saveSessionButton{"Save Session"};
	juce::TextButton loadSessionButton{"Load Session"};
	juce::ImageButton addMarkerButton;
	juce::ImageButton jumpToMarkerButton;


	juce::Slider timelineslider;//me
	juce::Label timeLabel;
	juce::String formatTime(double seconds);
	juce::TextButton set_A_pos{"Set A"};
	juce::TextButton set_B_pos{"Set B"};
	juce::TextButton set_AB_loop{ "A->B Loop" };
	//juce::TextButton clearAB;

	bool wasPlaying = false;

	juce::TextButton loadFolderButton{ "Load Playlist" };
	juce::ImageButton prevButton;
	juce::ImageButton nextButton;
	juce::TextButton deleteButton{ "Delete" };
	juce::ImageButton shuffleButton;

	juce::ListBox playlistBox;
	
	juce::Label metadataLabel;
	juce::Label speedLabel;
	juce::Label volumeLabel;

	juce::Slider volumeSlider;
	juce::Slider speedSlider;
	std::unique_ptr <juce::FileChooser> fileChooser;

	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};