#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::ChangeListener,
	public juce::ListBoxModel

{
public:
	PlayerGUI();
	~PlayerGUI() override;

	void resized() override;
	void paint(juce::Graphics& g) override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
	void selectedRowsChanged(int lastRowSelected) override;

private:
	PlayerAudio playerAudio;

	juce::TextButton muteButton{ "Mute" };
	juce::TextButton loopButton{ "Loop" };
	juce::TextButton playPauseButton{ "Play" };
	juce::TextButton loadButton{"Load File"};
	juce::TextButton jumpBackButton{"-10s"};
	juce::TextButton jumpForwardButton{"+10s"};
	juce::TextButton saveSessionButton{"Save Session"};
	juce::TextButton loadSessionButton{"Load Session"};
	juce::TextButton addMarkerButton{"Add Marker"};
	juce::TextButton jumpToMarkerButton{"Go to Marker"};
	juce::TextButton loadFolderButton{ "Load Playlist" };
	juce::TextButton prevButton{ "Previous" };
	juce::TextButton nextButton{ "Next" };
	juce::TextButton deleteButton{ "Delete" };
	juce::TextButton shuffleButton{ "Shuffle" };

	juce::ListBox playlistBox;
	
	juce::Label metadataLabel;

	juce::Slider volumeSlider;
	std::unique_ptr <juce::FileChooser> fileChooser;

	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};