#include "PlayerGUI.h"
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

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton , &loopButton , &muteButton, &play_pauseButton, &goToStartButton , &goToEndButton , &setAButton, &setBButton
        })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    positionSlider.setRange(0.0, 1.0, 0.1);
    positionSlider.setValue(0.0);
    positionSlider.onValueChange = [this]()
        {
            double newPos = positionSlider.getValue();
            playerAudio.setPosition(newPos);
   
        };

    addAndMakeVisible(positionSlider);

    startTimerHz(10);
}
void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    loopButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    goToStartButton.setBounds(540, y, 100, 40);
    goToEndButton.setBounds(640, y, 100, 40);
    play_pauseButton.setBounds(780, y, 100, 40);
    setAButton.setBounds(20, 250, 100, 40);
    setBButton.setBounds(140, 250, 100, 40);

    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    positionSlider.setBounds(20, 200, getWidth() - 40, 30);
}
PlayerGUI::~PlayerGUI()
{
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
        playerAudio.restart();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }
    if (button == &loopButton)
    {
        playerAudio.setLooping(true);
        playerAudio.start();
    }
    if (button == &muteButton)
    {
        playerAudio.switcherMute();
    }

    if (button == &play_pauseButton)
    {
        playerAudio.playOrpause();
    }
    if (button == &goToStartButton)
    {
        playerAudio.goToStart();
    }
    if (button == &goToEndButton)
    {
        playerAudio.goToEnd();
    }
    if (button == &setAButton)
    {
        pointA = playerAudio.getPosition();
        abLoopEnabled = false; 
    }

    if (button == &setBButton)
    {
        pointB = playerAudio.getPosition();
        abLoopEnabled = true;
    }
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider) {
        if (slider == &volumeSlider)
            playerAudio.setGain((float)slider->getValue());
}

void PlayerGUI::timerCallback()
{
    double length = playerAudio.getLength();

    if (length > 0.0)
    {
        if (positionSlider.getMaximum() != length)
            positionSlider.setRange(0.0, length, 0.01);

        double pos = playerAudio.getPosition();
        positionSlider.setValue(pos, juce::dontSendNotification);
    }
    if (abLoopEnabled)
    {
        double pos = playerAudio.getPosition();
        if (pos >= pointB)
            playerAudio.setPosition(pointA);
    }
}

