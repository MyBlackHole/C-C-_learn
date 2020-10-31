#ifndef GSAMPLEITEMDELEGATE_H
#define GSAMPLEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class GSampleItemPrivate;
class QTableView;
QT_END_NAMESPACE

class GSampleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit GSampleItemDelegate(QStringList* channel, int* rox, QStringList* sample, GSampleItemPrivate* setData, QObject* parent = 0);
    
    virtual void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const;

protected:
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
    GSampleItemPrivate* m_set;

    QStringList* const m_channel_names;
    QStringList* const m_sample_names;
    QTableView* m_tableView;
    int* m_rox;
};

Q_DECLARE_METATYPE(QTableView *)

#endif // GSAMPLEITEMDELEGATE_H
