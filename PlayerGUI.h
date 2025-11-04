#pragma once // PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener,
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
	bool Mute = false;
	bool Loop = false;
	bool AB_loop = false;
	void timerCallback() override;
	float pointA = -1.0;
	float pointB = -1.0;

private:
	PlayerAudio playerAudio;

	// GUI elements
	juce::TextButton loadButton{"Load File"};
	juce::TextButton toStartButton{"Start"};
	juce::TextButton stopPlayButton{"Stop"};
	juce::TextButton muteButton{"Mute"};
	juce::TextButton loopButton{ "Loop" };
	juce::TextButton toEndButton{ "End" };
	juce::TextButton jumpBackButton{"-10s"};
	juce::TextButton jumpForwardButton{"+10s"};
	juce::TextButton saveSessionButton{"Save Session"};
	juce::TextButton loadSessionButton{"Load Session"};
	juce::TextButton addMarkerButton{"Add Marker"};
	juce::TextButton jumpToMarkerButton{"Go to Marker"};
	juce::Slider timelineslider;//me
	juce::Label timeLabel;
	juce::String formatTime(double seconds);
	juce::TextButton set_A_pos{"set A"};
	juce::TextButton set_B_pos{"set B"};
	juce::TextButton set_AB_loop{ "A=>B_loop" };
	//juce::TextButton clearAB;



	// Storing volume before muting, initialized as the default start volume
	float prevVolume = 0.5f;

	juce::Slider volumeSlider;
	std::unique_ptr <juce::FileChooser> fileChooser;

	// Event handlers
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};