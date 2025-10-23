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
	void timerCallback() override;

private:
	PlayerAudio playerAudio;

	// GUI elements
	juce::TextButton loadButton{"Load File"};
	juce::TextButton restartButton{"Restart"};
	juce::TextButton stopButton{"Stop"};
	juce::TextButton playButton{"Play"};
	juce::TextButton muteButton{"Mute"};
	juce::TextButton loopButton{ "Loop" };

	// Storing volume before muting, initialized as the default start volume
	float prevVolume = 0.5f;

	juce::Slider volumeSlider;
	std::unique_ptr <juce::FileChooser> fileChooser;

	// Event handlers
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};