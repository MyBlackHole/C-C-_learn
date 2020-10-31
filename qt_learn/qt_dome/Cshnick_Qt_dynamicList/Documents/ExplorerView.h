#ifndef EXPLORERVIEW_H
#define EXPLORERVIEW_H

#include "DocumentManager.h"
#include <QTreeView>

namespace Docs {

class ExplorerViewPrivate;

class ExplorerView : public QTreeView
{
    Q_OBJECT
public:
    explicit ExplorerView(QWidget *parent = 0);
    ~ExplorerView();
    
private:
    ExplorerViewPrivate *d;

    friend class ExplorerViewPrivate;
    
};

} //namespace Docs

#endif // EXPLORERVIEW_H
