#include "mic_input.h"
#include <iostream>
#include <cstring>

MicInput::MicInput() : stream_(nullptr), initialized_(false), running_(false) {
}

MicInput::~MicInput() {
    stop();
    if (stream_) {
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
    if (initialized_) {
        Pa_Terminate();
    }
}

bool MicInput::init() {
    if (initialized_) {
        return true;
    }

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    initialized_ = true;

    // Получаем информацию об устройстве ввода по умолчанию
    PaDeviceIndex defaultDevice = Pa_GetDefaultInputDevice();
    if (defaultDevice == paNoDevice) {
        std::cerr << "No default input device found" << std::endl;
        return false;
    }

    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(defaultDevice);
    std::cout << "Input device used:" << deviceInfo->name << std::endl;

    // Настройка параметров входного потока
    PaStreamParameters inputParameters;
    inputParameters.device = defaultDevice;
    inputParameters.channelCount = 1;  // Моно
    inputParameters.sampleFormat = paInt16;  // 16-bit samples
    inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // Открытие потока
    err = Pa_OpenStream(&stream_,
                       &inputParameters,
                       nullptr,  // Нет выходного потока
                       SAMPLE_RATE,
                       FRAMES_PER_BUFFER,
                       paClipOff,
                       nullptr,  // Нет callback функции
                       nullptr); // Нет данных пользователя

    if (err != paNoError) {
        std::cerr << "PortAudio open stream error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    std::cout << "Microphone initialized successfully (Sample Rate: " << SAMPLE_RATE << " Hz)" << std::endl;
    return true;
}

void MicInput::start() {
    if (!stream_ || running_) {
        return;
    }

    PaError err = Pa_StartStream(stream_);
    if (err != paNoError) {
        std::cerr << "PortAudio start stream error: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    running_ = true;
    std::cout << "Recording from microphone started" << std::endl;
}

void MicInput::stop() {
    if (!stream_ || !running_) {
        return;
    }

    PaError err = Pa_StopStream(stream_);
    if (err != paNoError) {
        std::cerr << "PortAudio stop stream error: " << Pa_GetErrorText(err) << std::endl;
    }

    running_ = false;
    std::cout << "Microphone recording stopped" << std::endl;
}

int MicInput::read(short* buffer, int bufferSize) {
    if (!stream_ || !running_) {
        return 0;
    }

    PaError err = Pa_ReadStream(stream_, buffer, bufferSize);
    
    if (err == paNoError) {
        return bufferSize;
    } else if (err == paInputOverflowed) {
        std::cerr << "Input overflow detected" << std::endl;
        return bufferSize;  // Данные всё ещё валидны
    } else {
        std::cerr << "PortAudio read error: " << Pa_GetErrorText(err) << std::endl;
        return 0;
    }
}

bool MicInput::isRunning() const {
    return running_;
}