#include "PInfoHandler.h"

#include <QDomDocument>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QXmlStreamWriter>

#include "PluginManager.h"
#include "globals.h"

namespace Plugins {
class PInfoHandlerPrivate
{
    PInfoHandlerPrivate(PInfoHandler *pq)
        :q(pq)
        , mEnabled(false)
    {
    }

    void parseFile(const QString &fileName)
    {
        Q_ASSERT(QFileInfo(fileName).exists());

        mOwnPath = QUrl::fromLocalFile(fileName);
        QDomDocument xmlDom;
        QFile inFile(fileName);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString domString(inFile.readAll());

            int errorLine = 0; int errorColumn = 0;
            QString errorStr;

            if (xmlDom.setContent(domString, true, &errorStr, &errorLine, &errorColumn)) {
                mData = xmlDom;

                QString enabled = mData.documentElement().firstChildElement(tEnabled).text();
                mEnabled = enabled.isEmpty() ? true : q->boolForString(enabled);
                QString displayName = mData.documentElement().firstChildElement(tDisplayName).text();
                mDisplayName = displayName.isEmpty() ? dDefaultDisplayName : displayName;
                mIconSource = mData.documentElement().firstChildElement(tIconSource).text();
                mPluginFileName = mData.documentElement().firstChildElement(tPluginPath).text();
                Q_ASSERT(QFileInfo(absolutePluginPath()).exists());

            } else {
                qDebug() << "Error reading content of " << fileName << endl
                         << "Error:"  << inFile.errorString()
                         << "Line:"   << errorLine
                         << "Column:" << errorColumn;
            }
            inFile.close();
        } else {
            qDebug() << "Error reading" << fileName << endl
                     << "Error:" << inFile.errorString();
        }
    }

    void saveDom()
    {
        Q_ASSERT(QFileInfo(mPluginFileName).exists());

        QFile stream(mPluginFileName);
        if (!stream.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open " << mPluginFileName << "for writing PInfoHandlerPrivate::saveDom()";
            return;
        }
        QTextStream txtStream(&stream);

        mData.save(txtStream, 0);
        stream.close();
    }

    void saveXmlStreamReader()
    {
        QFile file(mOwnPath.toLocalFile());
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open " << mPluginFileName << "for writing PInfoHandlerPrivate::saveMembers()";
            return;
        }

        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement(tTopTag);
            writer.writeTextElement(tPluginPath, mPluginFileName);
            writer.writeTextElement(tEnabled, q->stringForBool(mEnabled));
            writer.writeTextElement(tDisplayName, mDisplayName);
            writer.writeTextElement(tIconSource, mIconSource);
        writer.writeEndElement();
        writer.writeEndDocument();
        file.close();
    }

    QString absolutePluginPath() const
    {
        QString pluginRelativePath = PluginManager::adjustedPluginName(mPluginFileName);
        if (pluginRelativePath.isNull()) {
            return QString();
        }

        QString returnPath = QFileInfo(mOwnPath.toLocalFile()).dir().absolutePath() + "/" + pluginRelativePath;
        return returnPath;
    }

private:
    PInfoHandler *q;
    QDomDocument mData;
    QString mPluginFileName;
    bool mEnabled;
    QString mDisplayName;
    QString mIconSource;
    QUrl mOwnPath;

    friend class PInfoHandler;
};

PInfoHandler::PInfoHandler()
    :d(new PInfoHandlerPrivate(this))
{
}

PInfoHandler::PInfoHandler(const PInfoHandler &other)
    : d(new PInfoHandlerPrivate(*other.d))
{
    d->q = this;
}

PInfoHandler::PInfoHandler(const QDomDocument &data)
    :d(new PInfoHandlerPrivate(this))
{
    d->mData = data;
}
PInfoHandler::PInfoHandler(const QString &dataFile)
    :d(new PInfoHandlerPrivate(this))
{
    d->parseFile(dataFile);
}

PInfoHandler::~PInfoHandler()
{
    if (d) {
        delete d;
        d = 0;
    }
}
void PInfoHandler::setData(const QDomDocument &pData)
{
    d->mData = pData;
}

void PInfoHandler::setFileData(const QString &fileData)
{
    d->parseFile(fileData);
}

void PInfoHandler::setEnabled(bool pEnabled)
{
    d->mEnabled = pEnabled;
}

bool PInfoHandler::isEnabled() const
{
    return d->mEnabled;
}

bool PInfoHandler::isValid() const
{
    return !d->mData.isNull() && QFileInfo(absolutePluginPath()).exists();
}

void PInfoHandler::save()
{
    d->saveXmlStreamReader();
}

QString PInfoHandler::absolutePluginPath() const
{
    return d->absolutePluginPath();
}

QString PInfoHandler::displayName() const
{
    return d->mDisplayName;
}

QString PInfoHandler::stringForBool(bool pArgument)
{
    return pArgument ? argTrue : argFalse;
}

bool PInfoHandler::boolForString(const QString &pArgument)
{
    return (pArgument == argTrue) ? true : false;
}

PInfoHandler &PInfoHandler::operator=(const PInfoHandler &other)
{
    *d = *other.d;
    d->q = this;
    return *this;
}

bool PInfoHandler::sameOwnPath(const PInfoHandler &other) const
{
    return d->mOwnPath == other.d->mOwnPath;
}

} //namespace Plugins
