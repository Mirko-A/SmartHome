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
    Ui/analytics_model.cpp \
    Ui/mainwindow.cpp \
    Ui/media_player.cpp \
    Ui/player_controls.cpp \
    Ui/playlist_model.cpp \
    Ui/video_widget.cpp

HEADERS += \
    Ui/analytics_model.h \
    Ui/mainwindow.h \
    Ui/media_player.h \
    Ui/player_controls.h \
    Ui/playlist_model.h \
    Ui/video_widget.h

FORMS += \
    Ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

INCLUDEPATH += $$PWD/Core/                           \
               $$PWD/Ui/                             \
               $$PWD/../Dependencies/json/include/   \
               $$PWD/../Dependencies/DHT22/include/
