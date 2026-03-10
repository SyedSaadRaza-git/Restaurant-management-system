#include "updatemenu.h"
#include "databaseconnection.h"
#include "qsqlquery.h"
#include "ui_updatemenu.h"
#include "menuwindow.h"
#include <QMessageBox>
#include "QSqlDatabase"

updatemenu::updatemenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::updatemenu)
{
    ui->setupUi(this);
    this->showFullScreen();
}

updatemenu::~updatemenu()
{
    delete ui;
}

void updatemenu::on_backButton_clicked()
{
    this->close();
    menuwindow*MenuWindow = new menuwindow();
    MenuWindow->show();
}


void updatemenu::on_cancelButton_clicked()
{
    this->close();
    menuwindow*MenuWindow = new menuwindow();
    MenuWindow->show();
}


void updatemenu::on_updateButton_clicked()
{
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Error", "Database not connected");
        return;
    }
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
    QString id = ui->lineEdit->text();
    QString cost = ui->lineEdit_2->text();
    QString sell = ui->lineEdit_3->text();

    if(id.isEmpty() || cost.isEmpty() || sell.isEmpty()){
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
    query.prepare("UPDATE Menu SET Cost_Price = ?, selling_Price = ? WHERE id = ?");
    query.addBindValue(cost);
    query.addBindValue(sell);
    query.addBindValue(id);

    if(query.exec() && query.numRowsAffected()>0){
        QMessageBox::information(this,"Success","Menu updated successfully");
    }else{
        QMessageBox::warning(this,"Failed","Menu ID not found");
    }
}

