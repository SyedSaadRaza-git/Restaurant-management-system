#include "addstaffwindow.h"
#include "ui_addstaffwindow.h"
#include "staffwindow.h"
#include <QRegularExpression>

//headers for database connection
#include "qsqlerror.h"
#include "qsqlquery.h"
#include<QMessageBox>

AddStaffWindow::AddStaffWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddStaffWindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Add Staff Window");
}

AddStaffWindow::~AddStaffWindow()
{
    delete ui;
}

void AddStaffWindow::on_saveButton_clicked()
{
    // Get the already-open database connection
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");

    if (!db.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Database is not open.");
        return;
    }

    QString name = ui->StaffNamelineEdit->text().trimmed();
    QString cnic = ui->StaffCniclineEdit->text().trimmed();
    QString phoneNo = ui->StaffPhonelineEdit->text().trimmed();
    QString position = ui->StaffPositionlineEdit->text().trimmed();
    double salary = ui->StaffSalarylineEdit->text().toDouble();

    // Validate inputs
    if (name.isEmpty() || cnic.isEmpty() || phoneNo.isEmpty() ||
        position.isEmpty() || salary <= 0) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields correctly.");
        return;
    }

    // CNIC validation
    QRegularExpression cnicRegex(R"(\d{5}-\d{7}-\d{1})");
    if (!cnicRegex.match(cnic).hasMatch()) {
        QMessageBox::warning(this, "Input Error",
                             "Invalid CNIC format. Use 12345-1234567-1");
        return;
    }

    // Phone validation
    QRegularExpression phoneRegex(R"((\+92-\d{3}-\d{7})|(03\d{2}-\d{7}))");
    if (!phoneRegex.match(phoneNo).hasMatch()) {
        QMessageBox::warning(this, "Input Error",
                             "Invalid phone format.");
        return;
    }

    // Prepare query USING DATABASE
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO staff (name, cnic, phone_no, position, salary) "
        "VALUES (:name, :cnic, :phone, :position, :salary)"
        );

    query.bindValue(":name", name);
    query.bindValue(":cnic", cnic);
    query.bindValue(":phone", phoneNo);
    query.bindValue(":position", position);
    query.bindValue(":salary", salary);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error",
                              query.lastError().text());
    } else {
        QMessageBox::information(this, "Success",
                                 "Staff member added successfully!");

        ui->StaffNamelineEdit->clear();
        ui->StaffCniclineEdit->clear();
        ui->StaffPhonelineEdit->clear();
        ui->StaffPositionlineEdit->clear();
        ui->StaffSalarylineEdit->clear();
    }
}


void AddStaffWindow::on_Back_clicked()
{
    this->close();
    staffwindow*StaffWindow = new staffwindow();
    StaffWindow->show();
}


void AddStaffWindow::on_Back_2_clicked()
{
    this->close();
    staffwindow*StaffWindow = new staffwindow();
    StaffWindow->show();
}

