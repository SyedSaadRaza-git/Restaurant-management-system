#include "removemenuwindow.h"
#include "ui_removemenuwindow.h"
#include "menuwindow.h"
//headers for database connection
#include <QSqlQuery>
#include <QSqlError>
#include "databaseconnection.h"
#include<QMessageBox>



removemenuwindow::removemenuwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::removemenuwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Remove Menu");
}

removemenuwindow::~removemenuwindow()
{
    delete ui;
}

void removemenuwindow::on_removeButton_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
        return;
    }

    // Get the ID from the line edit
    QString menuId = ui->lineEdit->text().trimmed();

    // Validate the input
    if (menuId.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid Menu ID.");
        return;
    }

    // Prepare the SQL query
    QSqlQuery query(db);
    query.prepare("DELETE FROM Menu WHERE id = :id");
    query.bindValue(":id", menuId);

    // Execute the query and provide feedback
    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "Success", "Menu item removed successfully!");
            ui->lineEdit->clear(); // Clear the line edit after removal
        }
        else {
            QMessageBox::warning(this, "Not Found", "No menu item found with the given ID.");
        }
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to remove menu item: " + query.lastError().text());
    }
}



void removemenuwindow::on_Back_clicked()
{
    this->close();
    menuwindow*MenuWindow=new menuwindow();
    MenuWindow->show();
}

