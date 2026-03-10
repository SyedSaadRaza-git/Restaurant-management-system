#include "updatestaffwindow.h"
#include "ui_updatestaffwindow.h"
#include "staffwindow.h"
#include "databaseconnection.h"
#include <QMessageBox>

updatestaffwindow::updatestaffwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::updatestaffwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
}

updatestaffwindow::~updatestaffwindow()
{
    delete ui;
}

void updatestaffwindow::on_cancelButton_clicked()
{
    this->close();
    staffwindow*StaffWindow = new staffwindow();
    StaffWindow->show();

}


void updatestaffwindow::on_updateButton_clicked()
{
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Error", "Database not connected");
        return;
    }
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
    QString id = ui->lineEdit->text();
    QString position = ui->lineEdit_2->text();
    QString salary = ui->lineEdit_3->text();

    if(id.isEmpty() || position.isEmpty() || salary.isEmpty()){
        QMessageBox::warning(this,"Error","Fill all fields");
        return;
    }

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");
    QSqlQuery query(db);
    query.prepare("UPDATE staff SET position = ?, salary = ? WHERE id = ?");
    query.addBindValue(position);
    query.addBindValue(salary);
    query.addBindValue(id);

    if(query.exec() && query.numRowsAffected()>0){
        QMessageBox::information(this,"Success","Staff updated successfully");
    }else{
        QMessageBox::warning(this,"Failed","Staff ID not found");
    }
}


void updatestaffwindow::on_backButton_clicked()
{
    this->close();
    staffwindow*StaffWindow = new staffwindow();
    StaffWindow->show();
}

