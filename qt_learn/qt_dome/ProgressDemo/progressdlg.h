#ifndef DIALOG_H
#define DIALOG_H

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QProgressBar>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void startProgress();

private:
    Ui::Dialog *ui;

    QLabel *FileNum;
    QLineEdit *FileNumLineEdit;
    QLabel *ProgressType;
    QComboBox *comboBox;
    QProgressBar *progressBar;
    QPushButton *starBtn;
    QGridLayout *mainLayout;

};

#endif // DIALOG_H
