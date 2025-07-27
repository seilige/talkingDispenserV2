#define _USE_MATH_DEFINES
#include <cmath>
#include "vowel_detector.h"
#include <algorithm>
#include <cmath>
#include <iostream>

VowelDetector::VowelDetector() {}
std::string VowelDetector::detectVowel(const std::vector<short>& audioData, int sampleRate) {
    if (audioData.size() < 2048) return ""; // Increase the minimum size of the input data
    
    // Apply pre-filtering - take only the central part of the audio data
    size_t start = audioData.size() / 4;
    size_t end = audioData.size() * 3 / 4;
    std::vector<short> centeredData(audioData.begin() + start, audioData.begin() + end);
    
    // Apply a windowing function to the data
    std::vector<double> windowedData = applyWindow(centeredData);
    
    // Check if the signal is silent
    if (windowedData.empty() || isSilence(windowedData)) {
        // Do not clear the buffer immediately, instead add an empty value
        recentDetections.push_back("");
        if (recentDetections.size() > maxRecentDetections) {
            recentDetections.erase(recentDetections.begin());
        }
        return getConsistentVowel();
    }
    
    // Perform FFT (Fast Fourier Transform) on the windowed data
    std::vector<std::complex<double>> fftData = fft(windowedData);
    
    // Compute the magnitude spectrum from the FFT result
    std::vector<double> spectrum = getMagnitudeSpectrum(fftData);
    
    // Classify the vowel sound based on the spectrum
    std::string detected = classifyVowel(spectrum, sampleRate);
    
    // Add the result to the buffer of recent detections
    recentDetections.push_back(detected);
    if (recentDetections.size() > maxRecentDetections) {
        recentDetections.erase(recentDetections.begin());
    }
    
    // Return a consistent vowel result based on recent detections
    return getConsistentVowel();
}

std::vector<double> VowelDetector::applyWindow(const std::vector<short>& data) {
    std::vector<double> windowed(data.size());
    double totalEnergy = 0;
    
    for (size_t i = 0; i < data.size(); i++) {
        // Apply the Hamming window function
        double window = 0.54 - 0.46 * cos(2.0 * M_PI * i / (data.size() - 1));
        windowed[i] = data[i] * window;
        totalEnergy += windowed[i] * windowed[i];
    }
    
    // Check the energy of the signal
    if (totalEnergy < minEnergyThreshold) {
        return {}; // Signal is too quiet
    }
    
    return windowed;
}

std::vector<std::complex<double>> VowelDetector::fft(const std::vector<double>& data) {
    size_t N = data.size();
    std::vector<std::complex<double>> result(N);
    
    // Simple implementation of the Discrete Fourier Transform (DFT) for small sizes
    for (size_t k = 0; k < N; k++) {
        std::complex<double> sum(0, 0);
        for (size_t n = 0; n < N; n++) {
            double angle = -2.0 * M_PI * k * n / N;
            sum += data[n] * std::complex<double>(cos(angle), sin(angle));
        }
        result[k] = sum;
    }
    
    return result;
}

std::vector<double> VowelDetector::getMagnitudeSpectrum(const std::vector<std::complex<double>>& fftData) {
    std::vector<double> spectrum(fftData.size() / 2);
    
    for (size_t i = 0; i < spectrum.size(); i++) {
        spectrum[i] = std::abs(fftData[i]);
    }
    
    return spectrum;
}

