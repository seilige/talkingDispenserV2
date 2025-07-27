#include "mic_input.h"
#include <iostream>
#include <cstring>

// Constructor for the MicInput class. Initializes member variables to default values.
MicInput::MicInput() : stream_(nullptr), initialized_(false), running_(false) {
}

// Destructor for the MicInput class. Ensures that the microphone stream is stopped and resources are released.
MicInput::~MicInput() {
    stop(); // Stop the microphone stream if it is running.
    if (stream_) {
        Pa_CloseStream(stream_); // Close the PortAudio stream if it is open.
        stream_ = nullptr;
    }
    if (initialized_) {
        Pa_Terminate(); // Terminate the PortAudio library if it was initialized.
    }
}

// Initializes the microphone input by setting up the PortAudio library and configuring the input stream.
bool MicInput::init() {
    if (initialized_) {
        return true; // If already initialized, return true.
    }

    // Initialize the PortAudio library.
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init error: " << Pa_GetErrorText(err) << std::endl;
        return false; // Return false if initialization fails.
    }
    initialized_ = true;

    // Retrieve information about the default input device.
    PaDeviceIndex defaultDevice = Pa_GetDefaultInputDevice();
    if (defaultDevice == paNoDevice) {
        std::cerr << "No default input device found" << std::endl;
        return false; // Return false if no input device is available.
    }

    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(defaultDevice);
    std::cout << "Input device used: " << deviceInfo->name << std::endl;

    // Configure the parameters for the input stream.
    PaStreamParameters inputParameters;
    inputParameters.device = defaultDevice; // Use the default input device.
    inputParameters.channelCount = 1;  // Set the number of channels to mono (1 channel).
    inputParameters.sampleFormat = paInt16;  // Use 16-bit integer samples.
    inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency; // Use the default low latency for the device.
    inputParameters.hostApiSpecificStreamInfo = nullptr; // No additional host API-specific information.

    // Open the input stream with the specified parameters.
    err = Pa_OpenStream(&stream_,
                       &inputParameters,
                       nullptr,  // No output stream is needed.
                       SAMPLE_RATE, // Set the sample rate for the stream.
                       FRAMES_PER_BUFFER, // Set the number of frames per buffer.
                       paClipOff, // Disable clipping.
                       nullptr,  // No callback function is used.
                       nullptr); // No user data is passed to the callback.

    if (err != paNoError) {
        std::cerr << "PortAudio open stream error: " << Pa_GetErrorText(err) << std::endl;
        return false; // Return false if the stream could not be opened.
    }

    std::cout << "Microphone initialized successfully (Sample Rate: " << SAMPLE_RATE << " Hz)" << std::endl;
    return true; // Return true if initialization is successful.
}

// Starts the microphone input stream for recording.
void MicInput::start() {
    if (!stream_ || running_) {
        return; // Do nothing if the stream is not initialized or already running.
    }

    // Start the PortAudio stream.
    PaError err = Pa_StartStream(stream_);
    if (err != paNoError) {
        std::cerr << "PortAudio start stream error: " << Pa_GetErrorText(err) << std::endl;
        return; // Return if the stream could not be started.
    }

    running_ = true; // Set the running flag to true.
    std::cout << "Recording from microphone started" << std::endl;
}

// Stops the microphone input stream if it is running.
void MicInput::stop() {
    if (!stream_ || !running_) {
        return; // Do nothing if the stream is not initialized or not running.
    }

    // Stop the PortAudio stream.
    PaError err = Pa_StopStream(stream_);
    if (err != paNoError) {
        std::cerr << "PortAudio stop stream error: " << Pa_GetErrorText(err) << std::endl;
    }

    running_ = false; // Set the running flag to false.
    std::cout << "Microphone recording stopped" << std::endl;
}

// Reads audio data from the microphone into the provided buffer.
int MicInput::read(short* buffer, int bufferSize) {
    if (!stream_ || !running_) {
        return 0; // Return 0 if the stream is not initialized or not running.
    }

    // Read audio data from the PortAudio stream into the buffer.
    PaError err = Pa_ReadStream(stream_, buffer, bufferSize);
    
    if (err == paNoError) {
        return bufferSize; // Return the size of the buffer if the read operation is successful.
    } else if (err == paInputOverflowed) {
        std::cerr << "Input overflow detected" << std::endl;
        return bufferSize;  // Return the buffer size even if an overflow occurred (data is still valid).
    } else {
        std::cerr << "PortAudio read error: " << Pa_GetErrorText(err) << std::endl;
        return 0; // Return 0 if a read error occurred.
    }
}

// Checks if the microphone input stream is currently running.
bool MicInput::isRunning() const {
    return running_; // Return the value of the running flag.
}