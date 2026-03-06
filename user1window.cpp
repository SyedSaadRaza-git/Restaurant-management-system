#include "user1window.h"
#include "tablewindow.h"
#include "ui_user1window.h"
#include"staffwindow.h"
#include"menuwindow.h"
#include "mainwindow.h"

User1Window::User1Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::User1Window)

{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Admin");
}
User1Window::~User1Window()
{
    delete ui;
}

void User1Window::on_StaffButton_clicked()
{
    StaffWindow = new staffwindow();
    StaffWindow->show();
    this->hide();
}


void User1Window::on_MenuButton_clicked()
{
    MenuWindow=new menuwindow();
    MenuWindow->show();
    this->hide();
}


void User1Window::on_LogOut_clicked()
{
    this->close();
    // Open the MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}


void User1Window::on_TableButton_clicked()
{
    TableWindow=new tablewindow();
    TableWindow->show();
    this->hide();

}

