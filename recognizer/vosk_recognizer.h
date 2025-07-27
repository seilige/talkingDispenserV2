#ifndef VOSK_RECOGNIZER_H
#define VOSK_RECOGNIZER_H

#include <vosk_api.h>
#include <string>
#include <queue>
#include <chrono>
#include <vector>

class SpeechRecognizer {
public:
    SpeechRecognizer(const std::string& modelPath);
    ~SpeechRecognizer();

    std::string recognize(const short* audio, int audioSize);
    bool isValid() const;
    void reset();

    std::vector<std::string> extractVowels(const std::string& text);
    std::vector<std::string> extractNewVowels(const std::string& newText, const std::string& previousText);
private:
    VoskModel* model_;
    VoskRecognizer* recognizer_;
    bool valid_;
    
    std::string parseJsonResult(const char* jsonResult);
    
    static constexpr int SAMPLE_RATE = 16000;
};

#endif  // VOSK_RECOGNIZER_H