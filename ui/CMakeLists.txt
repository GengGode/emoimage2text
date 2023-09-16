project(emo_to_text.ui)

set(CMAKE_CXX_STANDARD 17)

set(Qt5_DIR ${Qt_Release_Path}/lib/cmake/Qt5)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(Qt5_DIR ${Qt_Debug_Path}/lib/cmake/Qt5)
endif ()
message(STATUS "\nQt5_DIR: ${Qt5_DIR}\n")


find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(src_files
        main.cpp
        form/EmoToText.cpp
        form/EmoToText.h
        form/EmoToText.ui
        form/EmoToText.qrc
        form/EmoRetrievalWindow.cpp
        form/EmoRetrievalWindow.h
        form/EmoRetrievalWindow.ui
        )

add_executable(${PROJECT_NAME} ${src_files})

set_target_properties(${PROJECT_NAME}  PROPERTIES COMPILE_FLAGS "/MT")
#set_target_properties(${PROJECT_NAME}  PROPERTIES COMPILE_FLAGS "/MTd")

# cpr
target_link_libraries(${PROJECT_NAME} cpr::cpr)
# nlohmann_json
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
# spdlog
target_link_libraries(${PROJECT_NAME} spdlog::spdlog spdlog::spdlog_header_only)

target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Widgets)
