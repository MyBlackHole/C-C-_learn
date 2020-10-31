#include "GLDRichTextEditTableValueTest.h"

GLDRichTextEditTableValueTest::GLDRichTextEditTableValueTest(QWidget *parent)
    : QWidget(parent, Qt::WindowStaysOnTopHint | Qt::Window)
    , m_type(0)
{
    ui.setupUi(this);

}

GLDRichTextEditTableValueTest::~GLDRichTextEditTableValueTest()
{

}

int GLDRichTextEditTableValueTest::row() const
{
    return ui.edtRow->text().trimmed().toInt();
}

int GLDRichTextEditTableValueTest::column() const
{
    return ui.edtColumn->text().trimmed().toInt();
}

QString GLDRichTextEditTableValueTest::getInitText() const
{
    return ui.edtInit->text();
}

QString GLDRichTextEditTableValueTest::getText() const
{
    return ui.edtValue->text();
}

void GLDRichTextEditTableValueTest::setText(const QString & text) const
{
    ui.edtValue->setText(text);
}

int GLDRichTextEditTableValueTest::getTable() const
{
    return ui.edtTable->text().trimmed().toInt();
}

void GLDRichTextEditTableValueTest::on_btnGet_clicked()
{
    m_type = 0;
    emit triggered();
}

void GLDRichTextEditTableValueTest::on_btnSet_clicked()
{
    m_type = 1;
    emit triggered();
}

void GLDRichTextEditTableValueTest::on_btnInt_clicked()
{
    m_type = 2;
    emit triggered();
}
