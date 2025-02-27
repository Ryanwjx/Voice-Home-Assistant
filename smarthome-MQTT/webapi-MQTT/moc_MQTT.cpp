/****************************************************************************
** Meta object code from reading C++ file 'MQTT.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MQTT.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MQTT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MQTT_t {
    QByteArrayData data[9];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MQTT_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MQTT_t qt_meta_stringdata_MQTT = {
    {
QT_MOC_LITERAL(0, 0, 4), // "MQTT"
QT_MOC_LITERAL(1, 5, 24), // "mqttMessageReceivedReady"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 7), // "message"
QT_MOC_LITERAL(4, 39, 14), // "QMqttTopicName"
QT_MOC_LITERAL(5, 54, 5), // "topic"
QT_MOC_LITERAL(6, 60, 13), // "MQTTConnected"
QT_MOC_LITERAL(7, 74, 15), // "MQTTDisonnected"
QT_MOC_LITERAL(8, 90, 17) // "onMessageReceived"

    },
    "MQTT\0mqttMessageReceivedReady\0\0message\0"
    "QMqttTopicName\0topic\0MQTTConnected\0"
    "MQTTDisonnected\0onMessageReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MQTT[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   39,    2, 0x08 /* Private */,
       7,    0,   40,    2, 0x08 /* Private */,
       8,    2,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray, 0x80000000 | 4,    3,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray, 0x80000000 | 4,    3,    5,

       0        // eod
};

void MQTT::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MQTT *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mqttMessageReceivedReady((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< const QMqttTopicName(*)>(_a[2]))); break;
        case 1: _t->MQTTConnected(); break;
        case 2: _t->MQTTDisonnected(); break;
        case 3: _t->onMessageReceived((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< const QMqttTopicName(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMqttTopicName >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMqttTopicName >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MQTT::*)(const QByteArray & , const QMqttTopicName & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MQTT::mqttMessageReceivedReady)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MQTT::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_MQTT.data,
    qt_meta_data_MQTT,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MQTT::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MQTT::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MQTT.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MQTT::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void MQTT::mqttMessageReceivedReady(const QByteArray & _t1, const QMqttTopicName & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
