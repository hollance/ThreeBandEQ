#include "PluginProcessor.h"
#include "PluginEditor.h"

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

AudioProcessor::AudioProcessor() :
    juce::AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        )
{
    castParameter(apvts, ParameterID::bass, bassParam);
    castParameter(apvts, ParameterID::mids, midsParam);
    castParameter(apvts, ParameterID::treble, trebleParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioProcessor::createParameterLayout()
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

void AudioProcessor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    double smoothTime = 0.02;
    bassSmoother.reset(sampleRate, smoothTime);
    midsSmoother.reset(sampleRate, smoothTime);
    trebleSmoother.reset(sampleRate, smoothTime);

    reset();
}

void AudioProcessor::releaseResources()
{
}

void AudioProcessor::reset()
{
    bassSmoother.setCurrentAndTargetValue(bassParam->get());
    midsSmoother.setCurrentAndTargetValue(midsParam->get());
    trebleSmoother.setCurrentAndTargetValue(trebleParam->get());

    bassFilter.reset();
    midsFilter1.reset();
    midsFilter2.reset();
    trebleFilter.reset();

    lastBass = -999.0f;
    lastMids = -999.0f;
    lastTreble = -999.0f;
}

bool AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioProcessor::update() noexcept
{
    bassSmoother.setTargetValue(bassParam->get());
    midsSmoother.setTargetValue(midsParam->get());
    trebleSmoother.setTargetValue(trebleParam->get());
}

void AudioProcessor::smoothen() noexcept
{
    float sampleRate = float(getSampleRate());

    constexpr float lowFreq = 220.0f;
    constexpr float highFreq = 2200.0f;
    constexpr float Q = 0.6f;

    bass = bassSmoother.getNextValue();
    if (bass != lastBass) {
        bassFilter.lowShelf(sampleRate, lowFreq, Q, bass);
        lastBass = bass;
    }

    mids = midsSmoother.getNextValue();
    if (mids != lastMids) {
        midsFilter1.highShelf(sampleRate, lowFreq, Q, mids);
        midsFilter2.highShelf(sampleRate, highFreq, Q, -mids);
        lastMids = mids;
    }

    treble = trebleSmoother.getNextValue();
    if (treble != lastTreble) {
        trebleFilter.highShelf(sampleRate, highFreq, Q, treble);
        lastTreble = treble;
    }
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

    update();

    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample) {
        smoothen();

        float sampleL = channelL[sample];
        float sampleR = channelR[sample];

        sampleL = bassFilter.processSample(0, sampleL);
        sampleL = midsFilter1.processSample(0, sampleL);
        sampleL = midsFilter2.processSample(0, sampleL);
        sampleL = trebleFilter.processSample(0, sampleL);

        sampleR = bassFilter.processSample(1, sampleR);
        sampleR = midsFilter1.processSample(1, sampleR);
        sampleR = midsFilter2.processSample(1, sampleR);
        sampleR = trebleFilter.processSample(1, sampleR);

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
