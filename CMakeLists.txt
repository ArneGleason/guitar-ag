cmake_minimum_required(VERSION 3.22)

project(GuitarAG VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JUCE_PATH "" CACHE PATH "Optional path to a local JUCE checkout")

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
        src/dsp/StringVoice.cpp
        src/dsp/StringVoice.h
        src/plugin/PluginEditor.cpp
        src/plugin/PluginEditor.h
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginProcessor.h)

target_compile_definitions(GuitarAG
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(GuitarAG
    PRIVATE
        juce::juce_audio_utils)
