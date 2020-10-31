/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2014 by Ivan Volkov                                     *
 *   wulff007@gmail.com                                                    *
 *   Copyright (C) 2015 by Alexander Mikhalov                              *
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
#include "modeldataset.h"
#include "modeldatasethelper.h"
#include "models/testmodel.h"
#include "models/clonemodel.h"

#include <QIcon>

ModelDataset::ModelDataset(QObject *parent) :
    CuteReport::DatasetInterface(parent),
    m_helper(0),
    m_sourceModel(0),
    m_testModel(new TestModel(this)),    
    m_currentRow(0),
    m_isPopulated(false),
    m_isInited(false),
    m_externalModelAddress(0)
{    
}

ModelDataset::ModelDataset(const ModelDataset &dd, QObject *parent) :
    CuteReport::DatasetInterface(parent),
    m_helper(0),
    m_sourceModel(0),
    m_testModel(new TestModel(this)),
    m_currentRow(dd.m_currentRow),
    m_isPopulated(false),
    m_isInited(false),
    m_externalModelAddress(dd.m_externalModelAddress),
    m_addressVariableName(dd.m_addressVariableName),
    m_testModelData(dd.m_testModelData)
{
}


ModelDataset::~ModelDataset()
{
}

void ModelDataset::init()
{
    DatasetInterface::init();
    m_isInited = true;
}

void ModelDataset::setAddressVariable(QString name)
{
    if (name == m_addressVariableName)
        return;

    m_addressVariableName = name.trimmed();

    emit changed();
    emit addressVariableChanged(m_addressVariableName);
    emit variablesChanged();
}

QString ModelDataset::addressVariable() const
{
    return m_addressVariableName;
}

void ModelDataset::setModelAddress(quint64 address)
{
    if (!m_isInited)    // do not restore address from serializer, it is not actual
        return;

    m_externalModelAddress = address;

    emit changed();
    emit modelAddressChanged(m_externalModelAddress);
}

quint64 ModelDataset::modelAddress() const
{
    return m_externalModelAddress;
}

QByteArray ModelDataset::testModelData() const
{
    return m_testModelData;
}

void ModelDataset::setTestModelData(QByteArray data)
{
    if (data == m_testModelData)
        return;

    m_testModelData = data;

    emit changed();
    emit testModelDataChanged(m_testModelData);
}

QAbstractItemModel *ModelDataset::model()
{
    return !m_sourceModel ? m_testModel : m_sourceModel;
}


TestModel *ModelDataset::testModel() const
{
    return m_testModel;
}

QSet<QString> ModelDataset::variables() const
{
    QSet<QString> vars;
    if (!m_addressVariableName.isEmpty())
        vars << m_addressVariableName;
    return vars;
}

CuteReport::DatasetInterface *ModelDataset::objectClone() const
{
    return new ModelDataset(*this, parent());
}

QString ModelDataset::moduleShortName() const
{
    return QString("Model");
}

CuteReport::DatasetHelperInterface *ModelDataset::helper()
{
    if (!m_helper) {
        m_helper = new ModelDatasetHelper(this);
    }

    return m_helper;
}

QIcon ModelDataset::icon()
{
    return QIcon(":/images/model.png");
}

bool ModelDataset::setFirstRow()
{
    emit(beforeFirst());
    m_currentRow = 0;
    bool ret = getRowCount();
    emit(afterFirst());
    return ret;
}

bool ModelDataset::setLastRow()
{
    emit(beforeLast());
    m_currentRow = model()->rowCount();
    bool ret = m_currentRow < model()->rowCount() ? true:false;
    emit(afterLast());
    return ret;
}

bool ModelDataset::setNextRow()
{
    emit(beforeNext());
    m_currentRow++;
    bool ret = m_currentRow < getRowCount();
    emit(afterNext());
    return ret;
}

bool ModelDataset::setPreviousRow()
{
    emit(beforePrevious());
    m_currentRow--;
    bool ret = m_currentRow >= 0;
    emit(afterPrevious());
    return ret;
}

