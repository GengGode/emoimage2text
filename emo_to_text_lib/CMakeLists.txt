cmake_minimum_required(VERSION 3.15)
project(emo_to_text_lib)

set(CMAKE_CXX_STANDARD 14)

add_executable(emo_to_text_lib main.cpp)

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
# cpr
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)
# spdlog
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
                            GIT_TAG v1.12.0) # The commit hash for 1.8.2. Replace with the latest from:
FetchContent_MakeAvailable(spdlog)
# opencv
FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG        4.8.0)
FetchContent_MakeAvailable(opencv)
# paddle
# FetchContent_Declare(
#         paddle
#         GIT_REPOSITORY https://github.com/PaddlePaddle/Paddle.git
#         GIT_TAG        release/2.1)
# FetchContent_MakeAvailable(paddle)
# paddle_ocr
FetchContent_Declare(
        paddle_ocr
        GIT_REPOSITORY https://github.com/PaddlePaddle/PaddleOCR.git
        GIT_TAG        v2.6.0)
FetchContent_MakeAvailable(paddle_ocr)


# cpr
target_link_libraries(emo_to_text_lib PRIVATE cpr::cpr)
# spdlog
target_link_libraries(emo_to_text_lib PRIVATE spdlog::spdlog spdlog::spdlog_header_only)


target_link_libraries(emo_to_text_lib
        ${OpenCV_LIBS}
        ${PADDLE_LIBRARIES}
        ${PADDLEOCR_LIBRARIES}
        )