std::string VowelDetector::classifyVowel(const std::vector<double>& spectrum, int sampleRate) {
    if (spectrum.empty()) return "";
    
    double freqStep = (double)sampleRate / (2.0 * spectrum.size());
    
    // Find all peaks in the spectrum that are above a certain threshold
    std::vector<std::pair<double, double>> peaks;
    double maxAmplitude = *std::max_element(spectrum.begin(), spectrum.end());
    double threshold = maxAmplitude * 0.05; // Lower the threshold to 5%

    for (size_t i = 2; i < spectrum.size() - 2; i++) {
        if (spectrum[i] > spectrum[i-1] && spectrum[i] > spectrum[i+1] && 
            spectrum[i] > spectrum[i-2] && spectrum[i] > spectrum[i+2] &&
            spectrum[i] > threshold) {
            double freq = i * freqStep;
            if (freq >= 150 && freq <= 4000) { // Expand the frequency range
                peaks.push_back({freq, spectrum[i]});
            }
        }
    }
    
    if (peaks.empty()) return "";
    
    // Sort the peaks by amplitude in descending order
    std::sort(peaks.begin(), peaks.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Take up to 4 strongest peaks for analysis
    int numPeaks = std::min(4, (int)peaks.size());
    
    // Identify F1 and F2 formants
    double f1 = 0, f2 = 0;
    double f1_amp = 0, f2_amp = 0;
    
    for (int i = 0; i < numPeaks; i++) {
        double freq = peaks[i].first;
        double amp = peaks[i].second;
        
        // F1 - search in the range of 200-1000 Hz
        if (freq >= 200 && freq <= 1000 && amp > f1_amp) {
            f1 = freq;
            f1_amp = amp;
        }
        
        // F2 - search in the range of 800-3500 Hz
        if (freq >= 800 && freq <= 3500 && amp > f2_amp) {
            f2 = freq;
            f2_amp = amp;
        }
    }
    
    if (f1 == 0 || f2 == 0) return "";
    
    std::cout << "F1=" << f1 << "Hz, F2=" << f2 << "Hz" << std::endl;
    
    // Improved classification with weights for all vowels
    double score_a = 0, score_ya = 0, score_e = 0, score_ye = 0;
    double score_i = 0, score_y = 0, score_o = 0, score_yo = 0;
    double score_u = 0, score_yu = 0;

    // Vowel "а": F1=700-850, F2=1100-1300
    if (f1 >= 650 && f1 <= 900 && f2 >= 1000 && f2 <= 1500) {
        score_a = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 700 && f1 <= 850 && f2 >= 1100 && f2 <= 1300) {
            score_a *= 1.5;
        }
    }

    // Vowel "я": similar to "а" but with slightly different characteristics
    if (f1 >= 600 && f1 <= 850 && f2 >= 1200 && f2 <= 1600) {
        score_ya = (f1_amp + f2_amp) * 0.4;
        if (f1 >= 650 && f1 <= 800 && f2 >= 1300 && f2 <= 1500) {
            score_ya *= 1.3;
        }
    }

    // Vowel "э": F1=500-650, F2=1400-1800
    if (f1 >= 450 && f1 <= 700 && f2 >= 1300 && f2 <= 2000) {
        score_e = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 500 && f1 <= 650 && f2 >= 1400 && f2 <= 1800) {
            score_e *= 1.5;
        }
    }

    // Vowel "е": similar to "э"
    if (f1 >= 400 && f1 <= 650 && f2 >= 1500 && f2 <= 2100) {
        score_ye = (f1_amp + f2_amp) * 0.4;
        if (f1 >= 450 && f1 <= 600 && f2 >= 1600 && f2 <= 2000) {
            score_ye *= 1.3;
        }
    }

    // Vowel "и": F1=250-400, F2=2000-2800
    if (f1 >= 200 && f1 <= 450 && f2 >= 1800 && f2 <= 3000) {
        score_i = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 250 && f1 <= 400 && f2 >= 2000 && f2 <= 2800) {
            score_i *= 1.5;
        }
    }

    // Vowel "ы": F1=350-500, F2=1200-1600
    if (f1 >= 300 && f1 <= 550 && f2 >= 1100 && f2 <= 1700) {
        score_y = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 350 && f1 <= 500 && f2 >= 1200 && f2 <= 1600) {
            score_y *= 1.5;
        }
    }

    // Vowel "о": F1=450-650, F2=850-1200  
    if (f1 >= 400 && f1 <= 700 && f2 >= 800 && f2 <= 1300) {
        score_o = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 450 && f1 <= 650 && f2 >= 850 && f2 <= 1200) {
            score_o *= 1.5;
        }
    }

    // Vowel "ё": similar to "о" but slightly higher
    if (f1 >= 400 && f1 <= 650 && f2 >= 900 && f2 <= 1400) {
        score_yo = (f1_amp + f2_amp) * 0.4;
        if (f1 >= 450 && f1 <= 600 && f2 >= 1000 && f2 <= 1300) {
            score_yo *= 1.3;
        }
    }

    // Vowel "у": F1=300-450, F2=600-1000
    if (f1 >= 250 && f1 <= 500 && f2 >= 550 && f2 <= 1100) {
        score_u = (f1_amp + f2_amp) * 0.5;
        if (f1 >= 300 && f1 <= 450 && f2 >= 600 && f2 <= 1000) {
            score_u *= 1.5;
        }
    }

    // Vowel "ю": similar to "у" but with a higher F2
    if (f1 >= 250 && f1 <= 450 && f2 >= 800 && f2 <= 1300) {
        score_yu = (f1_amp + f2_amp) * 0.4;
        if (f1 >= 300 && f1 <= 400 && f2 >= 900 && f2 <= 1200) {
            score_yu *= 1.3;
        }
    }

    // Lower the minimum threshold for classification
    double minScore = maxAmplitude * 0.02;

    // Find the vowel with the highest score
    double maxScore = std::max({score_a, score_ya, score_e, score_ye, score_i, 
                               score_y, score_o, score_yo, score_u, score_yu});

    if (maxScore < minScore) {
        return "";
    }

    // Return the vowel with the highest score
    if (maxScore == score_a) return "а";
    else if (maxScore == score_ya) return "я";
    else if (maxScore == score_e) return "э";
    else if (maxScore == score_ye) return "е";
    else if (maxScore == score_i) return "и";
    else if (maxScore == score_y) return "ы";
    else if (maxScore == score_o) return "о";
    else if (maxScore == score_yo) return "ё";
    else if (maxScore == score_u) return "у";
    else if (maxScore == score_yu) return "ю";

    return "";
}

