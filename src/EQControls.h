#pragma once

#include <JuceHeader.h>
#include "Parameters.h"

class EQControls : public juce::Component
{
public:
    EQControls(Parameters& params);

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    struct Band
    {
        juce::String label;
        juce::Rectangle<int> rect;
        juce::Rectangle<int> innerRect;
        float value;
        juce::ParameterAttachment* attachment;
        juce::RangedAudioParameter* parameter;
    };

    void parameterUpdated(int index, float value);
    int indexOfBandAtPoint(const juce::Point<int>& point) const;
    void setBandValue(Band& band, float referenceValue, float pixelDistance, bool isDragging);

    Parameters& params;
    std::array<Band, 3> bands;
    Band* activeBand = nullptr;
    float startPos = 0.0f;
    float startValue = 0.0f;
    int lastUsedBand = 0;

    juce::ParameterAttachment bassAttachment;
    juce::ParameterAttachment midsAttachment;
    juce::ParameterAttachment trebleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQControls)
};
