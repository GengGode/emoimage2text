cmake_minimum_required(VERSION 3.15)
project(emo_to_text_lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(emo_to_text_lib main.cpp)

# cpr
target_link_libraries(emo_to_text_lib PRIVATE cpr::cpr)
# spdlog
target_link_libraries(emo_to_text_lib PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
