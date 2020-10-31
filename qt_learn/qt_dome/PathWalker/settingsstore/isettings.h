#ifndef ISTORESETTINGS_H
#define ISTORESETTINGS_H

#include <QFrame>

class ISettings: public QFrame
{
    Q_OBJECT
public:
    ISettings(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual bool settingsSave()=0;
    virtual bool settingsRestore()=0;
};

#endif // ISTORESETTINGS_H
