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
#include "csvdataset.h"
#include "csvdatasethelper.h"
#include "csvdatasetmodel.h"
#include "reportcore.h"
#include "functions.h"

#include <QtCore>

#define DATASET_NAME "CSV"

using namespace CuteReport;

CsvDataset::CsvDataset(QObject *parent)
    : DatasetInterface(parent),
      m_firstRowIsHeader(false),
      m_currentRow(0),
      m_isPopulated(false),
      m_delimeter(","),
      m_fixFileIssues(false)
{
    m_model = new Model (this);
    m_fmodel = new QSortFilterProxyModel(this);
    m_fmodel->setSourceModel(m_model);
}


CsvDataset::CsvDataset(const CsvDataset &dd, QObject * parent)
    : DatasetInterface(parent),
      m_firstRowIsHeader(dd.m_firstRowIsHeader),
      m_currentRow(0),
      m_isPopulated(false),
      m_delimeter(dd.m_delimeter),
      m_fixFileIssues(dd.m_fixFileIssues)
{
    m_model = new Model (this);
    m_fmodel = new QSortFilterProxyModel(this);
    m_fmodel->setSourceModel(m_model);
    if (dd.m_isPopulated) {
        populate();
        setCurrentRowNumber(dd.m_currentRow);
    }
}


CsvDataset::~CsvDataset()
{
}


QIcon CsvDataset::icon()
{
    QIcon icon(":/images/csv.jpeg");
    return icon;
}


DatasetInterface * CsvDataset::createInstance(QObject* parent) const
{
    return new CsvDataset(parent);
}


DatasetHelperInterface * CsvDataset::helper()
{
    if (!m_helper)
        m_helper = new CsvDatasetHelper(this);

    return m_helper;
}


DatasetInterface *CsvDataset::objectClone() const
{
    return new CsvDataset(*this, parent());
}


QString CsvDataset::moduleShortName() const
{
    return QString("CSV");
}


QSet<QString> CsvDataset::variables() const
{
    QSet<QString> set;

    set.unite( findVariables(m_fileName) );

    return set;
}


QString CsvDataset::getLastError()
{
    return m_lastError;
}


QString	CsvDataset::getFileName() const
{
    return m_fileName;
}


void CsvDataset::setFileName(const QString &str)
{
    if (m_fileName == str)
        return;

    m_fileName = str;

    emit fileNameChanged(m_fileName);
    emit changed();
}


QString CsvDataset::getDelimeter () const
{
    return m_delimeter;
}


void CsvDataset::setDelimeter (const QString &str)
{
    if (m_delimeter == str)
        return;

    m_delimeter = str;

    emit delimiterChanged(m_delimeter);
    emit changed();
}


bool CsvDataset::getFirstRowIsHeader()
{
    return m_firstRowIsHeader;
}


void CsvDataset::setFirstRowIsHeader(bool value)
{
    if (m_firstRowIsHeader == value)
        return;

    m_firstRowIsHeader = value;

    emit firstRowIsHeaderChanged(m_firstRowIsHeader);
    emit changed();
}


bool CsvDataset::getFixFileIssues()
{
    return m_fixFileIssues;
}


void CsvDataset::setFixFileIssues(bool value)
{
    if (m_fixFileIssues == value)
        return;
    m_fixFileIssues = value;

    emit fixFileIssuesChanged(m_fixFileIssues);
    emit changed();
}


QByteArray CsvDataset::fixIssues(const QByteArray &ba)
{
    QString str(ba);
    str.replace(QRegExp("\r\n|\n|\r"),"\n");
    return str.toUtf8();
}


QString CsvDataset::getFieldName(int column )
{
    return m_model->headerData ( column, Qt::Horizontal).toString();
}


QVariant::Type CsvDataset::getFieldType(int column)
{
    Q_UNUSED(column);
    return QVariant::String;
}


QAbstractItemModel * CsvDataset::model()
{
    return m_fmodel;
}


