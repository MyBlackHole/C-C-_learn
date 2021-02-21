/****************************************************************************
** Meta object code from reading C++ file 'sudokutable.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../sudoku/sudokutable.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sudokutable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SudokuTable_t {
    QByteArrayData data[22];
    char stringdata0[230];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SudokuTable_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SudokuTable_t qt_meta_stringdata_SudokuTable = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SudokuTable"
QT_MOC_LITERAL(1, 12, 18), // "setPreviousEnabled"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "flag"
QT_MOC_LITERAL(4, 37, 14), // "setNextEnabled"
QT_MOC_LITERAL(5, 52, 9), // "newPuzzle"
QT_MOC_LITERAL(6, 62, 8), // "previous"
QT_MOC_LITERAL(7, 71, 4), // "next"
QT_MOC_LITERAL(8, 76, 5), // "erase"
QT_MOC_LITERAL(9, 82, 5), // "clear"
QT_MOC_LITERAL(10, 88, 13), // "dataSolutions"
QT_MOC_LITERAL(11, 102, 16), // "setDataSolutions"
QT_MOC_LITERAL(12, 119, 3), // "num"
QT_MOC_LITERAL(13, 123, 7), // "eyeMode"
QT_MOC_LITERAL(14, 131, 10), // "setEyeMode"
QT_MOC_LITERAL(15, 142, 7), // "godMode"
QT_MOC_LITERAL(16, 150, 10), // "setGodMode"
QT_MOC_LITERAL(17, 161, 18), // "drawBasicBackgroud"
QT_MOC_LITERAL(18, 180, 21), // "drawEyeModeBackground"
QT_MOC_LITERAL(19, 202, 3), // "row"
QT_MOC_LITERAL(20, 206, 6), // "column"
QT_MOC_LITERAL(21, 213, 16) // "somethingChanged"

    },
    "SudokuTable\0setPreviousEnabled\0\0flag\0"
    "setNextEnabled\0newPuzzle\0previous\0"
    "next\0erase\0clear\0dataSolutions\0"
    "setDataSolutions\0num\0eyeMode\0setEyeMode\0"
    "godMode\0setGodMode\0drawBasicBackgroud\0"
    "drawEyeModeBackground\0row\0column\0"
    "somethingChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SudokuTable[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       4,    1,   97,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  100,    2, 0x0a /* Public */,
       6,    0,  101,    2, 0x0a /* Public */,
       7,    0,  102,    2, 0x0a /* Public */,
       8,    0,  103,    2, 0x0a /* Public */,
       9,    0,  104,    2, 0x0a /* Public */,
      10,    0,  105,    2, 0x0a /* Public */,
      11,    1,  106,    2, 0x0a /* Public */,
      13,    0,  109,    2, 0x0a /* Public */,
      14,    1,  110,    2, 0x0a /* Public */,
      15,    0,  113,    2, 0x0a /* Public */,
      16,    1,  114,    2, 0x0a /* Public */,
      17,    0,  117,    2, 0x0a /* Public */,
      18,    2,  118,    2, 0x0a /* Public */,
      21,    2,  123,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   20,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   20,

       0        // eod
};

void SudokuTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SudokuTable *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setPreviousEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setNextEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->newPuzzle(); break;
        case 3: _t->previous(); break;
        case 4: _t->next(); break;
        case 5: _t->erase(); break;
        case 6: _t->clear(); break;
        case 7: { int _r = _t->dataSolutions();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->setDataSolutions((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: { bool _r = _t->eyeMode();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: _t->setEyeMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: { bool _r = _t->godMode();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->setGodMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->drawBasicBackgroud(); break;
        case 14: _t->drawEyeModeBackground((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->somethingChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SudokuTable::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SudokuTable::setPreviousEnabled)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SudokuTable::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SudokuTable::setNextEnabled)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SudokuTable::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_meta_stringdata_SudokuTable.data,
    qt_meta_data_SudokuTable,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SudokuTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SudokuTable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SudokuTable.stringdata0))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int SudokuTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void SudokuTable::setPreviousEnabled(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SudokuTable::setNextEnabled(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
