#ifndef VOWEL_DETECTOR_H
#define VOWEL_DETECTOR_H

#include <vector>
#include <string>
#include <complex>

class VowelDetector {
public:
    VowelDetector();
    std::string detectVowel(const std::vector<short>& audioData, int sampleRate = 16000);
    
private:
    std::vector<double> applyWindow(const std::vector<short>& data);
    std::vector<std::complex<double>> fft(const std::vector<double>& data);
    std::vector<double> getMagnitudeSpectrum(const std::vector<std::complex<double>>& fftData);
    std::string classifyVowel(const std::vector<double>& spectrum, int sampleRate);
    
    struct FormantRanges {
        double f1_min, f1_max;  // First formant frequency range
        double f2_min, f2_max;  // Second formant frequency range
    };

    // Formant frequency ranges for different vowels
    FormantRanges vowel_a = {700, 850, 1100, 1300};   // "a"
    FormantRanges vowel_ya = {650, 800, 1300, 1500};  // "ya"
    FormantRanges vowel_e = {500, 650, 1400, 1800};   // "e"
    FormantRanges vowel_ye = {450, 600, 1600, 2000};  // "ye"
    FormantRanges vowel_i = {250, 400, 2000, 2800};   // "i"
    FormantRanges vowel_y = {350, 500, 1200, 1600};   // "y"
    FormantRanges vowel_o = {450, 650, 850, 1200};    // "o"
    FormantRanges vowel_yo = {450, 600, 1000, 1300};  // "yo"
    FormantRanges vowel_u = {300, 450, 600, 1000};    // "u"
    FormantRanges vowel_yu = {300, 400, 900, 1200};   // "yu"

    double minEnergyThreshold = 50000.0; // Minimum energy level required to detect a vowel

    double silenceThreshold = 10000.0; // Threshold below which the signal is considered silence
    int minConsistentFrames = 2;       // Minimum number of consistent frames required to confirm a vowel
    std::vector<std::string> recentDetections; // Buffer to store recent vowel detections
    size_t maxRecentDetections = 4;    // Maximum size of the recent detections buffer
    
    // Additional methods:
    bool isSilence(const std::vector<double>& windowedData); // Check if the given data represents silence
    std::string getConsistentVowel(); // Retrieve the most consistently detected vowel
};

#endif