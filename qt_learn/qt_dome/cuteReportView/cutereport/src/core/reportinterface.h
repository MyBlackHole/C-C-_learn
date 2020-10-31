/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2015 by Alexander Mikhalov                         *
 *   alexander.mikhalov@gmail.com                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#ifndef REPORTINTERFACE_H
#define REPORTINTERFACE_H

#include <QObject>
#include "cutereport_globals.h"
#include <QPointer>
#include <QPointF>
#include <QHash>
#include <QVariant>

namespace CuteReport
{

class ReportCore;
class BaseItemInterface;
class DatasetInterface;
class PageInterface;
class RendererInterface;
class PrinterInterface;
class FormInterface;
class StorageInterface;
class RenderedReportInterface;

class CUTEREPORT_EXPORTS ReportInterface : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString script READ script WRITE setScript DESIGNABLE false NOTIFY scriptChanged)
    Q_PROPERTY(int version READ version WRITE setVersion DESIGNABLE false)
    Q_PROPERTY(QString fileUrl READ fileUrl WRITE setFilePath NOTIFY fileUrlChanged)
    Q_PROPERTY(QString defaultStorage READ defaultStorageName WRITE setDefaultStorageName NOTIFY defaultStorageChanged)
    Q_PROPERTY(QString defaultRenderer READ defaultRendererName WRITE setDefaultRendererName NOTIFY defaultRendererChanged)
    Q_PROPERTY(QString defaultPrinter READ defaultPrinterName WRITE setDefaultPrinterName NOTIFY defaultPrinterChanged)
    Q_PROPERTY(QVariantHash variables READ variables WRITE setVariables NOTIFY variablesChanged)

public:

    enum ReportFlag {
        VariablesAutoUpdate = 0x01,
        DirtynessAutoUpdate = 0x02
    };
    Q_DECLARE_FLAGS(ReportFlags, ReportFlag)
    // WARNING do not include QScriptEngine: all custom application will need to add it to pro
//    struct FunctionValue
//    {
//        QScriptEngine::FunctionSignature function;
//        int args;
//        QScriptValue::PropertyFlags flags;
//    };

//    typedef QMap<QString, FunctionValue> FunctionValues;

//    struct ReportValue
//    {
//        QVariant value;
//        QScriptValue::PropertyFlags flags;
//    };

//    typedef QMap<QString, ReportValue> ReportValues;

    ~ReportInterface();

    ReportInterface * clone(bool withChildren = true, bool init = true);

    void init();

    QString name();
    void setName(const QString & name);

	QString author();
	void setAuthor(const QString & author);

	virtual QString script();
	virtual void setScript(const QString & script);

    double version();
    void setVersion(double tVersion);

    QString description();
    void setDescription(const QString & description);

    QString fileUrl();
    void setFilePath(const QString & fileUrl);

    QList<CuteReport::BaseItemInterface *> items(const QString & pageObjectName = QString());
    CuteReport::BaseItemInterface * item (const QString & itemName);

    QList<CuteReport::PageInterface *> pages();
    QStringList pageNames();
    CuteReport::PageInterface * page(const QString & pageName);
    void addPage (CuteReport::PageInterface * page);
    void deletePage(CuteReport::PageInterface * page);
    void deletePage(const QString & pageName);

    QList<CuteReport::DatasetInterface *> datasets();
    QStringList datasetNames();
    CuteReport::DatasetInterface * dataset(const QString & datasetName);
    void addDatasets(QList<CuteReport::DatasetInterface *> datasets);
    void addDataset(CuteReport::DatasetInterface* dataset);
    void deleteDataset(CuteReport::DatasetInterface* dataset);
    void deleteDataset(const QString & datasetName);

    QList<CuteReport::RendererInterface *> renderers();
    QStringList rendererNames();
    CuteReport::RendererInterface * renderer(const QString & rendererName = QString());
    void addRenderer(RendererInterface * renderer);
    void deleteRenderer(CuteReport::RendererInterface * renderer);
    void deleteRenderer(const QString & rendererName);
    QString defaultRendererName() const;
    void setDefaultRendererName(const QString &name);

    QList<CuteReport::PrinterInterface *> printers();
    QStringList printerNames();
    CuteReport::PrinterInterface * printer(const QString & printerName = QString());
    void addPrinter(CuteReport::PrinterInterface *printer);
    void deletePrinter(CuteReport::PrinterInterface * printer);
    void deletePrinter(const QString & printerName);
    QString defaultPrinterName() const;
    void setDefaultPrinterName(const QString &name);

    CuteReport::StorageInterface * storage(const QString & objectName) const;
    QStringList storageNames() const;
    CuteReport::StorageInterface * storageByUrl(const QString & url) const;
    QList<CuteReport::StorageInterface*> storageListByScheme(const QString & scheme) const;
    QList<CuteReport::StorageInterface*> storageListByModuleName(const QString & moduleName) const;
    QList<StorageInterface *> storages() const;
    void addStorage(CuteReport::StorageInterface *storage);
    void deleteStorage(const QString & storageName);
    void deleteStorage(StorageInterface * storage);
    bool hasStorageModule(const QString & moduleName);
    QString defaultStorageName() const;
    void setDefaultStorageName(const QString &name);

    QList<CuteReport::FormInterface*> forms();
    CuteReport::FormInterface* form(const QString & formName);
    void addForm(CuteReport::FormInterface * form );
    void deleteForm(CuteReport::FormInterface * form );

	virtual int suitId();

    const QVariantHash & variables();
    void setVariables(const QVariantHash &vars);
    void setVariableValue(const QString & name, const QVariant & value);

    void setFlags(ReportFlags flags);
    void setFlag(ReportFlag flag, bool enable);
    bool isFlagSet(ReportFlag flag);
    ReportFlags flags();

    bool isDirty() const;
    bool isValid() const;

    RenderedReportInterface *renderedReport() const;
    void setRenderedReport(RenderedReportInterface *renderedReport);
    void clearRenderedReport();

