#ifndef USER1WINDOW_H
#define USER1WINDOW_H

#include "tablewindow.h"
#include <QWidget>
#include<staffwindow.h>
#include<menuwindow.h>


namespace Ui {
class User1Window;
}

class User1Window : public QWidget
{
    Q_OBJECT

public:
    explicit User1Window(QWidget *parent = nullptr);
    ~User1Window();

private slots:
    void on_StaffButton_clicked();

    void on_MenuButton_clicked();

    void on_LogOut_clicked();

    void on_TableButton_clicked();

    void on_restoreButton_clicked();

private:
    Ui::User1Window *ui;
    staffwindow*StaffWindow;
    menuwindow*MenuWindow;
    tablewindow*TableWindow;
};
#endif// USER1WINDOW_H
