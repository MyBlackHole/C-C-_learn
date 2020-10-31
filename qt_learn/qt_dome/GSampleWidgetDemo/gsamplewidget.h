#ifndef GSAMPLEWIDGET_H
#define GSAMPLEWIDGET_H

#include "gwidget.h"
#include <QMap>
#include <QStringList>

namespace Ui {
class GSampleWidget;
}

QT_BEGIN_NAMESPACE
class GSampleWidgetPrivate;
class GSampleData;
class QModelIndex;
class QStandardItem;
QT_END_NAMESPACE

class GSampleWidget : public GWidget
{
    Q_OBJECT

public:
    explicit GSampleWidget(GSampleData *data, QWidget *parent = 0);
    ~GSampleWidget();   

    void start();
    void stop();

    void reflesh();
    QStringList chlFluors();    
signals:
    void propertyChanged();
    void patientInfoChanged();
    void channelEnabled(int channel, bool enabled);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_buttonShow_clicked();
    void on_tableView_doubleClicked(const QModelIndex & index);

    void slot_toolBox_hide();

    void slot_rightClickMenu(const QPoint &pos);
    void slot_setSampleFluorType();
private:
    Ui::GSampleWidget *ui;
    GSampleWidgetPrivate *d_ptr;

    Q_DECLARE_PRIVATE(GSampleWidget)
    Q_DISABLE_COPY(GSampleWidget)

    Q_PRIVATE_SLOT(d_func(), void _g_itemChanged(QStandardItem *item))
    Q_PRIVATE_SLOT(d_func(), void _g_itemClicked(const QModelIndex & index))
    Q_PRIVATE_SLOT(d_func(), void _g_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected))
    Q_PRIVATE_SLOT(d_func(), void _g_cleanSelection())
    Q_PRIVATE_SLOT(d_func(), void _g_cleanAll())
    Q_PRIVATE_SLOT(d_func(), void _g_lockSample())
    Q_PRIVATE_SLOT(d_func(), void _g_unlockSample())
    Q_PRIVATE_SLOT(d_func(), void _g_patientInfo())
    Q_PRIVATE_SLOT(d_func(), void _g_sampleChanged())

    Q_PRIVATE_SLOT(d_func(), void _g_fileChanged())
};

#endif // GSAMPLEWIDGET_H
