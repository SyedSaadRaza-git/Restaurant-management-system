#include "tablewindow.h"
#include "databaseconnection.h"
#include "ui_tablewindow.h"
#include "user1window.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>

#include "qsqlerror.h"
#include "qsqlquery.h"
#include<QMessageBox>

tablewindow::tablewindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::tablewindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    loadTables();
    ui->tableWidget_tables->verticalHeader()->setVisible(false);
    ui->tableWidget_tables->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_tables->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_tables->setSelectionMode(QAbstractItemView::NoSelection);

}

tablewindow::~tablewindow()
{
    delete ui;
}

void tablewindow::on_backButton_clicked()
{
    this->close();
    User1Window*user1window=new User1Window();
    user1window->show();
}


void tablewindow::on_addButton_clicked()
{
    QString tableNo = ui->lineEdit_tableNo->text().trimmed();

    if (tableNo.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter table number");
        return;
    }
    // Create a database connection
    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
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
    query.prepare("INSERT INTO restaurant_tables (table_no) VALUES (:tableNo)");
    query.bindValue(":tableNo", tableNo);

    if(query.exec()) {
        QMessageBox::information(this, "Success", "Table added!");
        ui->lineEdit_tableNo->clear();
        loadTables();   // refresh list
    }
    else {
        QMessageBox::critical(this, "Error", query.lastError().text());
    }
}


void tablewindow::on_removeButton_clicked()
{
    QString tableNo = ui->lineEdit_tableNo->text().trimmed();

    if (tableNo.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter table number");
        return;
    }
    // Create a database connection
    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
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
    query.prepare("DELETE FROM restaurant_tables WHERE table_no = :tableNo");
    query.bindValue(":tableNo", tableNo);

    if(query.exec() && query.numRowsAffected() > 0) {
        QMessageBox::information(this, "Removed", "Table removed!");
        ui->lineEdit_tableNo->clear();
        loadTables();
    }
    else {
        QMessageBox::warning(this, "Not Found", "Table not found");
    }
}

void tablewindow::loadTables()
{
    // Create a database connection
    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
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

    query.prepare("SELECT table_no FROM restaurant_tables ORDER BY table_no");

    if(!query.exec())
    {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }

    ui->tableWidget_tables->clearContents();
    ui->tableWidget_tables->setRowCount(0);
    ui->tableWidget_tables->setColumnCount(1);
    ui->tableWidget_tables->setHorizontalHeaderLabels(QStringList() << "Table Number");

    int row = 0;

    while(query.next())
    {
        ui->tableWidget_tables->insertRow(row);
        ui->tableWidget_tables->setItem(row, 0,
                                 new QTableWidgetItem(query.value(0).toString()));
        row++;
    }

    ui->tableWidget_tables->insertRow(row);
    ui->tableWidget_tables->setItem(row, 0,
                             new QTableWidgetItem("Total Tables: " + QString::number(row)));

}

