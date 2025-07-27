# Talking Dispenser (Minecraft Style)

**Talking Dispenser** is a C++ desktop application that mimics a talking Minecraft dispenser by recognizing spoken vowels in real time and displaying corresponding mouth positions.

## 🧠 How It Works

This project uses [Vosk API](https://alphacephei.com/vosk/) with the Russian speech model `vosk-model-small-ru-0.22` to recognize vowels (`а`, `о`, `и`, etc.) from the user's voice input. Depending on the vowel detected, the displayed image of the dispenser changes to simulate the appropriate mouth shape.

## 🔧 Technologies Used

- **C++**
- **SDL2** and **SDL2_image** for rendering images
- **PortAudio** for audio input
- **Vosk API** for speech recognition
- **vosk-model-small-ru-0.22** (lightweight Russian voice model)

## 📸 Features

- Real-time voice input via microphone
- Live vowel recognition in Russian
- Dynamic image swapping to reflect speaker's mouth shape
- Minecraft-themed UI using a dispenser character
- 