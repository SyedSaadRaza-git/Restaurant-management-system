#ifndef SHOWMENUWINDOW2_H
#define SHOWMENUWINDOW2_H

#include <QWidget>

namespace Ui {
class showmenuwindow2;
}

class showmenuwindow2 : public QWidget
{
    Q_OBJECT

public:
    explicit showmenuwindow2(QWidget *parent = nullptr);
    ~showmenuwindow2();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_Back_clicked();

private:
    Ui::showmenuwindow2 *ui;
};

#endif // SHOWMENUWINDOW2_H
