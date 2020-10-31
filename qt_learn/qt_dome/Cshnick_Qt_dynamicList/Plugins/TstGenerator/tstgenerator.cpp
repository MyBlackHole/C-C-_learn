#include "tstgenerator.h"
#include <QtGui>

static const QUrl storageUrl = QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236");


TstGenerator::TstGenerator(QObject *parent)
    : DPImageServicer(parent)
{
}

qint64 TstGenerator::imageCount() const
{
    QDir dir(data(Globals::InternalPathRole).toString());
    return dir.entryInfoList(QStringList() << "*.thumbnail.jpg", QDir::Files | QDir::NoDotAndDotDot, QDir::Name).count();
}

QImage TstGenerator::imageForindex(int index) const
{
    QString fileName = QString("page%1.thumbnail.jpg").arg(index, 3, 10, QLatin1Char('0'));
    QString filePath = data(Globals::InternalPathRole).toString() + "/" + fileName;

    if (QFileInfo(filePath).exists()) {
       return QImage(filePath);
    }

    return QImage();
}

Q_EXPORT_PLUGIN2(TstGenerator, TstGenerator)