bool CsvDataset::populate()
{
    emit beforePopulate();

    QStringList list;

    if (m_fileName.isEmpty()) {
        m_lastError = tr("Filename is empty. Please enter a filename first");
        return false;
    }

    CuteReport::ReportInterface * report = static_cast<CuteReport::ReportInterface*> (parent());

    QStringList missedVariables;
    if (!isStringValued(m_fileName, report->variables(), &missedVariables)) {
        m_lastError = QString("Variable is not defined in the \'File\' field: %1").arg(missedVariables.join(", "));
        return false;
    }

    QString fileURL = report ? setVariablesValue(m_fileName, report->variables()) : m_fileName;

    QByteArray fileData = reportCore()->loadObject(fileURL, report);
    if (m_fixFileIssues)
        fileData = fixIssues(fileData);

    QTextStream stream(fileData, QIODevice::Text| QIODevice::ReadOnly | QIODevice::Unbuffered);
    stream.setAutoDetectUnicode(true);

    QString line;
    do {
        line = stream.readLine();
        list << line;
    } while (!line.isNull());

    Array array;
    m_model->setHeader((m_firstRowIsHeader && !list.isEmpty()) ? list[0].split(m_delimeter) : QStringList());

    for (int i = (m_firstRowIsHeader ? 1:0); i<list.count(); i++) {
        array.append(list.at(i).split(m_delimeter));
    }
    m_model->setArray(array);

    m_isPopulated = true;
    m_currentRow = array.size() > 0 ? 0 : -1;

    emit afterPopulate();
    return true;
}


bool CsvDataset::isPopulated()
{
    return m_isPopulated;
}


void CsvDataset::setPopulated(bool b)
{
    m_isPopulated = b;
}


void CsvDataset::reset()
{
    m_isPopulated = false;
    m_model->clear();
    m_currentRow = -1;
    m_lastError = "";
}


void CsvDataset::resetCursor()
{
    m_currentRow = -1;
}


bool CsvDataset::setFirstRow()
{
    populateIfNotPopulated();
    emit(beforeFirst());
    m_currentRow = 0;
    bool ret = getRowCount();
    emit(afterFirst());
    return ret;
}


bool CsvDataset::setLastRow()
{
    populateIfNotPopulated();
    emit(beforeLast());
    m_currentRow = m_fmodel->rowCount();
    bool ret = m_currentRow < m_fmodel->rowCount() ? true:false;
    emit(afterLast());
    return ret;
}


bool CsvDataset::setNextRow()
{
    populateIfNotPopulated();
    emit(beforeNext());
    m_currentRow++;
    bool ret = m_currentRow < getRowCount();
    emit(afterNext());
    return ret;
}


bool CsvDataset::setPreviousRow()
{
    populateIfNotPopulated();
    emit(beforePrevious());
    m_currentRow--;
    bool ret = m_currentRow >= 0;
    emit(afterPrevious());
    return ret;
}


int CsvDataset::getCurrentRowNumber()
{
    return m_currentRow;
}


bool CsvDataset::setCurrentRowNumber(int index)
{
    populateIfNotPopulated();
    emit(beforeSeek(index));
    m_currentRow = index;
    bool ret = (m_currentRow >=0 && m_currentRow < m_fmodel->rowCount() ? true:false);
    emit(afterSeek(index));
    return ret;
}


int CsvDataset::getRowCount()
{
    populateIfNotPopulated();
    return m_fmodel->rowCount();
}


int CsvDataset::getColumnCount()
{
    populateIfNotPopulated();
    return m_fmodel->columnCount();
}


QVariant CsvDataset::getValue(int column)
{    populateIfNotPopulated();
     return m_fmodel->data( m_fmodel->index(m_currentRow,column) );
}


QVariant CsvDataset::getValue(const QString & fieldName)
{
    populateIfNotPopulated();
    return getValue(m_model->fieldIndex(fieldName));
}


QVariant CsvDataset::getNextRowValue(int column)
{
    populateIfNotPopulated();
    return m_model->data(m_model->index(m_currentRow+1, column));
}


QVariant CsvDataset::getNextRowValue(const QString & fieldName)
{
    populateIfNotPopulated();
    return getNextRowValue(m_model->fieldIndex(fieldName));
}


QVariant CsvDataset::getPreviousRowValue(int column)
{
    populateIfNotPopulated();
    return m_model->data(m_model->index(m_currentRow+1, column));
}


QVariant CsvDataset::getPreviousRowValue(const QString & fieldName)
{
    populateIfNotPopulated();
    return getPreviousRowValue(m_model->fieldIndex(fieldName));
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DatasetCSV, CsvDataset)
#endif
