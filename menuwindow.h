#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "updatemenu.h"
#include <QWidget>
#include<addmenuwindow.h>
#include<removemenuwindow.h>
#include<showmenuwindow.h>

namespace Ui {
class menuwindow;
}

class menuwindow : public QWidget
{
    Q_OBJECT

public:
    explicit menuwindow(QWidget *parent = nullptr);
    ~menuwindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_Back_clicked();

    void on_updateButton_clicked();

private:
    Ui::menuwindow *ui;
    addmenuwindow*AddMenuWindow;
    removemenuwindow*RemoveMenuWindow;
    showmenuwindow*ShowMenuWindow;
    updatemenu*UpdateMenu;
};

#endif // MENUWINDOW_H
