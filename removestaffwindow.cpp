#include "removestaffwindow.h"
#include "ui_removestaffwindow.h"
#include "QMessageBox"
#include "staffwindow.h"

//headers for database connection
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "databaseconnection.h"
#include<QMessageBox>



removestaffwindow::removestaffwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::removestaffwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Remove Staff");
}

removestaffwindow::~removestaffwindow()
{
    delete ui;
}

void removestaffwindow::on_RemoveButton_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
        return;
    }
    // Get the ID entered in the QLineEdit
    QString staffId = ui->idLineEdit->text();

    if (staffId.isEmpty()) {
        // If ID field is empty, show an error message
        QMessageBox::warning(this, "Input Error", "Please enter a valid staff ID.");
        return;
    }

    // Prepare the DELETE SQL query to remove the staff by ID

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");
    QSqlQuery query(db);
    query.prepare("DELETE FROM staff WHERE id = :staffId");
    query.bindValue(":staffId", staffId);  // Bind the staff ID from the input

    // Execute the query
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Staff member removed successfully.");
    } else {
        // Handle error if the query fails
        QMessageBox::critical(this, "Error", "Failed to remove staff member: " + query.lastError().text());
    }
}


void removestaffwindow::on_Back_clicked()
{
    this->hide();
    staffwindow*StaffWindow=new staffwindow();
    StaffWindow->show();

}

