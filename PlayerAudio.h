#pragma once // PlayerAudio.h
#include <JuceHeader.h>
#include <juce_audio_utils/juce_audio_utils.h>
class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
     bool loadFile2(const juce::File& file);
 void setGain2(float gain);
    bool loadFile(const juce::File& file);
    void start();
    void stop();
	void restart();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void setLooping(bool shouldLoop);
    bool isLooping() const;
    void playOrpause();
    void goToStart();
    void goToEnd();
	void playNext(const juce::Array<juce::File>& playlistFiles, int& currentFileIndex);
	void playPrevious(const juce::Array<juce::File>& playlistFiles, int& currentFileIndex);
    bool isMuted = false;
    float lastGain = 0.5f;
    void switcherMute();
    void setPlaybackSpeed(double speed);
juce::AudioThumbnail& getThumbnail() { return thumbnail; }
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource; 
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource2;
    juce::AudioTransportSource transportSource2;
    juce::MixerAudioSource mixer;
    float gain1 = 1.0f; 
    float gain2 = 1.0f; 
    juce::AudioThumbnailCache thumbnailCache{ 5 };  
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };
    double playbackSpeed = 1.0;
    bool looping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};




