#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EQControls.h"

class AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioProcessorEditor(AudioProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioProcessor& audioProcessor;
    EQControls controls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
};
