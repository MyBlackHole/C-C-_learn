/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2013-2014 by Alexander Mikhalov                         *
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
 ***************************************************************************/
#ifndef CSVDATASET_H
#define CSVDATASET_H

#include "datasetinterface.h"
#include "reportinterface.h"
#include "globals.h"

#include <QtSql>
#include <QSortFilterProxyModel>

class Model;

class CsvDataset : public CuteReport::DatasetInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "CuteReport.DatasetInterface/1.0")
#endif
    Q_INTERFACES(CuteReport::DatasetInterface)

    Q_PROPERTY(QString fileName READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString delimeter READ getDelimeter WRITE setDelimeter NOTIFY delimiterChanged)
    Q_PROPERTY(bool firstRowIsHeader READ getFirstRowIsHeader WRITE setFirstRowIsHeader NOTIFY firstRowIsHeaderChanged)
    Q_PROPERTY(bool fixFileIssues READ getFixFileIssues WRITE setFixFileIssues NOTIFY fixFileIssuesChanged)

public:
    explicit CsvDataset(QObject *parent = 0);
    virtual ~CsvDataset();

    virtual QIcon icon();

    virtual DatasetInterface * createInstance(QObject* parent = 0) const;
    virtual CuteReport::DatasetHelperInterface * helper();
    virtual QAbstractItemModel * model();

    virtual QString getLastError();

    virtual bool populate();
    virtual bool isPopulated();
    virtual void setPopulated(bool b);
    virtual void reset();
    virtual void resetCursor();
    virtual bool setFirstRow();
    virtual bool setLastRow();
    virtual bool setNextRow();
    virtual bool setPreviousRow();
    virtual int  getCurrentRowNumber();
    virtual bool setCurrentRowNumber(int index);
    virtual int getRowCount();
    virtual int getColumnCount();
    virtual QVariant getValue(int column);
    virtual QVariant getValue(const QString & fieldName);
    virtual QVariant getNextRowValue(int column);
    virtual QVariant getNextRowValue(const QString & fieldName);
    virtual QVariant getPreviousRowValue(int column);
    virtual QVariant getPreviousRowValue(const QString & fieldName);
    virtual QString getFieldName(int column );
    virtual QVariant::Type getFieldType(int column);

    QString	getFileName() const;
    void setFileName(const QString &str);
    QString getDelimeter () const;
    void setDelimeter (const QString &str);
    bool getFirstRowIsHeader();
    void setFirstRowIsHeader(bool getValue);
    bool getFixFileIssues();
    void setFixFileIssues(bool value);

    virtual QString moduleShortName() const;
    virtual QString suitName() const { return "Standard"; }

    virtual QSet<QString> variables() const;

signals:
    void fileNameChanged(QString);
    void delimiterChanged(QString);
    void keepDataInternalChanged(bool);
    void firstRowIsHeaderChanged(bool);
    void fixFileIssuesChanged(bool);

private:
    explicit CsvDataset(const CsvDataset &dd, QObject * parent);
    virtual DatasetInterface * objectClone() const;
    inline void populateIfNotPopulated() {if (!m_isPopulated) populate();}
    QByteArray fixIssues(const QByteArray & ba);

    bool m_firstRowIsHeader;
    int m_currentRow;
    bool m_isPopulated;
    QString m_fileName;
    QString m_delimeter;
    bool m_fixFileIssues;
    Model * m_model;
    QSortFilterProxyModel * m_fmodel;
    QPointer<CuteReport::DatasetHelperInterface> m_helper;
    QHash<QString, int> m_fields;

    QString m_lastError;
};

#endif
