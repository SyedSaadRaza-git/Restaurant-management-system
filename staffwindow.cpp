#include "staffwindow.h"
#include "ui_staffwindow.h"
#include "addstaffwindow.h"
#include"removestaffwindow.h"
#include"showstaffwindow.h"
#include "user1window.h"
#include "updatestaffwindow.h"

staffwindow::staffwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::staffwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Staff");
}

staffwindow::~staffwindow()
{
    delete ui;
}

void staffwindow::on_addstaffButton_clicked()
{
    addstaffwindow=new AddStaffWindow();
    addstaffwindow->show();
    this->hide();
}


void staffwindow::on_removestaffButton_clicked()
{
    RemoveStaffWindow=new removestaffwindow();
    RemoveStaffWindow->show();
    this->hide();
}


void staffwindow::on_showstaffButton_clicked()
{
    ShowStaffWindow=new showstaffwindow();
    ShowStaffWindow->show();
    this->hide();
}


void staffwindow::on_Back_clicked()
{
    this->close();
    User1Window*user1window=new User1Window();
    user1window->show();
}


void staffwindow::on_updateButton_clicked()
{
    UpdateStaffWindow=new updatestaffwindow();
    UpdateStaffWindow->show();
    this->hide();
}

