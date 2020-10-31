#ifndef GLDRICHTEXTEDITTABLEVALUETEST_H
#define GLDRICHTEXTEDITTABLEVALUETEST_H

#include <QWidget>
#include "ui_GLDRichTextEditTableValueTest.h"

class GLDRichTextEditTableValueTest : public QWidget
{
    Q_OBJECT

public:
    GLDRichTextEditTableValueTest(QWidget *parent = 0);
    ~GLDRichTextEditTableValueTest();

    int row() const;
    int column() const;

    QString getInitText() const;
    QString getText() const;
    void setText(const QString & text) const;

    int getTable() const;
    
    int getType() const{
        return m_type;
    }

signals:
    void triggered();

private slots:
void on_btnGet_clicked();
void on_btnSet_clicked();
void on_btnInt_clicked();


private:
    Ui::GLDRichTextEditTableValueTest ui;
    int m_type;
};

#endif // GLDRICHTEXTEDITTABLEVALUETEST_H
