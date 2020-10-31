#ifndef GSAMPLEITEM_P_H
#define GSAMPLEITEM_P_H

#include <QList>
#include <QMetaType>

//class GSampleItemPrivate
//{
//public:
//    int fileRow;
//    QList<int> headers;
//    QList<int> channels;
//    QList<int> fluors;
//    QList<int> chNames;
//    int nameRow;     //样本名称
//    int typeRow;     //样本类型
//    QList<int> selects;
//    QList<int> values;
//    QList<int> valShows;
//};



struct _TreeStruct{
    bool expand;
    int  row;
    QList<int> items;
};
Q_DECLARE_METATYPE(_TreeStruct)

class GSampleItemPrivate
{
public:
    int fileRow;
    QList<int> headers;
    QList<_TreeStruct> channelItems;    //通道设置
    int roxRow;     //参比荧光

    int lockRow;     //锁定
    int nameRow;     //样本名称
    int typeRow;     //样本类型
    QList<_TreeStruct> sidItems;        //样本编号
    QList<_TreeStruct> uidItems;        //唯一编码
    QList<_TreeStruct> useItems;     //使用通道
};

#endif // GSAMPLEITEM_P_H
