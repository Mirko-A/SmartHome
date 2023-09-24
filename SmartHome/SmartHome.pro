QT += core gui          \
      multimedia        \
      multimediawidgets \
      widgets           \
      charts            \
      network           \
      xml               \

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analytics_model.cpp \
    hal.cpp \
    home_config.cpp \
    main.cpp \
    mainwindow.cpp \
    media_player.cpp \
    player_controls.cpp \
    playlist_model.cpp

HEADERS += \
    analytics_model.h \
    hal.h \
    home_config.h \
    mainwindow.h \
    media_player.h \
    player_controls.h \
    playlist_model.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

INCLUDEPATH += $$PWD/../Dependencies/json/include/
