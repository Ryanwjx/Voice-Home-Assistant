SOURCES += \
    $$PWD/smarthome-MQTT.cpp \
    $$PWD/switchbutton.cpp

HEADERS += \
    $$PWD/smarthome-MQTT.h \
    $$PWD/switchbutton.h

RESOURCES += \
    $$PWD/res.qrc

DISTFILES +=

include($$PWD/../webapi-MQTT/MQTT.pri)
