#ifndef MIC_INPUT_H
#define MIC_INPUT_H

#include <portaudio.h>
#include <vector>

class MicInput {
public:
    MicInput();
    ~MicInput();

    bool init();
    void start();
    void stop();
    int read(short* buffer, int bufferSize);
    bool isRunning() const;

private:
    PaStream* stream_;
    bool initialized_;
    bool running_;
    
    static constexpr int SAMPLE_RATE = 16000;  // Vosk рекомендует 16kHz
    static constexpr int FRAMES_PER_BUFFER = 512;
};

#endif  // MIC_INPUT_H
