#include "renderedmemoitem.h"
#include "memo.h"
#include "memo_p.h"
#include "rendererpublicinterface.h"


RenderedMemoItem::RenderedMemoItem(CuteReport::BaseItemInterface * item, CuteReport::BaseItemInterfacePrivate *itemPrivateData)
    :RenderedItemInterface(item, itemPrivateData)
{
}


void RenderedMemoItem::paint(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    MemoItem::paint(painter, option, d_ptr, boundingRect(), CuteReport::RenderingReport);
}


void RenderedMemoItem::redraw(bool withChildren)
{
    MemoItem::adjust(reinterpret_cast<MemoItemPrivate*>(d_ptr), QPointF());
    RenderedItemInterface::redraw(withChildren);
}
