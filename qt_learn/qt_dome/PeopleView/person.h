#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QMetaType>

class Person
{
public:
    explicit Person();
    Person(const QString &firstname, const QString &surname);
    virtual ~Person() {  ;}

    void setFirstName(const QString &firstname);
    QString firstName() const;

    void setSurName(const QString &surname);
    QString surname() const;

    QString name() const;
private:
    long m_id;

    QString m_firstname;
    QString m_surname;
};

Q_DECLARE_METATYPE(Person);

#endif // PERSON_H
