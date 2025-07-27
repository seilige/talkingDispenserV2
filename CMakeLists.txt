cmake_minimum_required(VERSION 3.10)
project(TalkingDispenser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:/c++/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Добавь SDL2_DIR
set(SDL2_DIR "C:/c++/vcpkg/installed/x64-windows/share/sdl2")

# Подключаем Vosk API
add_library(vosk SHARED IMPORTED)
set_target_properties(vosk PROPERTIES
    IMPORTED_LOCATION "C:/Users/Acer/Desktop/vosk-api-win/libvosk.dll"
    IMPORTED_IMPLIB "C:/Users/Acer/Desktop/vosk-api-win/libvosk.lib"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Acer/Desktop/vosk-api-win"
)

# Поиск SDL2 и SDL2_image
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED PATHS "C:/c++/vcpkg/installed/x64-windows/share/sdl2-image")

# Поиск PortAudio
find_package(portaudio CONFIG REQUIRED)

# Главный исполняемый файл
add_executable(${PROJECT_NAME}
    main.cpp
    audio/mic_input.cpp
    recognizer/vosk_recognizer.cpp
    audio/vowel_detector.cpp
    audio/vowel_queue.cpp
)

# Копируем папку модели в директорию сборки
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/model/vosk-model-small-ru-0.22"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/model/vosk-model-small-ru-0.22"
)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
    portaudio
    vosk
)

# Копируем необходимые DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Users/Acer/Desktop/vosk-api-win/libvosk.dll"
        "C:/Users/Acer/Desktop/vosk-api-win/libgcc_s_seh-1.dll"
        "C:/Users/Acer/Desktop/vosk-api-win/libstdc++-6.dll"
        "C:/Users/Acer/Desktop/vosk-api-win/libwinpthread-1.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
