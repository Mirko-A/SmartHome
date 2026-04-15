QT += core gui          \
      multimedia        \
      multimediawidgets \
      widgets           \
      charts            \
      network           \
      xml               \

LIBS += -L/usr/local/lib -lwiringPi \
        -L$$PWD/../build -lSmartHomeCore

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    Gui/analytics_model.cpp \
    Gui/mainwindow.cpp \
    Gui/media_player.cpp \
    Gui/player_controls.cpp \
    Gui/playlist_model.cpp \
    Gui/video_widget.cpp

HEADERS += \
    Gui/analytics_model.h \
    Gui/mainwindow.h \
    Gui/media_player.h \
    Gui/player_controls.h \
    Gui/playlist_model.h \
    Gui/video_widget.h

FORMS += \
    Gui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

INCLUDEPATH += $$PWD/Core/                           \
               $$PWD/Gui/                             \
               $$PWD/../Dependencies/json/include/   \
               $$PWD/../Dependencies/DHT22/include/
