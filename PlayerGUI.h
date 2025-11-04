#pragma once // PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <juce_audio_utils/juce_audio_utils.h>

class PlayerGUI : public juce::Component, 
    private juce::Timer,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
	void paint(juce::Graphics& g) override;
    void timerCallback() override;
private:
    PlayerAudio playerAudio;

    // GUI elements
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton play_pauseButton{ "Play/Pause" };
    juce::TextButton goToStartButton{ "|< GoToStart" };
    juce::TextButton goToEndButton{ "GoToEnd >|" };
	juce::TextButton prevButton{ "Previous" };
	juce::TextButton nextButton{ "Next" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::Slider volumeSlider;
	juce::Label fileInfoLabel;
	juce::ComboBox playListComboBox;
    juce::Slider positionSlider;
      juce::Slider speedSlider;
      juce::Slider progressSlider;
    juce::Label timeLabel;
    bool userIsDragging = false;
    double pointA = 0.0;
    double pointB = 0.0;
    bool abLoopEnabled = false;
    bool isMuted = false;
    float lastVolume = 0.5f;
    std::unique_ptr<juce::FileChooser> fileChooser;

	juce::Array<juce::File> playListFiles;
	int currentFileIndex = -1;

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};


