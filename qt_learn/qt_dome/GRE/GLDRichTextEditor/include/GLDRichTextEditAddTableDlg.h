#ifndef GLDRICHTEXTEDITADDTABLEDLG_H
#define GLDRICHTEXTEDITADDTABLEDLG_H

#include <QDialog>
#include "ui_GLDRichTextEditAddTableDlg.h"

class GLDRichTextEditAddTableDlg : public QDialog
{
    Q_OBJECT

public:
    GLDRichTextEditAddTableDlg(QWidget *parent = 0);
    ~GLDRichTextEditAddTableDlg();

    int rows() const;
    int columns() const;


private:
    Ui::GLDRichTextEditAddTableDlg ui;
};

#endif // GLDRICHTEXTEDITADDTABLEDLG_H
