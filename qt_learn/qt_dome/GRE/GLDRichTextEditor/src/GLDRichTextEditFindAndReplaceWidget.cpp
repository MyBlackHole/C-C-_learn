#include "GLDRichTextEditFindAndReplaceWidget.h"
#include <QCloseEvent>

GLDRichTextEditFindAndReplaceWidget::GLDRichTextEditFindAndReplaceWidget(QWidget *parent)
    : QWidget(parent, Qt::WindowStaysOnTopHint | Qt::Window)
{
    ui.setupUi(this);

    connect(ui.btnFind, SIGNAL(clicked()), this, SIGNAL(finded()));
    connect(ui.btnReplace, SIGNAL(clicked()), this, SIGNAL(replaced()));
    connect(ui.btnReplaceAll, SIGNAL(clicked()), this, SIGNAL(allReplaced()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
}

GLDRichTextEditFindAndReplaceWidget::~GLDRichTextEditFindAndReplaceWidget()
{

}

void GLDRichTextEditFindAndReplaceWidget::setFind()
{
    ui.grpBox->show();
    ui.labReplace->hide();
    ui.edtReplaceText->hide();
    ui.btnReplace->hide();
    ui.btnReplaceAll->hide();
}

void GLDRichTextEditFindAndReplaceWidget::setReplace()
{
    ui.grpBox->hide();
    ui.labReplace->show();
    ui.edtReplaceText->show();
    ui.btnReplace->show();
    ui.btnReplaceAll->show();
}

QString GLDRichTextEditFindAndReplaceWidget::getFindText() const
{
    return ui.edtFindText->text().trimmed();
}

QString GLDRichTextEditFindAndReplaceWidget::getReplaceText() const
{
    return ui.edtReplaceText->text().trimmed();
}

bool GLDRichTextEditFindAndReplaceWidget::isWord() const
{
    return (Qt::Unchecked == ui.cbxWord->checkState() ? false : true);
}

bool GLDRichTextEditFindAndReplaceWidget::isCase() const
{
    return (Qt::Unchecked == ui.cbxCase->checkState() ? false : true);
}

bool GLDRichTextEditFindAndReplaceWidget::isNext() const
{
    return ui.rbtnDown->isChecked();
}

void GLDRichTextEditFindAndReplaceWidget::closeEvent(QCloseEvent *e)
{
    Q_ASSERT(NULL != e);
    e->ignore();
    hide();
}
