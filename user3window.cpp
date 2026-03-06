#include "user3window.h"
#include "ui_user3window.h"
#include "showstaffwindow2.h"
#include "showmenuwindow2.h"
#include "mainwindow.h"

User3Window::User3Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::User3Window)
{
    ui->setupUi(this);

    //for user3 maximized screen
    this->showFullScreen();
    setWindowTitle("Staff Detail");
    }

User3Window::~User3Window()
{
    delete ui;
}

void User3Window::on_pushButton_clicked()
{
    ShowStaffWindow2=new showstaffwindow2();
    ShowStaffWindow2->show();
    this->hide();
}


void User3Window::on_pushButton_3_clicked()
{
    this->close();
    ShowMenuWindow2= new showmenuwindow2();
    ShowMenuWindow2->show();

}


void User3Window::on_pushButton_2_clicked()
{
    orderDetailWindowInstance=new orderdetailwindow();
    orderDetailWindowInstance->show();
    this->hide();
}


void User3Window::on_LogOut_clicked()
{
    this->close();
    // Open the MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}

