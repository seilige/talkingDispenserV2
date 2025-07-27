#include "vosk_recognizer.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <queue>
#include <vector>
SpeechRecognizer::SpeechRecognizer(const std::string& modelPath)
    : model_(nullptr), recognizer_(nullptr), valid_(false) {
    
    // Установка уровня логирования Vosk (0 = минимум логов)
    vosk_set_log_level(-1);
    
    // Загрузка модели
    model_ = vosk_model_new(modelPath.c_str());
    if (!model_) {
        std::cerr << "Failed to load model Vosk from:" << modelPath << std::endl;
        std::cerr << "Make sure the model folder exists and contains the necessary files." << std::endl;
        return;
    }

    // Создание распознавателя
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
    if (recognizer_) {
        vosk_recognizer_free(recognizer_);
    }
    if (model_) {
        vosk_model_free(model_);
    }
}

bool SpeechRecognizer::isValid() const {
    return valid_;
}

void SpeechRecognizer::reset() {
    if (recognizer_) {
        vosk_recognizer_reset(recognizer_);
    }
}

std::string SpeechRecognizer::recognize(const short* audio, int audioSize) {
    if (!valid_ || !recognizer_) {
        std::cout << "Recognizer not valid!" << std::endl;
        return "";
    }

    // Преобразуем размер из количества семплов в байты
    int audioBytes = audioSize * sizeof(short);
    
    // Подаём аудиоданные в распознаватель
    int result = vosk_recognizer_accept_waveform(recognizer_, 
                                               reinterpret_cast<const char*>(audio), 
                                               audioBytes);

    std::string recognizedText;

    if (result) {
        // Финальный результат - сбрасываем распознаватель для нового текста
        const char* jsonResult = vosk_recognizer_result(recognizer_);
        recognizedText = parseJsonResult(jsonResult);
        if (!recognizedText.empty()) {
            std::cout << "Vosk final result: " << recognizedText << std::endl;
            // Сбрасываем распознаватель для начала нового распознавания
            vosk_recognizer_reset(recognizer_);
        }
    } else {
        // Частичный результат - работаем только с ним
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
    
    // Простой парсинг JSON для извлечения текста
    // Ищем поле "text": "..."
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
    startQuote++; // Пропускаем открывающую кавычку
    
    size_t endQuote = json.find("\"", startQuote);
    if (endQuote == std::string::npos) {
        return "";
    }
    
    return json.substr(startQuote, endQuote - startQuote);
}

std::vector<std::string> SpeechRecognizer::extractVowels(const std::string& text) {
    std::cout << "Extracting vowels from: '" << text << "'" << std::endl;
    
    std::vector<std::string> vowels;
    
    // Проходим по каждому символу в тексте
    for (size_t i = 0; i < text.length(); i++) {
        std::string currentChar;
        
        // Обработка UTF-8 символов (русские буквы занимают 2 байта)
        if ((unsigned char)text[i] >= 0xC0) {
            if (i + 1 < text.length()) {
                currentChar = text.substr(i, 2);
                i++; // Пропускаем второй байт UTF-8
            }
        } else {
            currentChar = text[i];
        }
        
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
    
    // Если текст пустой, ничего не возвращаем
    if (newText.empty()) {
        return {};
    }
    
    // Если новый текст короче предыдущего или полностью отличается, 
    // значит началось новое распознавание - берем все гласные
    if (newText.length() < previousText.length() || 
        previousText.empty() || 
        newText.substr(0, std::min(newText.length(), previousText.length())) != 
        previousText.substr(0, std::min(newText.length(), previousText.length()))) {
        
        std::cout << "New recognition started" << std::endl;
        return extractVowels(newText);
    }
    
    // Если новый текст длиннее и начинается с предыдущего,
    // извлекаем только новую часть
    if (newText.length() > previousText.length() && 
        newText.substr(0, previousText.length()) == previousText) {
        
        std::string newPart = newText.substr(previousText.length());
        std::cout << "New part: '" << newPart << "'" << std::endl;
        return extractVowels(newPart);
    }
    
    // В остальных случаях ничего не добавляем
    return {};
}
