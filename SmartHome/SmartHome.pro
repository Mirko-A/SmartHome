QT += core gui          \
      multimedia        \
      multimediawidgets \
      widgets           \
      charts            \
      network           \
      xml               \

LIBS += -L/usr/local/lib -lwiringPi

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ac.cpp \
    analytics_model.cpp \
    hal.cpp \
    home_config.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    media_player.cpp \
    player_controls.cpp \
    playlist_model.cpp \
    sensor.cpp

HEADERS += \
    ac.h \
    analytics_model.h \
    hal.h \
    home_config.h \
    light.h \
    mainwindow.h \
    media_player.h \
    player_controls.h \
    playlist_model.h \
    sensor.h \
    smart_home_types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

INCLUDEPATH += $$PWD/../Dependencies/json/include/   \
               $$PWD/../Dependencies/DHT22/include/
