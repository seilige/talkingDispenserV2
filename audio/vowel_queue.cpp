#include "vowel_queue.h"
#include <iostream>

VowelQueue::VowelQueue() : isEmpty(true) {}

void VowelQueue::addVowels(const std::vector<std::string>& vowels) {
    if (!vowels.empty() && !vowels[0].empty()) {
        // Добавляем простое сглаживание - меняем гласный только если он отличается
        if (currentVowel != vowels[0] || isEmpty) {
            currentVowel = vowels[0];
            lastUpdateTime = std::chrono::steady_clock::now();
            isEmpty = false;
            std::cout << "Vowel changed to: " << currentVowel << std::endl;
        } else {
            // Обновляем время, чтобы гласный не исчез
            lastUpdateTime = std::chrono::steady_clock::now();
        }
    }
}

std::string VowelQueue::getCurrentVowel() {
    auto now = std::chrono::steady_clock::now();
    
    // Если прошло слишком много времени, очищаем текущий гласный
    if (!isEmpty && now - lastUpdateTime > vowelDisplayDuration) {
        clear();
    }
    
    return isEmpty ? "" : currentVowel;
}

bool VowelQueue::hasVowels() const {
    return !isEmpty;
}

void VowelQueue::clear() {
    currentVowel.clear();
    isEmpty = true;
}
