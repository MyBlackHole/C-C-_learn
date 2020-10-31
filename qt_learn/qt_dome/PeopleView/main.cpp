#include <QApplication>
#include <QTextCodec>
#include "peopleview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<Person> people;
    people << Person("Léonardo", "Di Caprio") << Person("Tom", "Cruise")
           << Person("Brad", "Pitt") << Person("Tom", "Hanks")
           << Person("Liam", "Neeson") << Person("Daniel", "Craig")
           << Person("Clint", "Eastwood") << Person("Christian", "Bale")
           << Person("Ryan", "Gosling") << Person("Angelina", "Jolie")
           << Person("Jennifer", "Aniston") << Person("Sharon", "Stone")
           << Person("Eva", "Mendes") << Person("Olga", "Kurylenko")
           << Person("Robert", "Downey Jr.") << Person("Harrison", "Ford")
           << Person("Eva", "Green") << Person("Olivia", "Wilde");

    PeopleView w;
    w.setPeople(people);
    w.show();

    return a.exec();
}
