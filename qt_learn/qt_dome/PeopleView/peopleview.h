#ifndef PEOPLEVIEW_H
#define PEOPLEVIEW_H

#include <QListView>
#include <QStandardItemModel>
#include <QScrollBar>

#include "person.h"
#include "persondelegate.h"

class PeopleView : public QListView
{
    Q_OBJECT
public:
    explicit PeopleView(QWidget *parent = 0);

    void setPeople(QList<Person> &people);

private:
    QStandardItemModel *m_model;
};

#endif // PEOPLEVIEW_H
