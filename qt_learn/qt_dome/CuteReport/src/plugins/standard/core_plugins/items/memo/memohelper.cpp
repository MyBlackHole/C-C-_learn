/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2014 by Alexander Mikhalov                         *
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
#include "memohelper.h"
#include "memo.h"
#include "ui_memohelper.h"
#include "designeriteminterface.h"
#include "reportcore.h"
#include "textformattoolbar.h"

#include <QTabWidget>
#include <QLayout>
#include <QTextEdit>
//#include "highlighter.h"

SUIT_BEGIN_NAMESPACE

MemoHelper::MemoHelper(MemoItem *item) :
    m_ui(new Ui::MemoHelper),
    m_item(item),
    m_currentToolWidget(0),
    m_edFormatedText(0),
    m_edSourceText(0),
    m_tabs(0)
//    m_highlighter(0)
{
    m_ui->setupUi(this);
    this->setWindowTitle( tr("Memo Helper") );

    m_edSourceText = new QTextEdit(this);
    m_edSourceText->setPlainText(m_item->text());


    if (m_item->allowHTML()) {
        m_edFormatedText = new QTextEdit(this);
        m_edFormatedText->setText(m_item->text());

        m_tabs = new QTabWidget(this);
        QWidget *formatedWdg = new QWidget(this);
        m_tabs->addTab(formatedWdg,tr("Formated"));
        m_tabs->addTab(m_edSourceText,tr("Source"));


        QVBoxLayout *lay = new QVBoxLayout(formatedWdg);
        lay->setMargin(4);
        lay->addWidget(m_edFormatedText);

        TextFormatToolBar *tb = new TextFormatToolBar(this);
        tb->setTextEdit(m_edFormatedText);
        tb->setStyleSheet("QToolBar { border: 0px }");
        lay->setMenuBar(tb);

        m_ui->pageTextLayout->addWidget(m_tabs);

        connect(m_tabs, SIGNAL(currentChanged(int)), SLOT(slotCurrentTextTabChange(int)));
    } else {
        m_ui->pageTextLayout->addWidget(m_edSourceText);
    }


    if  (m_item->reportCore()->designerInterface()) {
        m_ui->bExpression->setEnabled(m_item->reportCore()->designerInterface()->moduleExists("ExpressionEditor"));
        m_ui->bExpression->setIcon(m_item->reportCore()->designerInterface()->moduleIcon("ExpressionEditor"));
        if (!m_ui->bExpression->icon().isNull())
            m_ui->bExpression->setText(QString());
        m_ui->bExpression->setToolTip(m_item->reportCore()->designerInterface()->moduleToolTip("ExpressionEditor"));
        m_ui->bAggregate->setEnabled(m_item->reportCore()->designerInterface()->moduleExists("AggregateSelector"));
        m_ui->bFormatting->setEnabled(m_item->reportCore()->designerInterface()->moduleExists("FormattingSelector"));
        m_ui->bWordWrap->setEnabled(false);

        connect(m_ui->bExpression, SIGNAL(clicked()), this, SLOT(slotExpressionClicked()));
        connect(m_ui->bAggregate, SIGNAL(clicked()), this, SLOT(slotAggregateClicked()));
        connect(m_ui->bFormatting, SIGNAL(clicked()), this, SLOT(slotFormattingClicked()));
    } else {
        m_ui->bExpression->setEnabled(false);
        m_ui->bAggregate->setEnabled(false);
        m_ui->bFormatting->setEnabled(false);
        m_ui->bWordWrap->setEnabled(false);
    }

    setState(Text);
}


MemoHelper::~MemoHelper()
{
    delete m_ui;
}


void MemoHelper::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MemoHelper::sync()
{
    if (m_tabs && m_tabs->currentIndex() == 1) {
        m_edFormatedText->setText(m_edSourceText->toPlainText());
    }

    m_item->setText(m_item->allowHTML() ? m_edFormatedText->toHtml() : m_edSourceText->toPlainText());
}


bool MemoHelper::screenBack(bool accept)
{
    switch(m_currentState) {
        case Text: return false;
        case Expr: setState(Text);
            if (accept)
                if (!m_item->reportCore()->designerInterface())
                    return false;
                m_edFormatedText->insertPlainText(m_item->reportCore()->designerInterface()->getResult(m_currentToolWidget));
            return true;
    }
    return true;
}


void MemoHelper::slotExpressionClicked()
{
    setState(Expr);
}


void MemoHelper::slotAggregateClicked()
{

}


void MemoHelper::slotFormattingClicked()
{

}

void MemoHelper::slotCurrentTextTabChange(int index)
{
    if (index == 0) {
        m_edFormatedText->setText(m_edSourceText->toPlainText());
    } else if (index == 1) {
        m_edSourceText->setPlainText(m_edFormatedText->toHtml());
    }
}


void MemoHelper::setState(MemoHelper::State state)
{
    switch (state) {
        case Text: m_ui->stackedWidget->setCurrentIndex(0); break;
        case Expr: if (setToolWidget("Expression Editor")) m_ui->stackedWidget->setCurrentIndex(1); break;
    }
    m_currentState = state;
}


bool MemoHelper::setToolWidget(const QString &moduleName)
{
    delete m_currentToolWidget;
    if (!m_item->reportCore()->designerInterface())
        return false;

    m_currentToolWidget = m_item->reportCore()->designerInterface()->createWidget(moduleName);
    if (!m_currentToolWidget)
        return false;
    m_ui->toolLayout->addWidget(m_currentToolWidget);
    return true;
}

SUIT_END_NAMESPACE
