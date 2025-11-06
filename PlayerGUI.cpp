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
	auto& thumbnail = playerAudio.getThumbnail();
if (thumbnail.getNumChannels() == 0)
{
    g.setColour(juce::Colours::white);
    g.drawFittedText("No Audio Loaded", getLocalBounds(), juce::Justification::centred, 1);
    return;
}
g.setColour(juce::Colours::lightgreen);
auto waveformArea = getLocalBounds().reduced(20).withHeight(100).withY(200);
thumbnail.drawChannels(g, waveformArea, 0.0, thumbnail.getTotalLength(), 1.0f);
double pos = playerAudio.getPosition();
double length = playerAudio.getLength();
if (length > 0.0)
{
    float x = waveformArea.getX() + (pos / length) * waveformArea.getWidth();
    g.setColour(juce::Colours::yellow);
    g.drawLine(x, waveformArea.getY(), x, waveformArea.getBottom(), 2.0f);
  }
}

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton , &loopButton , &muteButton,  &setAButton, &setBButton,
        &play_pauseButton, &goToStartButton , &goToEndButton, &prevButton, &nextButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
	
	// Playlist ComboBox
	addAndMakeVisible(playListComboBox);
	playListComboBox.onChange = [this]() {
	    int selectedIndex = playListComboBox.getSelectedItemIndex();
	    if (selectedIndex >= 0 && selectedIndex < playListFiles.size())
	    {
	        currentFileIndex = selectedIndex;
			bool loaded = playerAudio.loadFile(playListFiles[currentFileIndex]);
	        if (loaded)
	        {
	            playerAudio.start();
	            fileInfoLabel.setText(playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
	        }
	        else {
	            fileInfoLabel.setText("Failed to load file: " + playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
	        }
	    }
	};

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
	 volumeSlider2.setRange(0.0, 1.0, 0.01);
     volumeSlider2.setValue(0.5);
     volumeSlider2.addListener(this);
     addAndMakeVisible(volumeSlider2);

	 // Label Information
	fileInfoLabel.setText("No File Loaded!", juce::dontSendNotification);
	fileInfoLabel.setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(fileInfoLabel);

     // Speed slider
    speedSlider.setRange(0.5, 2.0, 0.1);
	speedSlider.setValue(1.0);
	speedSlider.addListener(this);
	addAndMakeVisible(speedSlider);
	startTimerHz(30);
  timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
  timeLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(timeLabel);
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
	prevButton.setBounds(900, y, 80, 40);
	nextButton.setBounds(990, y, 80, 40);
    setAButton.setBounds(20, 250, 100, 40);
    setBButton.setBounds(140, 250, 100, 40);

    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

   volumeSlider.setBounds(20, 200, getWidth() - 40, 30);
volumeSlider2.setBounds(10, 250, getWidth() - 40, 30);
positionSlider.setBounds(20, 290, getWidth() - 40, 30);
fileInfoLabel.setBounds(20, 330, getWidth() - 40, 30);
playListComboBox.setBounds(20, 370, getWidth() - 40, 30);
speedSlider.setBounds(20, 410, getWidth() - 40, 30);
timeLabel.setBounds(20, 450, getWidth() - 40, 30);
trackInfoLabel.setBounds(20, 490, getWidth() - 40, 30);
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
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
				
				if (results.isEmpty())
				{
					fileInfoLabel.setText("No File Selected OR File not Found.", juce::dontSendNotification);
					return;
				}

				playListFiles.clear();
				for (auto& f : results)
				{
				    playListFiles.add(f);
				}

				playListComboBox.clear();
				for (int i = 0; i < playListFiles.size(); ++i)
				{
				    playListComboBox.addItem(playListFiles[i].getFileName(), i + 1); 
				}
				if (playListFiles.size() > 0)
				{
				    currentFileIndex = 0;
				    playListComboBox.setSelectedId(1);
				
					bool loaded = playerAudio.loadFile(playListFiles[currentFileIndex]);
				    if (loaded)
				    {
				        playerAudio.start();
				        fileInfoLabel.setText(playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
				    }
				    else {
						fileInfoLabel.setText("Failed to load file: " + playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
				    }
				}
				// metadata display    
				if (currentFileIndex >= 0 && currentFileIndex < playListFiles.size())
				{
				    juce::File file = playListFiles[currentFileIndex];
				    juce::AudioFormatManager formatManager;
				    formatManager.registerBasicFormats();
				    auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
				    juce::String Info;
				
				    if (reader != nullptr)
				    {
				        auto meta = reader->metadataValues;
				
				        juce::String title = meta.getValue("title", meta.getValue("Title", "Unknown Title!"));
				        juce::String author = meta.getValue("artist",
									          meta.getValue("Artist",
								              meta.getValue("Author",
				                    		  meta.getValue("Performer", "Unknown Author!"))));
				        double lengthInSeconds = playerAudio.getLength();
				
				        if (meta.size() == 0 || (title == "Unknown Title!" && author == "Unknown Author!"))
				        {
				            Info = "File: " + file.getFileName() + " | Duration: " + juce::String(lengthInSeconds, 2) + " sec";
				        }
				        else {
				            Info = "Title: " + title + "\nAuthor: " + author + "\nDuration: " + juce::String(lengthInSeconds, 2) + " sec";
				        }
				    }
				    else {
				        double lengthInSeconds = playerAudio.getLength();
				        Info = "File: " + file.getFileName() + " | Duration: " + juce::String(lengthInSeconds, 2) + " sec";
				    }
				    fileInfoLabel.setText(Info, juce::dontSendNotification);
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
	if (button == &nextButton)
	{
		playerAudio.playNext(playListFiles, currentFileIndex);
	    if (currentFileIndex >= 0 && currentFileIndex < playListFiles.size()) {
	        fileInfoLabel.setText(playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
	    }
	}
	if (button == &prevButton)
	{
		playerAudio.playPrevious(playListFiles, currentFileIndex);
	    if (currentFileIndex >= 0 && currentFileIndex < playListFiles.size()) {
	        fileInfoLabel.setText(playListFiles[currentFileIndex].getFileName(), juce::dontSendNotification);
	    }
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
	else if (slider == &volumeSlider2)
   {
    playerAudio.setGain2((float)volumeSlider2.getValue());
    }
	     if (slider == &speedSlider)
           playerAudio.setPlaybackSpeed(slider->getValue());
}

void PlayerGUI::timerCallback()
{
    double length = playerAudio.getLength();
	double pos = playerAudio.getPosition();

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

	 auto formatTime = [](double seconds)
     {
         int mins = int(seconds) / 60;
         int secs = int(seconds) % 60;
         return juce::String::formatted("%02d:%02d", mins, secs);
     };

 if (length > 0)
 {
     timeLabel.setText(formatTime(pos) + " / " + formatTime(length), juce::dontSendNotification);
 }

 repaint();
}






