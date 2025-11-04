#include "PlayerAudio.h"
PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
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

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
        }
    }
    return true;
}
void PlayerAudio::start()
{
    transportSource.start();
}
void PlayerAudio::stop()
{
    transportSource.stop();
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

    if (readerSource != nullptr)
        readerSource->setLooping(looping);
}

bool PlayerAudio::isLooping() const
{
    return looping;
}
void PlayerAudio::playOrpause()
{
    static bool isPlaying = false;
    if (isPlaying) {
        transportSource.start();
    }
    else {
        transportSource.stop();
    }
    isPlaying = !isPlaying;
}
void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
}
void PlayerAudio::goToEnd()
{
    double length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
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
}
double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}
