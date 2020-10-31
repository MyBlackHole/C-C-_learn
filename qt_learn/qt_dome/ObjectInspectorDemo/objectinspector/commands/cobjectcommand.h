#ifndef COBJECTCOMMAND_H
#define COBJECTCOMMAND_H

#include <QUndoCommand>
#include <QVariant>

class CObjectCommand : public QUndoCommand
{
public:
    CObjectCommand(const QString & propertyName, const QList<QObject*> & objects, const QVariant & oldValue = QVariant(), const QVariant & newValue = QVariant(), QUndoCommand * parent = 0);

protected:
    inline  const   QString &   propertyName()  const { return fpropName; }
    inline  const   QVariant&   oldValue()  const { return foldValue; }
    inline  const   QVariant&   newValue()  const { return fnewValue; }
    inline  void    setNewValue(const QVariant & Value){ fnewValue = Value; }
    virtual void    updateCommandText() = 0;

protected:
    QList<QObject*> fobjects;

private:
    QString         fpropName;
    QVariant        foldValue;
    QVariant        fnewValue;
};

#endif // COBJECTCOMMAND_H
