#ifndef GLDRICHTEXTEDITFINDANDREPLACEWIDGET_H
#define GLDRICHTEXTEDITFINDANDREPLACEWIDGET_H

#include <QWidget>
#include "ui_GLDRichTextEditFindAndReplaceWidget.h"

class GLDRichTextEditFindAndReplaceWidget : public QWidget
{
    Q_OBJECT

public:
    GLDRichTextEditFindAndReplaceWidget(QWidget *parent = 0);
    ~GLDRichTextEditFindAndReplaceWidget();

    void setFind();
    void setReplace();

    QString getFindText() const;
    QString getReplaceText() const;
    bool isWord() const;
    bool isCase() const;
    bool isNext() const;

signals:
    void finded();
    void replaced();
    void allReplaced();

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    Ui::GLDRichTextEditFindAndReplaceWidget ui;
};

#endif // GLDRICHTEXTEDITFINDANDREPLACEWIDGET_H
