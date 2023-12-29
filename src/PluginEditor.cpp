#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p) :
    juce::AudioProcessorEditor(&p),
    audioProcessor(p),
    controls(p.params)
{
    addAndMakeVisible(controls);
    setOpaque(true);
    setSize(300, 150);
}

void AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void AudioProcessorEditor::resized()
{
    controls.setBounds(getLocalBounds());
}
