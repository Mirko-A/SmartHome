cmake_minimum_required(VERSION 3.14)
project(SmartHomeCore CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Verify submodules are initialized
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/Dependencies/json/include/nlohmann/json.hpp")
    message(FATAL_ERROR
        "nlohmann/json submodule not initialized.\n"
        "Run: git submodule update --init --recursive")
endif()

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
    SmartHome/Core/home_config.cpp
#    Dependencies/DHT22/src/DHT22.cpp
)

target_include_directories(SmartHomeCore PUBLIC
    SmartHome/Core/
    Dependencies/json/include/
#    Dependencies/DHT22/include/
)

#target_link_libraries(SmartHomeCore PUBLIC wiringPi)
