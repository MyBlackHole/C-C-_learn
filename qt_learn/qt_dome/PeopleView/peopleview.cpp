#include "peopleview.h"

bool sortPeople(const  Person &p1, const Person &p2)
{
    return p1.surname() < p2.surname();
}

PeopleView::PeopleView(QWidget *parent)
    : QListView(parent)
{
    setItemDelegate(new PersonDelegate(this));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFrameShape(QFrame::NoFrame);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    verticalScrollBar()->setPageStep(3);
    verticalScrollBar()->setSingleStep(1);

    m_model = new QStandardItemModel(this);
    setModel(m_model);
}

void PeopleView::setPeople(QList<Person> &people)
{
    setModel(NULL);
    m_model->clear();

    qSort(people.begin(), people.end(), sortPeople);
    QString letter = "";
    for(int i = 0; i < people.size(); i ++)
    {
        /* If letter is different than the first letter of the person's surname */
        /* it means that we must change letter and so add an header to display it */
        if(letter != people.at(i).surname().at(0).toLower())
        {
            letter = people.at(i).surname().at(0).toLower();
            QStandardItem *item = new QStandardItem(letter);
            m_model->appendRow(item);
        }

        QStandardItem *item = new QStandardItem();
        item->setData(QVariant::fromValue(people.at(i)), Qt::DisplayRole);
        m_model->appendRow(item);
    }

    setModel(m_model);
}
