cmake_minimum_required(VERSION 3.15)
project(emo_to_text)

# camke proxy
set(ENV{http_proxy} "http://127.0.0.1:1080")
set(ENV{https_proxy} "http://127.0.0.1:1080")

set(Qt_Debug_Path   "../3rdparty/qt-5.15.2-lite-debug-shared")
set(Qt_Release_Path "../3rdparty/qt-5.15.2-lite-release-static")

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
# cpr
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 1.10.4) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)
# nlohmann_json
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git
                                GIT_TAG v3.9.1) # The commit hash for 3.9.1. Replace with the latest from:
FetchContent_MakeAvailable(nlohmann_json)
# spdlog
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
                            GIT_TAG v1.12.0) # The commit hash for 1.8.2. Replace with the latest from:
FetchContent_MakeAvailable(spdlog)

add_subdirectory(ui)
add_subdirectory(emo_to_text_lib)
