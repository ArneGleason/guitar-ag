cmake_minimum_required(VERSION 3.22)

project(GuitarAG VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JUCE_PATH "" CACHE PATH "Optional path to a local JUCE checkout")
set(GUITAR_AG_MODEL_LABEL "StringVoice KS-024 TailSustain")

find_package(Git QUIET)

if (Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GUITAR_AG_GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GUITAR_AG_GIT_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
endif()

if (NOT GUITAR_AG_GIT_COMMIT)
    set(GUITAR_AG_GIT_COMMIT "unknown")
endif()

if (GUITAR_AG_GIT_STATUS)
    string(APPEND GUITAR_AG_GIT_COMMIT "-dirty")
endif()

configure_file(
    cmake/BuildInfo.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/generated/BuildInfo.h"
    @ONLY)

if (JUCE_PATH)
    add_subdirectory("${JUCE_PATH}" "${CMAKE_BINARY_DIR}/JUCE")
else()
    find_package(JUCE CONFIG QUIET)

    if (NOT JUCE_FOUND)
        message(FATAL_ERROR
            "JUCE was not found. Configure with -DJUCE_PATH=/path/to/JUCE "
            "or set CMAKE_PREFIX_PATH/JUCE_DIR to an installed JUCE CMake package.")
    endif()
endif()

juce_add_plugin(GuitarAG
    COMPANY_NAME "Arne Gleason"
    BUNDLE_ID "com.arnegleason.guitarag"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE ArGl
    PLUGIN_CODE GtAg
    FORMATS VST3
    PRODUCT_NAME "Guitar AG")

target_sources(GuitarAG
    PRIVATE
        src/dsp/AudioEngine.cpp
        src/dsp/AudioEngine.h
        src/dsp/ElectricGuitarTone.cpp
        src/dsp/ElectricGuitarTone.h
        src/dsp/FretboardMapper.cpp
        src/dsp/FretboardMapper.h
        src/dsp/StringVoice.cpp
        src/dsp/StringVoice.h
        src/plugin/PluginEditor.cpp
        src/plugin/PluginEditor.h
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginProcessor.h)

target_include_directories(GuitarAG
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated")

target_compile_definitions(GuitarAG
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(GuitarAG
    PRIVATE
        juce::juce_audio_utils)

juce_add_console_app(GuitarAGOfflineRender
    PRODUCT_NAME "Guitar AG Offline Render")

target_sources(GuitarAGOfflineRender
    PRIVATE
        tools/render-calibration.cpp
        src/dsp/AudioEngine.cpp
        src/dsp/AudioEngine.h
        src/dsp/ElectricGuitarTone.cpp
        src/dsp/ElectricGuitarTone.h
        src/dsp/FretboardMapper.cpp
        src/dsp/FretboardMapper.h
        src/dsp/StringVoice.cpp
        src/dsp/StringVoice.h)

target_compile_definitions(GuitarAGOfflineRender
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(GuitarAGOfflineRender
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats)
