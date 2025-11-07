#include "MainComponent.h"

MainComponent::MainComponent()
{
    
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(600, 400);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
     player2.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}
void MainComponent::resized()
{
    auto area = getLocalBounds();
    auto leftHalf = area.removeFromLeft(area.getWidth() / 2);
    player1.setBounds(leftHalf.reduced(10));
    player2.setBounds(area.reduced(10));
}






