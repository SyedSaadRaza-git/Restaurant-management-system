#include "menuwindow.h"
#include "ui_menuwindow.h"
#include"addmenuwindow.h"
#include"removemenuwindow.h"
#include"showmenuwindow.h"
#include "user1window.h"
#include "updatemenu.h"

menuwindow::menuwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menuwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Menu");

}

menuwindow::~menuwindow()
{
    delete ui;
}

void menuwindow::on_pushButton_clicked()
{
    AddMenuWindow=new addmenuwindow();
    AddMenuWindow->show();
    this->hide();
}


void menuwindow::on_pushButton_2_clicked()
{
    RemoveMenuWindow=new removemenuwindow();
    RemoveMenuWindow->show();
    this->hide();
}


void menuwindow::on_pushButton_3_clicked()
{
    ShowMenuWindow=new showmenuwindow();
    ShowMenuWindow->show();
    this->hide();
}


void menuwindow::on_Back_clicked()
{
    this->close();
    User1Window*user1window = new User1Window();
    user1window->show();
}


void menuwindow::on_updateButton_clicked()
{
    UpdateMenu=new updatemenu();
    UpdateMenu->show();
    this->hide();
}

