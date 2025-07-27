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
        double f1_min, f1_max;  // Первая форманта
        double f2_min, f2_max;  // Вторая форманта
    };

    FormantRanges vowel_a = {700, 850, 1100, 1300};   // "а"
    FormantRanges vowel_ya = {650, 800, 1300, 1500};  // "я"
    FormantRanges vowel_e = {500, 650, 1400, 1800};   // "э"
    FormantRanges vowel_ye = {450, 600, 1600, 2000};  // "е"
    FormantRanges vowel_i = {250, 400, 2000, 2800};   // "и"
    FormantRanges vowel_y = {350, 500, 1200, 1600};   // "ы"
    FormantRanges vowel_o = {450, 650, 850, 1200};    // "о"
    FormantRanges vowel_yo = {450, 600, 1000, 1300};  // "ё"
    FormantRanges vowel_u = {300, 450, 600, 1000};    // "у"
    FormantRanges vowel_yu = {300, 400, 900, 1200};   // "ю"

    double minEnergyThreshold = 50000.0; // 1000.0;  // Минимальный уровень энергии

    double silenceThreshold = 10000.0; // 500.0;    // Порог тишины
    int minConsistentFrames = 2;        // Минимум кадров для подтверждения
    std::vector<std::string> recentDetections; // Последние детекции
    size_t maxRecentDetections = 4;     // Размер буфера
    
    // Добавить методы:
    bool isSilence(const std::vector<double>& windowedData);
    std::string getConsistentVowel();
};

#endif
