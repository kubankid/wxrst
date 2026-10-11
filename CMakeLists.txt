cmake_minimum_required(VERSION 3.22)
project(ICEWATER VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")

# Windows: static MSVC runtime for ALL targets (incl. JUCE plugin-client objects) so the
# VST3/EXE run without the VC++ redistributable. Must be set before add_subdirectory(JUCE).
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# JUCE: use the bundled ./JUCE checkout (git submodule) if present, otherwise fetch it.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.10
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# AU only exists on macOS. On Windows we build VST3 + Standalone.
if(APPLE)
    set(DENNY_FORMATS VST3 AU Standalone)
    set(DENNY_COPY_AFTER_BUILD TRUE)   # install to ~/Library/Audio/Plug-Ins
else()
    set(DENNY_FORMATS VST3 Standalone)
    set(DENNY_COPY_AFTER_BUILD FALSE)  # C:\Program Files needs admin; copy manually / via CI zip
endif()

juce_add_plugin(ICEWATER
    COMPANY_NAME "DennyAudio"
    PLUGIN_MANUFACTURER_CODE Dnny
    PLUGIN_CODE Wxrs
    FORMATS ${DENNY_FORMATS}
    PRODUCT_NAME "WXRST"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${DENNY_COPY_AFTER_BUILD}
    VST3_CATEGORIES Instrument Synth
    AU_MAIN_TYPE kAudioUnitType_MusicDevice)

if(MSVC)
    target_compile_options(ICEWATER PUBLIC /utf-8 /bigobj)
endif()

target_sources(ICEWATER PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_include_directories(ICEWATER PRIVATE Source)

target_compile_definitions(ICEWATER PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(ICEWATER PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

# ---- offline chord-engine test (not shipped) ----
option(ICEWATER_BUILD_TEST "Build the console chord-engine test" OFF)
if(ICEWATER_BUILD_TEST)
    juce_add_console_app(ICEWATER_TEST PRODUCT_NAME "ChordTest")
    target_sources(ICEWATER_TEST PRIVATE Source/ChordTest.cpp)
    target_include_directories(ICEWATER_TEST PRIVATE Source)
    target_compile_definitions(ICEWATER_TEST PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(ICEWATER_TEST PRIVATE juce::juce_audio_basics juce::juce_core PUBLIC juce::juce_recommended_config_flags)
endif()

if(ICEWATER_BUILD_TEST)
    juce_add_console_app(SAMPLE_TEST PRODUCT_NAME "SampleTest")
    target_sources(SAMPLE_TEST PRIVATE Source/SampleTest.cpp)
    target_include_directories(SAMPLE_TEST PRIVATE Source)
    target_compile_definitions(SAMPLE_TEST PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(SAMPLE_TEST PRIVATE juce::juce_audio_formats juce::juce_dsp juce::juce_core PUBLIC juce::juce_recommended_config_flags)
endif()
