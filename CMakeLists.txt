cmake_minimum_required(VERSION 3.14)
project(SmartHomeCore CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

#if(NOT EXISTS "${CMAKE_SOURCE_DIR}/Dependencies/DHT22/include/DHT22.h")
#    message(FATAL_ERROR
#        "DHT22 submodule not initialized.\n"
#        "Run: git submodule update --init --recursive")
#endif()

add_library(SmartHomeCore STATIC
    SmartHome/Core/hal.cpp
    SmartHome/Core/light.cpp
    SmartHome/Core/ac.cpp
    SmartHome/Core/sensor.cpp
    SmartHome/Core/home_ctrl.cpp
#    Dependencies/DHT22/src/DHT22.cpp
)

target_include_directories(SmartHomeCore PUBLIC
    SmartHome/Core/
#    Dependencies/DHT22/include/
)

target_link_libraries(SmartHomeCore PUBLIC nlohmann_json::nlohmann_json)

#target_link_libraries(SmartHomeCore PUBLIC wiringPi)

add_executable(SmartHome SmartHome/main.cpp SmartHome/Tui/tui.cpp)
target_include_directories(SmartHome PRIVATE SmartHome/Tui/)
target_link_libraries(SmartHome PRIVATE
    SmartHomeCore
    ftxui::screen
    ftxui::dom
    ftxui::component
)
