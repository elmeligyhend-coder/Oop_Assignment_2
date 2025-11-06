#include "PlayerAudio.h"
PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
	 resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&mixer, false, 2);
      mixer.addInputSource(&transportSource, false);   
      mixer.addInputSource(&transportSource2, false);
}
PlayerAudio::~PlayerAudio()
{
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
	transportSource2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
	 resamplingSource->releaseResources();
     thumbnail.clear();
}
bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
             thumbnail.setSource(new juce::FileInputSource(file)); 
            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            return true;
        }
    }
    return false;
}
void PlayerAudio::start()
{
    transportSource.start();
	transportSource2.start();
}
void PlayerAudio::stop()
{
  transportSource.stop();
  transportSource2.stop();
  transportSource.setPosition(0);
  transportSource2.setPosition(0);
}
void PlayerAudio::restart()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
    transportSource.start();
}
void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
if (readerSource) readerSource->setLooping(looping);
if (readerSource2) readerSource2->setLooping(looping);
   
}

bool PlayerAudio::isLooping() const
{
    return looping;
}
void PlayerAudio::playOrpause()
{
     if (transportSource.isPlaying() || transportSource2.isPlaying())
    {
     stop();
    }
   else
   {
     start();
    }
}
void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
	 transportSource2.setPosition(0.0);
}
void PlayerAudio::goToEnd()
{
    double length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
	 transportSource2.setPosition(transportSource2.getLengthInSeconds());
}
void PlayerAudio::playNext(const juce::Array<juce::File>& playlistFiles, int& currentFileIndex)
{
    if (playlistFiles.isEmpty()) return;
    currentFileIndex = (currentFileIndex + 1) % playlistFiles.size();
    if (loadFile(playlistFiles[currentFileIndex]))
    {
        transportSource.setPosition(0.0);
        start();
    }
}

void PlayerAudio::playPrevious(const juce::Array<juce::File>& playlistFiles, int& currentFileIndex)
{
    if (playlistFiles.isEmpty()) return;
    currentFileIndex = (currentFileIndex - 1 + playlistFiles.size()) % playlistFiles.size();
    if (loadFile(playlistFiles[currentFileIndex]))
    {
        transportSource.setPosition(0.0);
        start();
	}
}

void PlayerAudio::switcherMute()
{
    if (isMuted)
    {
        setGain(lastGain);
        isMuted = false;
    }
    else
    {
        lastGain = 0.5f;
        setGain(0.0f);
        isMuted = true;
    }
}
void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}
void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
	 transportSource2.setPosition(pos);
}
double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::setPlaybackSpeed(double speed)
{
    playbackSpeed = juce::jlimit(0.5, 2.0, speed); 
    resamplingSource->setResamplingRatio(playbackSpeed);
}
bool PlayerAudio::loadFile2(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource2.stop();
            transportSource2.setSource(nullptr);
            readerSource2.reset();

            readerSource2 = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource2.setSource(readerSource2.get(), 0, nullptr, reader->sampleRate);
        }
    }
    return true;
}
void PlayerAudio::setGain2(float gain)
{
    gain2 = gain; 
}