bool VowelDetector::isSilence(const std::vector<double>& windowedData) {
    double energy = 0;
    for (double sample : windowedData) {
        energy += sample * sample;
    }
    return energy < silenceThreshold;
}
std::string VowelDetector::getConsistentVowel() {
    if (recentDetections.empty()) {
        return "";
    }
    
    // Count only the most recent detections
    int count_a = 0, count_ya = 0, count_e = 0, count_ye = 0;
    int count_i = 0, count_y = 0, count_o = 0, count_yo = 0;
    int count_u = 0, count_yu = 0;
    int recentCount = std::min(4, (int)recentDetections.size());
    
    for (int i = recentDetections.size() - recentCount; i < (int)recentDetections.size(); i++) {
        if (recentDetections[i] == "а") count_a++;
        else if (recentDetections[i] == "я") count_ya++;
        else if (recentDetections[i] == "э") count_e++;
        else if (recentDetections[i] == "е") count_ye++;
        else if (recentDetections[i] == "и") count_i++;
        else if (recentDetections[i] == "ы") count_y++;
        else if (recentDetections[i] == "о") count_o++;
        else if (recentDetections[i] == "ё") count_yo++;
        else if (recentDetections[i] == "у") count_u++;
        else if (recentDetections[i] == "ю") count_yu++;
    }
    
    // If there is at least one detection among the last 4, return it
    if (count_a >= 1) return "а";
    if (count_ya >= 1) return "я";
    if (count_e >= 1) return "э";
    if (count_ye >= 1) return "е";
    if (count_i >= 1) return "и";
    if (count_y >= 1) return "ы";
    if (count_o >= 1) return "о";
    if (count_yo >= 1) return "ё";
    if (count_u >= 1) return "у";
    if (count_yu >= 1) return "ю";
 
    return "";
}