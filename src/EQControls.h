#pragma once

#include <JuceHeader.h>
#include "Parameters.h"

class EQControls : public juce::Component
{
public:
    EQControls(Parameters& params);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Band
    {
        juce::String label;
        juce::Rectangle<int> rect;
        float value;
    };

    void parameterUpdated(int index, float value);

    Parameters& params;

    std::array<Band, 3> bands;

    juce::ParameterAttachment bassAttachment;
    juce::ParameterAttachment midsAttachment;
    juce::ParameterAttachment trebleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQControls)
};
