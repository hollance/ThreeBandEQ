#pragma once

#include <cmath>

/**
  State variable filter (SVF), designed by Andrew Simper of Cytomic.

  http://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
template <typename T, int NumChannels>
class StateVariableFilter
{
public:
    StateVariableFilter() : m0(0.0), m1(0.0), m2(0.0) { }

    void lowShelf(T sampleRate, T freq, T Q, T dbGain) noexcept
    {
        T A = std::exp(T(0.0575646273248511421) * dbGain);
        g = std::tan(pi * freq / sampleRate) / std::sqrt(A);
        k = T(1.0) / Q;
        a1 = T(1.0) / (T(1.0) + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = k * (A - T(1.0));
        m2 = A*A - T(1.0);
    }

    void highShelf(T sampleRate, T freq, T Q, T dbGain) noexcept
    {
        T A = std::exp(T(0.0575646273248511421) * dbGain);
        g = std::tan(pi * freq / sampleRate) * std::sqrt(A);
        k = T(1.0) / Q;
        a1 = T(1.0) / (T(1.0) + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = A * A;
        m1 = k * (T(1.0) - A) * A;
        m2 = T(1.0) - A*A;
    }

    void reset() noexcept
    {
        for (int channel = 0; channel < NumChannels; ++channel) {
            ic1eq[channel] = 0.0;
            ic2eq[channel] = 0.0;
        }
    }

    T processSample(int channel, T v0) noexcept
    {
        T v3 = v0 - ic2eq[channel];
        T v1 = a1 * ic1eq[channel] + a2 * v3;
        T v2 = ic2eq[channel] + a2 * ic1eq[channel] + a3 * v3;
        ic1eq[channel] = T(2.0) * v1 - ic1eq[channel];
        ic2eq[channel] = T(2.0) * v2 - ic2eq[channel];
        return m0 * v0 + m1 * v1 + m2 * v2;
    }

private:
    static constexpr T pi = static_cast<T>(3.14159265358979323846264338327950288);

    T g, k, a1, a2, a3;    // filter coefficients
    T m0, m1, m2;          // mix coefficients
    T ic1eq[NumChannels];  // internal state
    T ic2eq[NumChannels];
};