bool ModelDataset::populate()
{
    if (!m_isInited)
        return false;

    emit beforePopulate();

    CuteReport::ReportInterface * report = dynamic_cast<CuteReport::ReportInterface*>(parent());

    QAbstractItemModel *model = 0;
    delete m_sourceModel;
    m_sourceModel = 0;

    if (!m_addressVariableName.isEmpty() && report->variables().contains(addressVariable())) {
        QVariant var = report->variables().value(addressVariable());
        if (var.type() == QVariant::ULongLong)
            model = (QAbstractItemModel*)(var.toULongLong());
    } else if (m_externalModelAddress != 0) {
        model = (QAbstractItemModel*)(m_externalModelAddress);
    }

    if (model) {
        CloneModel * cloneModel = new CloneModel(this);
        cloneModel->populate(model);

        m_sourceModel = cloneModel;
    } else {
        // TODO: use CuteReport log instead
        qWarning() << "Variable '" << addressVariable() << "contains, type long, but can't cast to QAbstractItemModel";
    }

    if (!m_sourceModel) {
        m_testModel->load(testModelData());
        m_currentRow = m_testModel->rowCount() > 0 ? 0 : -1;
    } else
        m_currentRow = m_sourceModel->rowCount() > 0 ? 0 : -1;

    m_isPopulated = true;

    emit afterPopulate();
    return true;
}

bool ModelDataset::isPopulated()
{
    return m_isPopulated;
}

void ModelDataset::setPopulated(bool b)
{
    m_isPopulated = b;
}

void ModelDataset::reset()
{
    m_isPopulated = false;
    m_testModel->clear();
    m_currentRow = -1;
}

void ModelDataset::resetCursor()
{
    m_currentRow = -1;
}

int ModelDataset::getCurrentRowNumber()
{
    return m_currentRow;
}

bool ModelDataset::setCurrentRowNumber(int index)
{
    emit(beforeSeek(index));
    m_currentRow = index;
    bool ret = (m_currentRow >=0 && m_currentRow < model()->rowCount() ? true:false);
    emit(afterSeek(index));
    return ret;
}

int ModelDataset::getRowCount()
{
    if (!m_isPopulated) populate();
    return model()->rowCount();
}

CuteReport::DatasetInterface *ModelDataset::createInstance(QObject *parent) const
{
    return new ModelDataset(parent);
}


int ModelDataset::columnIndexByName(QString name)
{
    if (name.isEmpty()) {
        qWarning() << "column name is empty";
        return -1;
    }

    for (int i = 0; i < model()->columnCount(); i++) {
//        qDebug() << sourceModel()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        if (model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() == name) {
            return i;
        }
    }

    int col = columnIndexIn(name, "field");

    if (col == -1) {
        col = columnIndexIn(name, "col");
    }

    if (col == -1) {
        qWarning() << name << "is not defined in source or test model";
    }
    return col;
}

int ModelDataset::columnIndexIn(const QString &name, const QString &templateName) const
{
    int col = -1;
    QString tmp = name;
    if (tmp.contains(templateName, Qt::CaseInsensitive)) {
        tmp.remove(templateName, Qt::CaseInsensitive);

        bool ok = false;
        col = tmp.toInt(&ok);

        if (!ok) {
            col = -1;
            qWarning() << name << "is incorrect field name";
        } else {
            return col - 1;
        }
    }

    return col;
}

QString ModelDataset::getLastError()
{
    return QString();
}

int ModelDataset::getColumnCount()
{
    if (!m_isPopulated) populate();
    return model()->columnCount();
}

QVariant ModelDataset::getValue(int index)
{
    if (!m_isPopulated) populate();
    if (m_currentRow == -1) {
        return QVariant(QVariant::Invalid);
    }

    if (index == -1 || index > model()->columnCount() - 1) {
        return QVariant(QVariant::Invalid);
    }

    return model()->index(m_currentRow, index).data(Qt::DisplayRole);
}

QVariant ModelDataset::getValue(const QString &field)
{
    int col = columnIndexByName(field);
    if (col == -1) {
        return QVariant::Invalid;
    }

    return getValue(col);
}

QVariant ModelDataset::getNextRowValue(int index)
{
    if (!m_isPopulated) {
        populate();
    }

    return m_currentRow <= model()->rowCount() && index < model()->columnCount()
            ? model()->index(m_currentRow + 1, index).data()
            : QVariant(QVariant::Invalid);
}

QVariant ModelDataset::getNextRowValue(const QString &field)
{
    return getNextRowValue(columnIndexByName(field));
}

QVariant ModelDataset::getPreviousRowValue(int index)
{
    if (!m_isPopulated)  {
        populate();
    }

    return m_currentRow-1 >= 0 && index < model()->columnCount()
            ? model()->index(m_currentRow, index).data(Qt::DisplayRole)
            : QVariant(QVariant::Invalid);
}

QVariant ModelDataset::getPreviousRowValue(const QString &field)
{
    return getPreviousRowValue(columnIndexByName(field));
}

QString ModelDataset::getFieldName(int column)
{
    Q_UNUSED(column)
    return QString();
}

QVariant::Type ModelDataset::getFieldType(int column)
{
    Q_UNUSED(column)
    return QVariant::String;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DatasetModel, ModelDataset)
#endif



