#ifndef GSAMPLEWIDGET_P_H
#define GSAMPLEWIDGET_P_H

#include "gwidget.h"

#define ASSIGN_NONE         0
#define ASSIGN_ENABLE       1
#define ASSIGN_SAMPLE_UID   2
#define ASSIGN_REPORT_UID   3
#define ASSIGN_TYPE         4
#define ASSIGN_CH_FLAG      5
#define ASSIGN_NAME         6
#define ASSIGN_ID           7
#define ASSIGN_CON_FLAG     8
#define ASSIGN_CON_VAL      9

QT_BEGIN_NAMESPACE
class GSampleWidget;
class GSampleData;
class QTableView;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;
class QItemSelection;
class GSampleItemPrivate;
class GHoverDelayObject;
class QMenu;
QT_END_NAMESPACE

#define UNIQUE_ENCODE_LAST_LEN 2

class GSampleWidgetPrivate
{
    Q_DECLARE_PUBLIC(GSampleWidget)

public:
    GSampleWidgetPrivate();
    ~GSampleWidgetPrivate();

    GSampleWidget *q_ptr;
    QWidget *parent;
    GSampleData *sampleData;

//    bool mousePressed;

    QStandardItemModel  *modelSet;
    GSampleItemPrivate  *itemSet;

    QMenu   *rightMenu;
    QAction *actionLock;
    QAction *actionUnlock;

    GHoverDelayObject *hoverDelay;  //鼠标悬停延时类指针

    bool isLock;    //是否锁定，不能修改

    QList<QAction *> rightMenuActions;

    QStringList channelNames;
    QStringList sampleNames;

    mutable QHash< int, QPair<QVariant, QVariant> > task;

    void init();
    bool isLocked();
    _WellSampleInfo selectedWell();
    void updateChannelName(_WellSampleInfo *well);
    int  updateSampleType();
    void updateToolEditable(bool edit);
    void updateToolInfo(const _WellSampleInfo &info);

    void _g_itemChanged(QStandardItem * item);
    void _g_itemClicked(const QModelIndex & index);
    void _g_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void _g_cleanSelection();
    void _g_cleanAll();
    void _g_patientInfo();
    void _g_lockSample(bool isRightMenu = true);
    void _g_unlockSample(bool isRightMenu = true);
    void _g_sampleChanged();

    void _g_fileChanged();
};

#endif // GSAMPLEWIDGET_P_H