public slots:
    void updateVariables();
    void setDirty(bool b = true);
    void setValid(bool b = true);
    void setInvalid();

signals:
    void nameChanged(QString);
    void authorChanged(QString);
    void descriptionChanged(QString);
    void scriptChanged(QString);
    void fileUrlChanged(QString);
    void rendererAdded(CuteReport::RendererInterface*);
    void rendererDeleted(CuteReport::RendererInterface*);
    void rendererDeleted(QString rendererName);
    void printerAdded(CuteReport::PrinterInterface*);
    void printerDeleted(CuteReport::PrinterInterface*);
    void printerDeleted(QString printerName);
    void storageAdded(CuteReport::StorageInterface*);
    void storageDeleted(CuteReport::StorageInterface*);
    void storageDeleted(QString storageName);
    void datasetAdded(CuteReport::DatasetInterface * dataset);
    void datasetDeleted(CuteReport::DatasetInterface * dataset);
    void datasetDeleted(QString datasetName);
    void formAdded(CuteReport::FormInterface*);
    void formDeleted(CuteReport::FormInterface*);
    void pageAdded(CuteReport::PageInterface*);
    void pageDeleted(CuteReport::PageInterface*);
    void itemAdded(CuteReport::BaseItemInterface*);
    void itemDeleted(CuteReport::BaseItemInterface*, bool);
    void defaultStorageChanged(QString);
    void defaultRendererChanged(QString);
    void defaultPrinterChanged(QString);
    void variablesChanged();
    void dirtynessChanged(bool);
    void validityChanged(bool);
    void changed();
    void propertyChanged();

private slots:
    void childDestroyed(QObject * object);
    void slotItemAdded(CuteReport::BaseItemInterface *item);
    void slotItemRemoved(CuteReport::BaseItemInterface* item, QString, bool directDeletion);
    void slotScriptStringsChanged();
    void slotObjectVariablesChanged();
    void slotNewItemAdded(CuteReport::BaseItemInterface* item);

protected:
    explicit ReportInterface(QObject *parent = 0);
    explicit ReportInterface(const ReportInterface & dd, QObject * parent);

    void precessFlags(ReportInterface::ReportFlags previousFlags);
    void setUniqueName(QObject * object);

	QString m_name;
	QString m_author;
	QString m_script;
    double m_version;
    QString m_description;
    QString m_filePath;
    QString m_defaultStorageName;
    QString m_defaultRendererName;
    QString m_defaultPrinterName;
    QVariantHash m_variables;
    ReportFlags m_flags;
    bool m_isDirty;
    bool m_isValid;

    RenderedReportInterface * m_renderedReport;

    friend class ReportCore;
};


}   //namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(CuteReport::ReportInterface::ReportFlags)


#endif
