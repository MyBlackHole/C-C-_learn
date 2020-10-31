#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H

#include <QApplication>
#include <QEvent>
#include "D:/workspace/QtCreatorProjects/PathWalker/settingsstore/isettings.h"

class SettingsSaver: public QObject
{
public:
    SettingsSaver(QObject *parent= 0):QObject(parent){}

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if (event->type() == QEvent::Close) {
            foreach(QWidget *w, QApplication::allWidgets()){
                ISettings *is= qobject_cast<ISettings*>(w);
                if(is==0) continue;
                is->settingsSave();
            }
        } else {
            if(event->type() == QEvent::Show){
                foreach(QWidget *w, QApplication::allWidgets()){
                    ISettings *is= qobject_cast<ISettings*>(w);
                    if(is==0) continue;
                    is->settingsRestore();
                }
            }
        }
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
};

class SettingsStore
{
public:
    // install event filter on window to
    // call ISettings::settingsSave/ISettings::settingsRestore
    // on close/show event
    static void registerWindow(QWidget *w);
    static SettingsSaver* saver();
    static void cfgPathSet(const QString & path);
    static QString cfgPath();

private:
   SettingsStore(){}  // Private so that it can  not be called
   SettingsStore(SettingsStore const&){}             // copy constructor is private
   SettingsStore& operator=(SettingsStore const&){return *this;}  // assignment operator is private
   static SettingsSaver *aSaver;
   static QString aCfgPath;
};

#endif // SETTINGSSTORE_H
