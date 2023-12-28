#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioProcessorEditor(AudioProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
};
