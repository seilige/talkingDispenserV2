#ifndef VOSK_RECOGNIZER_H
#define VOSK_RECOGNIZER_H

#include <vosk_api.h>
#include <string>
#include <queue>
#include <chrono>
#include <vector>

// The SpeechRecognizer class provides an interface for speech recognition
// using the Vosk API. It allows initializing a speech recognition model,
// processing audio data to recognize speech, and extracting specific
// information such as vowels from the recognized text.
class SpeechRecognizer {
public:
    // Constructor: Initializes the SpeechRecognizer with the specified model path.
    // The model path should point to a valid Vosk speech recognition model.
    SpeechRecognizer(const std::string& modelPath);

    // Destructor: Cleans up resources used by the SpeechRecognizer, including
    // the Vosk model and recognizer instances.
    ~SpeechRecognizer();

    // Processes the given audio data and returns the recognized text as a string.
    // Parameters:
    // - audio: Pointer to the audio data (16-bit PCM samples).
    // - audioSize: Number of samples in the audio data.
    // Returns:
    // - A string containing the recognized text.
    std::string recognize(const short* audio, int audioSize);

    // Checks if the SpeechRecognizer is in a valid state.
    // Returns:
    // - true if the recognizer is valid and ready to use, false otherwise.
    bool isValid() const;

    // Resets the internal state of the recognizer, clearing any ongoing recognition
    // process and preparing it for new input.
    void reset();

    // Extracts all vowels from the given text.
    // Parameters:
    // - text: The input text from which vowels will be extracted.
    // Returns:
    // - A vector of strings, each containing a vowel found in the text.
    std::vector<std::string> extractVowels(const std::string& text);

    // Extracts vowels that are present in the new text but not in the previous text.
    // Parameters:
    // - newText: The new text to analyze.
    // - previousText: The previous text to compare against.
    // Returns:
    // - A vector of strings containing the new vowels found in the new text.
    std::vector<std::string> extractNewVowels(const std::string& newText, const std::string& previousText);

private:
    // Pointer to the Vosk model instance used for speech recognition.
    VoskModel* model_;

    // Pointer to the Vosk recognizer instance used for processing audio data.
    VoskRecognizer* recognizer_;

    // Indicates whether the recognizer is in a valid state.
    bool valid_;

    // Parses the JSON result returned by the Vosk recognizer and extracts
    // the recognized text.
    // Parameters:
    // - jsonResult: The JSON string returned by the recognizer.
    // Returns:
    // - A string containing the recognized text extracted from the JSON.
    std::string parseJsonResult(const char* jsonResult);

    // The sample rate used for audio processing (16 kHz).
    static constexpr int SAMPLE_RATE = 16000;
};

#endif  // VOSK_RECOGNIZER_H