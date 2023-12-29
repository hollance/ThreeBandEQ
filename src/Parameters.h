#pragma once

namespace ParameterID
{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    PARAMETER_ID(bass)
    PARAMETER_ID(mids)
    PARAMETER_ID(treble)

    #undef PARAMETER_ID
}

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepare(double sampleRate) noexcept;
    void reset() noexcept;
    void update() noexcept;
    void smoothen() noexcept;

    float bass;
    float mids;
    float treble;

    juce::AudioParameterFloat* bassParam;
    juce::AudioParameterFloat* midsParam;
    juce::AudioParameterFloat* trebleParam;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::LinearSmoothedValue<float> bassSmoother;
    juce::LinearSmoothedValue<float> midsSmoother;
    juce::LinearSmoothedValue<float> trebleSmoother;
};
