#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessor::AudioProcessor() :
    juce::AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout()),
    params(apvts)
{
    // do nothing
}

void AudioProcessor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    params.prepare(sampleRate);
    eq.prepare(float(sampleRate));
    reset();
}

void AudioProcessor::releaseResources()
{
    // do nothing
}

void AudioProcessor::reset()
{
    params.reset();
    eq.reset();
}

bool AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numInputChannels = getTotalNumInputChannels();
    auto numOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // Clear any output channels that don't contain input data.
    for (auto i = numInputChannels; i < numOutputChannels; ++i) {
        buffer.clear(i, 0, numSamples);
    }

    params.update();

    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample) {
        params.smoothen();

        eq.setBassGain(params.bass);
        eq.setMidsGain(params.mids);
        eq.setTrebleGain(params.treble);

        float sampleL = channelL[sample];
        float sampleR = channelR[sample];

        sampleL = eq.processSample(0, sampleL);
        sampleR = eq.processSample(1, sampleR);

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }
}

void AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessorEditor* AudioProcessor::createEditor()
{
    return new AudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioProcessor();
}
