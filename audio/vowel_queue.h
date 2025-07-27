#ifndef VOWEL_QUEUE_H
#define VOWEL_QUEUE_H

#include <string>
#include <chrono>
#include <vector>

// The VowelQueue class is responsible for managing a queue of vowels.
// It provides functionality to add vowels, retrieve the current vowel,
// check if there are vowels in the queue, and clear the queue.
class VowelQueue {
public:
    VowelQueue(); // Constructor to initialize the VowelQueue object.

    // Adds a list of vowels to the queue.
    // @param vowels: A vector of strings representing the vowels to be added.
    void addVowels(const std::vector<std::string>& vowels);

    // Retrieves the current vowel from the queue.
    // @return: A string representing the current vowel.
    std::string getCurrentVowel();

    // Checks if there are any vowels in the queue.
    // @return: A boolean value indicating whether the queue contains vowels.
    bool hasVowels() const;

    // Clears all vowels from the queue.
    void clear();

private:
    std::string currentVowel; // Stores the current vowel being displayed.
    std::chrono::steady_clock::time_point lastUpdateTime; // Tracks the last time the vowel was updated.
    const std::chrono::milliseconds vowelDisplayDuration{100}; // Duration for which each vowel is displayed (100ms).
    bool isEmpty; // Indicates whether the queue is empty.
};

#endif