QT       += core gui widgets mqtt network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = SmartHome
TEMPLATE = app

SOURCES += \
    SerialPort.cpp \
    main.cpp \
    loginwidget.cpp \
    mainform.cpp \
    hardwarepage.cpp \
    mqttpage.cpp \
    aipage.cpp \
    fsmpKeyboard/keyboard.cpp

HEADERS += \
    SerialPort.h \
    loginwidget.h \
    mainform.h \
    hardwarepage.h \
    mqttpage.h \
    aipage.h \
    fsmpKeyboard/keyboard.h \
    hardware/fsmpLed.h \
    hardware/fsmpBeeper.h \
    hardware/fsmpFan.h \
    hardware/fsmpCamera.h \
    hardware/fsmpEvents.h \
    hardware/fsmpTempHum.h \
    hardware/fsmpLight.h \
    hardware/fsmpVibrator.h \
    text2qrcode/text2qrcode.h \
    text2qrcode/qrencode.h

FORMS += \
    loginwidget.ui \
    hardwarepage.ui \
    mqttpage.ui \
    aipage.ui \
    fsmpKeyboard/keyboard.ui

INCLUDEPATH += $$PWD/fsmpKeyboard \
               $$PWD/hardware \
               $$PWD/text2qrcode

# qrencode 二维码静态库（项目自带 text2qrcode/libqrencode.a）
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/text2qrcode/release/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/text2qrcode/debug/ -lqrencode
else:unix: LIBS += -L$$PWD/text2qrcode/ -lqrencode

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/release/libqrencode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/debug/libqrencode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/release/qrencode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/debug/qrencode.lib
else:unix: PRE_TARGETDEPS += $$PWD/text2qrcode/libqrencode.a

RESOURCES += \
    resources/res.qrc \
    resources/keyboard.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
unix {
    INCLUDEPATH += $$PWD/text2qrcode
    LIBS += -L$$PWD/text2qrcode -lqrencode
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/text2qrcode/release/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/text2qrcode/debug/ -lqrencode
else:unix: LIBS += -L$$PWD/text2qrcode/ -lqrencode

INCLUDEPATH += $$PWD/text2qrcode
DEPENDPATH += $$PWD/text2qrcode

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/release/libqrencode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/debug/libqrencode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/release/qrencode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/text2qrcode/debug/qrencode.lib
else:unix: PRE_TARGETDEPS += $$PWD/text2qrcode/libqrencode.a
