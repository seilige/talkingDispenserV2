#include "vosk_recognizer.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <queue>
#include <vector>
SpeechRecognizer::SpeechRecognizer(const std::string& modelPath)
    : model_(nullptr), recognizer_(nullptr), valid_(false) {
    
    // Set the logging level for Vosk (0 = minimal logs)
    vosk_set_log_level(-1);
    
    // Load the model from the specified path
    model_ = vosk_model_new(modelPath.c_str());
    if (!model_) {
        std::cerr << "Failed to load model Vosk from:" << modelPath << std::endl;
        std::cerr << "Make sure the model folder exists and contains the necessary files." << std::endl;
        return;
    }

    // Create the recognizer instance
    recognizer_ = vosk_recognizer_new(model_, SAMPLE_RATE);
    if (!recognizer_) {
        std::cerr << "Failed to create Vosk recognizer" << std::endl;
        vosk_model_free(model_);
        model_ = nullptr;
        return;
    }

    valid_ = true;
    std::cout << "Vosk Recognizer initialized successfully" << std::endl;
}

SpeechRecognizer::~SpeechRecognizer() {
    // Free the recognizer instance if it exists
    if (recognizer_) {
        vosk_recognizer_free(recognizer_);
    }
    // Free the model instance if it exists
    if (model_) {
        vosk_model_free(model_);
    }
}

bool SpeechRecognizer::isValid() const {
    // Check if the recognizer is valid and ready for use
    return valid_;
}

void SpeechRecognizer::reset() {
    // Reset the recognizer to prepare it for new audio input
    if (recognizer_) {
        vosk_recognizer_reset(recognizer_);
    }
}

std::string SpeechRecognizer::recognize(const short* audio, int audioSize) {
    if (!valid_ || !recognizer_) {
        std::cout << "Recognizer not valid!" << std::endl;
        return "";
    }

    // Convert the size from the number of samples to the number of bytes
    int audioBytes = audioSize * sizeof(short);
    
    // Feed the audio data into the recognizer
    int result = vosk_recognizer_accept_waveform(recognizer_, 
                                               reinterpret_cast<const char*>(audio), 
                                               audioBytes);

    std::string recognizedText;

    if (result) {
        // Final result - reset the recognizer for new recognition
        const char* jsonResult = vosk_recognizer_result(recognizer_);
        recognizedText = parseJsonResult(jsonResult);
        if (!recognizedText.empty()) {
            std::cout << "Vosk final result: " << recognizedText << std::endl;
            // Reset the recognizer to start new recognition
            vosk_recognizer_reset(recognizer_);
        }
    } else {
        // Partial result - process it only
        const char* jsonPartial = vosk_recognizer_partial_result(recognizer_);
        recognizedText = parseJsonResult(jsonPartial);
        if (!recognizedText.empty()) {
            std::cout << "Vosk partial result: " << recognizedText << std::endl;
        }
    }

    return recognizedText;
}

std::string SpeechRecognizer::parseJsonResult(const char* jsonResult) {
    if (!jsonResult) {
        return "";
    }
    
    std::string json(jsonResult);
    
    // Simple JSON parsing to extract the "text" field
    // Look for the "text": "..." field
    size_t textPos = json.find("\"text\"");
    if (textPos == std::string::npos) {
        return "";
    }
    
    size_t colonPos = json.find(":", textPos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    size_t startQuote = json.find("\"", colonPos);
    if (startQuote == std::string::npos) {
        return "";
    }
    startQuote++; // Skip the opening quote
    
    size_t endQuote = json.find("\"", startQuote);
    if (endQuote == std::string::npos) {
        return "";
    }
    
    return json.substr(startQuote, endQuote - startQuote);
}

std::vector<std::string> SpeechRecognizer::extractVowels(const std::string& text) {
    std::cout << "Extracting vowels from: '" << text << "'" << std::endl;
    
    std::vector<std::string> vowels;
    
    // Iterate through each character in the text
    for (size_t i = 0; i < text.length(); i++) {
        std::string currentChar;
        
        // Handle UTF-8 characters (Russian letters occupy 2 bytes)
        if ((unsigned char)text[i] >= 0xC0) {
            if (i + 1 < text.length()) {
                currentChar = text.substr(i, 2);
                i++; // Skip the second byte of UTF-8
            }
        } else {
            currentChar = text[i];
        }
        
        // Check for vowels in both Russian and English alphabets
        if (currentChar == "а" || currentChar == "А") {
            vowels.push_back("а");
        } else if (currentChar == "я" || currentChar == "Я") {
            vowels.push_back("я");
        } else if (currentChar == "э" || currentChar == "Э") {
            vowels.push_back("э");
        } else if (currentChar == "е" || currentChar == "Е") {
            vowels.push_back("е");
        } else if (currentChar == "и" || currentChar == "И") {
            vowels.push_back("и");
        } else if (currentChar == "ы" || currentChar == "Ы") {
            vowels.push_back("ы");
        } else if (currentChar == "о" || currentChar == "О") {
            vowels.push_back("о");
        } else if (currentChar == "ё" || currentChar == "Ё") {
            vowels.push_back("ё");
        } else if (currentChar == "у" || currentChar == "У") {
            vowels.push_back("у");
        } else if (currentChar == "ю" || currentChar == "Ю") {
            vowels.push_back("ю");
        } else if (currentChar == "a" || currentChar == "A") {
            vowels.push_back("а");
        } else if (currentChar == "e" || currentChar == "E") {
            vowels.push_back("э");
        } else if (currentChar == "i" || currentChar == "I") {
            vowels.push_back("и");
        } else if (currentChar == "o" || currentChar == "O") {
            vowels.push_back("о");
        } else if (currentChar == "u" || currentChar == "U") {
            vowels.push_back("у");
        } else if (currentChar == "y" || currentChar == "Y") {
            vowels.push_back("ы");
        }
    }
    
    if (!vowels.empty()) {
        std::cout << "Found vowels: ";
        for (const auto& vowel : vowels) {
            std::cout << vowel << " ";
        }
        std::cout << std::endl;
    }
    
    return vowels;
}

std::vector<std::string> SpeechRecognizer::extractNewVowels(const std::string& newText, const std::string& previousText) {
    std::cout << "Current text: '" << newText << "'" << std::endl;
    
    // If the text is empty, return nothing
    if (newText.empty()) {
        return {};
    }
    
    // If the new text is shorter than the previous one or completely different,
    // it means a new recognition session has started - extract all vowels
    if (newText.length() < previousText.length() || 
        previousText.empty() || 
        newText.substr(0, std::min(newText.length(), previousText.length())) != 
        previousText.substr(0, std::min(newText.length(), previousText.length()))) {
        
        std::cout << "New recognition started" << std::endl;
        return extractVowels(newText);
    }
    
    // If the new text is longer and starts with the previous one,
    // extract vowels only from the new part
    if (newText.length() > previousText.length() && 
        newText.substr(0, previousText.length()) == previousText) {
        
        std::string newPart = newText.substr(previousText.length());
        std::cout << "New part: '" << newPart << "'" << std::endl;
        return extractVowels(newPart);
    }
    
    // In all other cases, do not add anything
    return {};
}