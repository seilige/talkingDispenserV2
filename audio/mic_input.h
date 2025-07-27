#ifndef MIC_INPUT_H
#define MIC_INPUT_H

#include <portaudio.h>
#include <vector>

class MicInput {
public:
    MicInput();
    ~MicInput();

    // Initializes the microphone input. Returns true if successful, false otherwise.
    bool init();

    // Starts the audio stream for capturing microphone input.
    void start();

    // Stops the audio stream and releases resources.
    void stop();

    // Reads audio data from the microphone into the provided buffer.
    // Parameters:
    // - buffer: Pointer to the buffer where audio data will be stored.
    // - bufferSize: The size of the buffer in samples.
    // Returns the number of samples read.
    int read(short* buffer, int bufferSize);

    // Checks if the audio stream is currently running.
    // Returns true if the stream is active, false otherwise.
    bool isRunning() const;

private:
    PaStream* stream_;       // Pointer to the PortAudio stream object.
    bool initialized_;       // Indicates whether the microphone input has been initialized.
    bool running_;           // Indicates whether the audio stream is currently running.
    
    static constexpr int SAMPLE_RATE = 16000;  // Recommended sample rate for Vosk is 16kHz.
    static constexpr int FRAMES_PER_BUFFER = 512; // Number of frames per buffer for audio processing.
};

#endif  // MIC_INPUT_H