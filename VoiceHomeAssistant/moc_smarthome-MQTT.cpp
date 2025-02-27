/****************************************************************************
** Meta object code from reading C++ file 'smarthome-MQTT.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../smarthome-MQTT/smarthome-MQTT/smarthome-MQTT.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smarthome-MQTT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_smarthomeMQTT_t {
    QByteArrayData data[7];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_smarthomeMQTT_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_smarthomeMQTT_t qt_meta_stringdata_smarthomeMQTT = {
    {
QT_MOC_LITERAL(0, 0, 13), // "smarthomeMQTT"
QT_MOC_LITERAL(1, 14, 9), // "onToggled"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 11), // "deviceState"
QT_MOC_LITERAL(4, 37, 7), // "message"
QT_MOC_LITERAL(5, 45, 14), // "QMqttTopicName"
QT_MOC_LITERAL(6, 60, 5) // "topic"

    },
    "smarthomeMQTT\0onToggled\0\0deviceState\0"
    "message\0QMqttTopicName\0topic"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_smarthomeMQTT[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       3,    2,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QByteArray, 0x80000000 | 5,    4,    6,

       0        // eod
};

void smarthomeMQTT::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<smarthomeMQTT *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->deviceState((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< const QMqttTopicName(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMqttTopicName >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject smarthomeMQTT::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_smarthomeMQTT.data,
    qt_meta_data_smarthomeMQTT,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *smarthomeMQTT::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *smarthomeMQTT::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_smarthomeMQTT.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int smarthomeMQTT::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
