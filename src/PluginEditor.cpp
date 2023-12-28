#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p) :
    juce::AudioProcessorEditor(&p),
    audioProcessor(p)
{
    setOpaque(true);
    setSize(400, 200);
}

void AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void AudioProcessorEditor::resized()
{
}
