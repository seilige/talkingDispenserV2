#ifndef VOWEL_QUEUE_H
#define VOWEL_QUEUE_H

#include <string>
#include <chrono>
#include <vector>

class VowelQueue {
public:
    VowelQueue();
    void addVowels(const std::vector<std::string>& vowels);
    std::string getCurrentVowel();
    bool hasVowels() const;
    void clear();

private:
    std::string currentVowel;
    std::chrono::steady_clock::time_point lastUpdateTime;
    const std::chrono::milliseconds vowelDisplayDuration{100}; // 200мс на гласный
    bool isEmpty;
};

#endif
