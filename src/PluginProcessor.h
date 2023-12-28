#pragma once

#include <JuceHeader.h>
#include "StateVariableFilter.h"

namespace ParameterID
{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    PARAMETER_ID(bass)
    PARAMETER_ID(mids)
    PARAMETER_ID(treble)

    #undef PARAMETER_ID
}

class AudioProcessor : public juce::AudioProcessor
{
public:
    AudioProcessor();

    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override { }
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override { }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorEditor* createEditor() override;

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

    juce::AudioParameterFloat* bassParam;
    juce::AudioParameterFloat* midsParam;
    juce::AudioParameterFloat* trebleParam;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;
    void smoothen() noexcept;

    juce::LinearSmoothedValue<float> bassSmoother;
    juce::LinearSmoothedValue<float> midsSmoother;
    juce::LinearSmoothedValue<float> trebleSmoother;

    float bass, lastBass;
    float mids, lastMids;
    float treble, lastTreble;

    StateVariableFilter<float, 2> bassFilter;
    StateVariableFilter<float, 2> midsFilter1;
    StateVariableFilter<float, 2> midsFilter2;
    StateVariableFilter<float, 2> trebleFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
};
