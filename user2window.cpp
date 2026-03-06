#include "user2window.h"
#include "ui_user2window.h"
#include "takeawaywindow.h"
#include "dinninwindow.h"
#include "mainwindow.h"
#include "ordertrakingwindow.h"

User2Window::User2Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::User2Window)
{
    ui->setupUi(this);

    //for user2 maximized screen
    this->showFullScreen();
    setWindowTitle("Order");

}

User2Window::~User2Window()
{
    delete ui;
}

void User2Window::on_TakeawayButton_clicked()
{
    TakeAwayWindow=new takeawaywindow();
    TakeAwayWindow->show();
    this->hide();
}


void User2Window::on_DinninButton_clicked()
{
    DinnInWindow=new dinninwindow();
    DinnInWindow->show();
    this->hide();

}


void User2Window::on_LogOut_clicked()
{
    this->close();
    // Open the MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}


void User2Window::on_pushButton_clicked()
{
    OrderTrakingWindow=new ordertrakingwindow();
    OrderTrakingWindow->show();
    this->hide();
}

