#include "person.h"

Person::Person()
{

}

Person::Person(const QString &firstname, const QString &surname)
    : m_firstname(firstname)
    , m_surname(surname)
{

}

void Person::setFirstName(const QString &firstname)
{
    m_firstname = firstname;
}

QString Person::firstName() const
{
    return m_firstname;
}

void Person::setSurName(const QString &surname)
{
    m_surname = surname;
}

QString Person::surname() const
{
   return m_surname;
}

QString Person::name() const
{
    return m_firstname + " " + m_surname;
}
