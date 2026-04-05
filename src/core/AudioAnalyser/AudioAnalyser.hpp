#pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>

class AudioAnalyser
{
public:
    /**
     * @brief Represents the data of a single frequency component in a signal.
     *
     * @details
     * Each FrequencyData instance encapsulates the essential parameters describing a sinusoidal component of a signal in the frequency domain.
     * - amplitude: The magnitude of the frequency component, indicating its contribution strength to the overall signal.
     * - phase: The phase shift of the frequency component in radians, representing the horizontal offset of the sinusoid relative to time zero.
     * - frequency: The frequency in Hertz (Hz), specifying how many cycles per second the sinusoidal component completes.
     *
     * This struct is fundamental for both Discrete Fourier Transform (DFT) and Fast Fourier Transform (FFT) representations,
     * as these transforms decompose time-domain signals into sums of such sinusoidal components.
     */
    struct FrequencyData
    {
        float amplitude;
        float phase;
        float frequency;
    };

    AudioAnalyser();

    /**
     * @brief Decomposes a time-domain audio signal into its frequency components using the Discrete Fourier Transform (DFT).
     *
     * @details
     * The Discrete Fourier Transform converts a sequence of N time-domain samples into N frequency-domain components.
     * Each output frequency bin corresponds to a complex number encoding amplitude and phase information of a sinusoidal component.
     * Mathematically, the DFT is defined as:
     * \f[
     * X_k = \sum_{n=0}^{N-1} x_n e^{-2\pi i k n / N}
     * \f]
     * where \f$x_n\f$ are the input samples, and \f$X_k\f$ are the frequency components.
     *
     * This function calculates the amplitude as the magnitude of \f$X_k\f$, the phase as the argument (angle) of \f$X_k\f$,
     * and the frequency as \f$k \times \frac{sampleRate}{N}\f$ for each frequency bin k.
     * According to the Nyquist-Shannon sampling theorem, the unique frequency components range from 0 Hz to half the sampling rate,
     * thus only the first \f$\frac{N}{2} + 1\f$ bins are returned.
     *
     * The computational complexity of this naive DFT implementation is O(N^2), where N is the number of samples.
     *
     * @param samples A vector of float values representing the audio samples in the time domain.
     * @param sampleRate The sampling rate of the audio signal in Hz.
     *
     * @return std::vector<FrequencyData> A vector containing the frequency components, each with amplitude, phase, and frequency information.
     */
    static std::vector<FrequencyData>
    discreteFourierTransform(const std::vector<float> &samples, int sampleRate);

    /**
     * @brief Reconstructs a time-domain audio signal from its frequency components.
     *
     * @details
     * This function performs the inverse operation of the DFT, synthesizing a time-domain waveform by summing sinusoidal waves
     * defined by the frequency components' amplitude, phase, and frequency.
     * The reconstruction formula is:
     * \f[
     * x_n = \sum_{k=0}^{M-1} A_k \cos(2 \pi f_k \frac{n}{sampleRate} + \phi_k)
     * \f]
     * where \f$A_k\f$, \f$f_k\f$, and \f$\phi_k\f$ are the amplitude, frequency, and phase of the k-th component, respectively,
     * and \f$n\f$ is the sample index.
     *
     * The number of samples produced is specified by numSamples, and the output approximates the original signal within that length.
     *
     * @param spectrum A vector of FrequencyData representing the frequency components of the signal.
     * @param numSamples The number of samples to generate in the reconstructed signal.
     * @param sampleRate The sampling rate of the audio signal in Hz.
     *
     * @return std::vector<float> A vector of float values representing the reconstructed audio samples in the time domain.
     */
    static std::vector<float>
    reconstructDFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate);

    /**
     * @brief Recursive implementation of the Fast Fourier Transform (FFT) algorithm.
     *
     * @details
     * The FFT efficiently computes the Discrete Fourier Transform by recursively dividing the polynomial representation of the input
     * into even and odd indexed coefficients, exploiting symmetries to reduce the complexity from O(N^2) to O(N log N).
     *
     * The input polynomial vector represents complex coefficients of a polynomial:
     * \f[
     * P(x) = \sum_{n=0}^{N-1} a_n x^n
     * \f]
     * The FFT evaluates this polynomial at the N-th roots of unity:
     * \f[
     * \omega_N = e^{2\pi i / N}
     * \f]
     * If invert is false, the function transforms from coefficient to value representation (forward FFT).
     * If invert is true, it performs the inverse FFT, converting from value back to coefficient representation,
     * including normalization by dividing by N.
     *
     * This recursive divide-and-conquer approach splits the polynomial into two halves:
     * - Even-indexed coefficients form one sub-polynomial.
     * - Odd-indexed coefficients form another sub-polynomial.
     *
     * The results of recursive calls are combined using the roots of unity to produce the final transformed vector.
     *
     * @param polynomial Vector of complex coefficients representing the polynomial.
     * @param invert Boolean flag indicating whether to perform inverse transform (true) or forward transform (false).
     *
     * @return std::vector<std::complex<float>> The transformed polynomial in value or coefficient form depending on invert flag.
     */
    static std::vector<std::complex<float>>
    fftRecursive(std::vector<std::complex<float>> &polynomial, bool invert);

    /**
     * @brief Computes the frequency spectrum of a signal using the Fast Fourier Transform (FFT).
     *
     * @details
     * This function assumes the input sample size is a power of two, which is a requirement for the radix-2 Cooley-Tukey FFT algorithm.
     * The FFT converts the time-domain samples into their frequency-domain representation efficiently in O(N log N) time.
     *
     * The output frequency components are derived from the complex FFT output by calculating amplitude and phase similarly to the DFT.
     * Frequencies correspond to bins spaced at \f$\frac{sampleRate}{N}\f$ Hz intervals.
     *
     * This method is preferred over the naive DFT for large input sizes due to its significant performance improvement.
     *
     * @param samples Vector of float audio samples in the time domain, size must be a power of two.
     * @param sampleRate Sampling rate of the audio signal in Hz.
     *
     * @return std::vector<FrequencyData> Vector of frequency components with amplitude, phase, and frequency.
     */
    static std::vector<FrequencyData>
    fastFourierTransform(const std::vector<float> &samples, int sampleRate);

    /**
     * @brief Reconstructs a time-domain signal from its FFT-derived frequency spectrum.
     *
     * @details
     * This function performs the inverse FFT operation by summing sinusoidal components specified in the spectrum,
     * similar to the reconstructDFT function but optimized for FFT results.
     * It generates a time-domain signal with numSamples samples at the given sample rate.
     *
     * The reconstruction formula is:
     * \f[
     * x_n = \sum_{k=0}^{M-1} A_k \cos(2 \pi f_k \frac{n}{sampleRate} + \phi_k)
     * \f]
     * where \f$A_k\f$, \f$f_k\f$, and \f$\phi_k\f$ are amplitude, frequency, and phase of each component.
     *
     * This method is useful for signal synthesis, filtering, or analysis when working with FFT data.
     *
     * @param spectrum Vector of FrequencyData representing the FFT frequency components.
     * @param numSamples Number of samples to generate in the reconstructed time-domain signal.
     * @param sampleRate Sampling rate of the audio signal in Hz.
     *
     * @return std::vector<float> Vector of reconstructed audio samples in the time domain.
     */
    static std::vector<float>
    reconstructFFT(const std::vector<FrequencyData> &spectrum, int numSamples, int sampleRate);

private:
};