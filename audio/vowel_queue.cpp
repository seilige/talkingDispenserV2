#include "vowel_queue.h"
#include <iostream>

VowelQueue::VowelQueue() : isEmpty(true) {}

// Adds vowels to the queue. If the first vowel in the input vector is different 
// from the current vowel or if the queue is empty, the current vowel is updated.
// Additionally, the timestamp of the last update is refreshed. If the vowel is 
// the same as the current one, only the timestamp is updated to prevent the vowel 
// from being cleared due to timeout.
void VowelQueue::addVowels(const std::vector<std::string>& vowels) {
    if (!vowels.empty() && !vowels[0].empty()) {
        // Perform a simple smoothing mechanism - change the vowel only if it differs
        if (currentVowel != vowels[0] || isEmpty) {
            currentVowel = vowels[0];
            lastUpdateTime = std::chrono::steady_clock::now();
            isEmpty = false;
            std::cout << "Vowel changed to: " << currentVowel << std::endl;
        } else {
            // Update the timestamp to prevent the vowel from being cleared
            lastUpdateTime = std::chrono::steady_clock::now();
        }
    }
}

// Returns the current vowel. If too much time has passed since the last update, 
// the current vowel is cleared and an empty string is returned.
std::string VowelQueue::getCurrentVowel() {
    auto now = std::chrono::steady_clock::now();
    
    // If the timeout duration has passed, clear the current vowel
    if (!isEmpty && now - lastUpdateTime > vowelDisplayDuration) {
        clear();
    }
    
    return isEmpty ? "" : currentVowel;
}

// Checks if there are any vowels in the queue. Returns true if the queue is not empty.
bool VowelQueue::hasVowels() const {
    return !isEmpty;
}

// Clears the current vowel and marks the queue as empty.
void VowelQueue::clear() {
    currentVowel.clear();
    isEmpty = true;
}