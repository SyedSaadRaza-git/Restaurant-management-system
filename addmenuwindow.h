#ifndef ADDMENUWINDOW_H
#define ADDMENUWINDOW_H

#include <QWidget>



namespace Ui {
class addmenuwindow;
}

class addmenuwindow : public QWidget
{
    Q_OBJECT

public:
    explicit addmenuwindow(QWidget *parent = nullptr);
    ~addmenuwindow();

private slots:
    void on_cancelmenuButton_clicked();

    void on_savemenuButton_clicked();

    void on_Back_clicked();

    void on_Back_2_clicked();

    void on_browseImageButton_clicked();

private:
    Ui::addmenuwindow *ui;
       QString selectedImagePath;
};

#endif // ADDMENUWINDOW_H
