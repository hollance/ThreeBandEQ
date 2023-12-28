#pragma once

#include "StateVariableFilter.h"

template <typename SampleType, int NumChannels>
class ThreeBandEQ
{
public:
    void prepare(SampleType newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    void reset() noexcept
    {
        bassFilter.reset();
        midsFilter1.reset();
        midsFilter2.reset();
        trebleFilter.reset();

        bass = SampleType(-999.0);
        mids = SampleType(-999.0);
        treble = SampleType(-999.0);
    }

    void setBassGain(SampleType dbGain) noexcept
    {
        if (dbGain != bass) {
            bass = dbGain;
            bassFilter.lowShelf(sampleRate, lowFreq, Q, bass);
        }
    }

    void setMidsGain(SampleType dbGain) noexcept
    {
        if (dbGain != mids) {
            mids = dbGain;
            midsFilter1.highShelf(sampleRate, lowFreq, Q, mids);
            midsFilter2.highShelf(sampleRate, highFreq, Q, -mids);
        }
    }

    void setTrebleGain(SampleType dbGain) noexcept
    {
        if (dbGain != treble) {
            treble = dbGain;
            trebleFilter.highShelf(sampleRate, highFreq, Q, treble);
        }
    }

    SampleType processSample(int channel, SampleType sample) noexcept
    {
        sample = bassFilter.processSample(channel, sample);
        sample = midsFilter1.processSample(channel, sample);
        sample = midsFilter2.processSample(channel, sample);
        sample = trebleFilter.processSample(channel, sample);
        return sample;
    }

private:
    static constexpr SampleType lowFreq = SampleType(220.0);
    static constexpr SampleType highFreq = SampleType(2200.0);
    static constexpr SampleType Q = SampleType(0.6);

    SampleType sampleRate;
    SampleType bass;
    SampleType mids;
    SampleType treble;

    StateVariableFilter<SampleType, NumChannels> bassFilter;
    StateVariableFilter<SampleType, NumChannels> midsFilter1;
    StateVariableFilter<SampleType, NumChannels> midsFilter2;
    StateVariableFilter<SampleType, NumChannels> trebleFilter;
};
