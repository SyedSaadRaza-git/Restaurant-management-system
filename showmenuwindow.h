#ifndef SHOWMENUWINDOW_H
#define SHOWMENUWINDOW_H

#include <QWidget>

namespace Ui {
class showmenuwindow;
}

class showmenuwindow : public QWidget
{
    Q_OBJECT

public:
    explicit showmenuwindow(QWidget *parent = nullptr);
    ~showmenuwindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_Back_clicked();

private:
    Ui::showmenuwindow *ui;
};

#endif // SHOWMENUWINDOW_H
