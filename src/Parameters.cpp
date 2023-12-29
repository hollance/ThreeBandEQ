#include <JuceHeader.h>
#include "Parameters.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);  // parameter does not exist or wrong type
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts_) : apvts(apvts_)
{
    castParameter(apvts, ParameterID::bass, bassParam);
    castParameter(apvts, ParameterID::mids, midsParam);
    castParameter(apvts, ParameterID::treble, trebleParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::bass,
        "Bass",
        juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecibels)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::mids,
        "Mids",
        juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecibels)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::treble,
        "Treble",
        juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecibels)));

    return layout;
}

void Parameters::prepare(double sampleRate) noexcept
{
    double smoothTime = 0.02;
    bassSmoother.reset(sampleRate, smoothTime);
    midsSmoother.reset(sampleRate, smoothTime);
    trebleSmoother.reset(sampleRate, smoothTime);
}

void Parameters::reset() noexcept
{
    bassSmoother.setCurrentAndTargetValue(bassParam->get());
    midsSmoother.setCurrentAndTargetValue(midsParam->get());
    trebleSmoother.setCurrentAndTargetValue(trebleParam->get());
}

void Parameters::update() noexcept
{
    bassSmoother.setTargetValue(bassParam->get());
    midsSmoother.setTargetValue(midsParam->get());
    trebleSmoother.setTargetValue(trebleParam->get());
}

void Parameters::smoothen() noexcept
{
    bass = bassSmoother.getNextValue();
    mids = midsSmoother.getNextValue();
    treble = trebleSmoother.getNextValue();
}
