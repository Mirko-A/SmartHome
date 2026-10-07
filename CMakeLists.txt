cmake_minimum_required(VERSION 3.14)
project(SmartHomeCore CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(Threads REQUIRED)
find_package(Qt5 REQUIRED
    COMPONENTS Widgets Multimedia MultimediaWidgets Charts Svg
)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v5.0.0
)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(ftxui json)

#if(NOT EXISTS "${CMAKE_SOURCE_DIR}/Dependencies/DHT22/include/DHT22.h")
#    message(FATAL_ERROR
#        "DHT22 submodule not initialized.\n"
#        "Run: git submodule update --init --recursive")
#endif()

add_library(SmartHomeCore STATIC
    SmartHome/Core/Hal/hal.cpp
    SmartHome/Core/Hal/gpio.cpp
    SmartHome/Core/Hal/owned_pin.cpp
    SmartHome/Core/light.cpp
    SmartHome/Core/ac.cpp
    SmartHome/Core/sensor.cpp
    SmartHome/Core/home_ctrl.cpp
    SmartHome/Core/home_settings.cpp
#    Dependencies/DHT22/src/DHT22.cpp
)

target_include_directories(SmartHomeCore PUBLIC
    SmartHome/Core/
    SmartHome/Core/Hal/
#    Dependencies/DHT22/include/
)

target_link_libraries(SmartHomeCore PUBLIC nlohmann_json::nlohmann_json)
#target_link_libraries(SmartHomeCore PUBLIC wiringPi)

add_executable(SmartHome
    # Entry point
    SmartHome/main.cpp
    # GUI
    SmartHome/Gui/gui.cpp
    SmartHome/Gui/gui_session.cpp
    SmartHome/Gui/analytics_model.cpp
    SmartHome/Gui/main_window.cpp
    SmartHome/Gui/media_player.cpp
    SmartHome/Gui/player_controls.cpp
    SmartHome/Gui/playlist_model.cpp
    # TUI
    SmartHome/Tui/tui.cpp
    SmartHome/Tui/tui_app.cpp
    SmartHome/Tui/components/ac.cpp
    SmartHome/Tui/components/light.cpp
    SmartHome/Tui/components/sensors.cpp
    SmartHome/Tui/components/speaker.cpp
    # Shared
    SmartHome/Shared/config_io.cpp
    # Resources
    SmartHome/icons_resource.qrc
)
set_target_properties(SmartHome PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
target_include_directories(SmartHome PRIVATE
    SmartHome/Gui/
    SmartHome/Tui/
    SmartHome/Shared/
)
target_link_libraries(SmartHome PRIVATE
    # Internal
    SmartHomeCore
    # External
    Threads::Threads

    ftxui::screen
    ftxui::dom
    ftxui::component

    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Charts
    Qt5::Svg
)
