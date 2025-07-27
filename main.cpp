#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#endif

#define SDL_MAIN_HANDLED
#include <vector>
#include <queue>
#include <chrono>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include "audio/mic_input.h"
#include "audio/vowel_detector.h"
#include "recognizer/vosk_recognizer.h"
#include "audio/vowel_queue.h" // Добавить этот include

int main() {
    SDL_SetMainReady();

    #ifdef _WIN32
        SetConsoleOutputCP(65001); // UTF-8
        SetConsoleCP(65001);       // UTF-8
    #endif

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Talking Dispenser", 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        640, 480, 
                                        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture1 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/1.png"); // а, я
    SDL_Texture* texture2 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/2.png"); // э, е
    SDL_Texture* texture3 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/3.png"); // и
    SDL_Texture* texture4 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/4.png"); // ы
    SDL_Texture* texture5 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/5.png"); // о, ё
    SDL_Texture* texture6 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/6.png"); // у, ю
    SDL_Texture* texture7 = IMG_LoadTexture(renderer, "C:/Users/Acer/Desktop/im/7.png"); // тишина

    if (!texture1 || !texture2 || !texture3 || !texture4 || !texture5 || !texture6 || !texture7) {
        std::cerr << "Failed to load textures: " << IMG_GetError() << std::endl;
        
        // Проверим каждую текстуру отдельно
        if (!texture1) std::cerr << "Failed to load 1.jpg" << std::endl;
        if (!texture2) std::cerr << "Failed to load 2.jpg" << std::endl;
        if (!texture3) std::cerr << "Failed to load 3.jpg" << std::endl;
        if (!texture4) std::cerr << "Failed to load 4.jpg" << std::endl;
        if (!texture5) std::cerr << "Failed to load 5.jpg" << std::endl;
        if (!texture6) std::cerr << "Failed to load 6.jpg" << std::endl;
        if (!texture7) std::cerr << "Failed to load 7.jpg" << std::endl;

        // Clean up loaded textures
        if (texture1) SDL_DestroyTexture(texture1);
        if (texture2) SDL_DestroyTexture(texture2);
        if (texture3) SDL_DestroyTexture(texture3);
        if (texture4) SDL_DestroyTexture(texture4);
        if (texture5) SDL_DestroyTexture(texture5);
        if (texture6) SDL_DestroyTexture(texture6);
        if (texture7) SDL_DestroyTexture(texture7);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    } else {
        // Проверим размеры текстур для отладки
        int w, h;
        SDL_QueryTexture(texture1, nullptr, nullptr, &w, &h);
        std::cout << "Texture '1' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture2, nullptr, nullptr, &w, &h);
        std::cout << "Texture '2' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture3, nullptr, nullptr, &w, &h);
        std::cout << "Texture '3' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture4, nullptr, nullptr, &w, &h);
        std::cout << "Texture '4' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture5, nullptr, nullptr, &w, &h);
        std::cout << "Texture '5' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture6, nullptr, nullptr, &w, &h);
        std::cout << "Texture '6' loaded: " << w << "x" << h << std::endl;
        SDL_QueryTexture(texture7, nullptr, nullptr, &w, &h);
        std::cout << "Texture '7' loaded: " << w << "x" << h << std::endl;
    }

    // Initialize audio input
    MicInput micInput;

    // Убираем старый код с SpeechRecognizer и заменяем на:
    VowelDetector vowelDetector;

    if (!micInput.init()) {
        std::cerr << "Failed to initialize audio input" << std::endl;
        SDL_DestroyTexture(texture1);
        SDL_DestroyTexture(texture2);
        SDL_DestroyTexture(texture3);
        SDL_DestroyTexture(texture4);
        SDL_DestroyTexture(texture5);
        SDL_DestroyTexture(texture6);
        SDL_DestroyTexture(texture7);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize speech recognizer
    SpeechRecognizer recognizer("C:/Users/Acer/Desktop/main/model/vosk-model-small-ru-0.22");
    VowelQueue vowelQueue;
    
    if (!recognizer.isValid()) {
        std::cerr << "Failed to initialize speech recognizer" << std::endl;
        SDL_DestroyTexture(texture1);
        SDL_DestroyTexture(texture2);
        SDL_DestroyTexture(texture3);
        SDL_DestroyTexture(texture4);
        SDL_DestroyTexture(texture5);
        SDL_DestroyTexture(texture6);
        SDL_DestroyTexture(texture7);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Start audio recording
    micInput.start();

    // Main loop
    bool running = true;
    SDL_Texture* currentTexture = texture7; // Устанавливаем текстуру 'a' по умолчанию
    auto lastRecognitionTime = std::chrono::steady_clock::now();
    const auto DISPLAY_DURATION = std::chrono::milliseconds(2000); // Показывать текстуру 2 секунды
    
    std::cout << "Talking Dispenser started! Pronounce vowels 'a', 'o', 'i'..." << std::endl;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        std::vector<short> audioBuffer(2048);
        int samplesRead = micInput.read(audioBuffer.data(), static_cast<int>(audioBuffer.size()));

        if (samplesRead > 0) {
            // Приоритет: прямое распознавание звуков
            std::string detectedVowel = vowelDetector.detectVowel(audioBuffer, 16000);
            
            if (!detectedVowel.empty()) {
                std::cout << "Direct detection: " << detectedVowel << std::endl;
                std::vector<std::string> vowels = {detectedVowel};
                vowelQueue.addVowels(vowels);
                lastRecognitionTime = std::chrono::steady_clock::now();
            }
            
            // Дополнительно: распознавание через Vosk (только если прямое не сработало)
            static std::string lastRecognizedText = "";
            std::string recognizedText = recognizer.recognize(audioBuffer.data(), samplesRead);
            
            if (!recognizedText.empty() && recognizedText != lastRecognizedText) {
                std::vector<std::string> newVowels = recognizer.extractNewVowels(recognizedText, lastRecognizedText);
                if (!newVowels.empty() && detectedVowel.empty()) {
                    std::cout << "Vosk backup: ";
                    for (const auto& v : newVowels) {
                        std::cout << v << " ";
                    }
                    std::cout << std::endl;
                    vowelQueue.addVowels(newVowels);
                    lastRecognitionTime = std::chrono::steady_clock::now();
                }
                lastRecognizedText = recognizedText;
            }
        }

        // Обновляем текстуру
        std::string currentVowel = vowelQueue.getCurrentVowel();
        if (!currentVowel.empty()) {
            SDL_Texture* newTexture = nullptr;
            
            // Группируем гласные по типам губ
            if (currentVowel == "а" || currentVowel == "я") {
                newTexture = texture1;
            } else if (currentVowel == "э" || currentVowel == "е") {
                newTexture = texture2;
            } else if (currentVowel == "и") {
                newTexture = texture3;
            } else if (currentVowel == "ы") {
                newTexture = texture4;
            } else if (currentVowel == "о" || currentVowel == "ё") {
                newTexture = texture5;
            } else if (currentVowel == "у" || currentVowel == "ю") {
                newTexture = texture6;
            }
            
            if (newTexture && currentTexture != newTexture) {
                currentTexture = newTexture;
                std::cout << "Switched to vowel group for '" << currentVowel << "'" << std::endl;
            }
        } else {
            // Возвращаемся к тишине
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastRecognitionTime > std::chrono::milliseconds(150)) {
                if (currentTexture != texture7) {
                    currentTexture = texture7;
                    std::cout << "Back to silence" << std::endl;
                }
            }
        }


        // Display current texture
        if (currentTexture) {
            SDL_RenderCopy(renderer, currentTexture, nullptr, nullptr);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Small delay to prevent high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // Stop audio recording
    micInput.stop();

    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texture4);
    SDL_DestroyTexture(texture5);
    SDL_DestroyTexture(texture6);
    SDL_DestroyTexture(texture7);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    std::cout << "Program terminated." << std::endl;
    return 0;
}