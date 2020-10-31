#include "GLDRichTextEditAddTableDlg.h"

GLDRichTextEditAddTableDlg::GLDRichTextEditAddTableDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

GLDRichTextEditAddTableDlg::~GLDRichTextEditAddTableDlg()
{

}

int GLDRichTextEditAddTableDlg::rows() const
{
    return ui.edtRowNum->text().trimmed().toInt();
}

int GLDRichTextEditAddTableDlg::columns() const
{
    return ui.edtColumnNum->text().trimmed().toInt();
}
