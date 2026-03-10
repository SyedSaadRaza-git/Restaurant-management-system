#include "showstaffwindow.h"
#include "ui_showstaffwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include "databaseconnection.h"
#include <QTableWidgetItem>
#include "staffwindow.h"

showstaffwindow::showstaffwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::showstaffwindow)
{
    ui->setupUi(this);
    this->showFullScreen();

    ui->staffDetailsTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);
    ui->staffDetailsTable->verticalHeader()->setVisible(false);
    ui->staffDetailsTable->setAlternatingRowColors(true);
    ui->staffDetailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

showstaffwindow::~showstaffwindow()
{
    delete ui;
}

void showstaffwindow::on_searchButton_clicked()
{
    QString staffId = ui->idLineEdit->text().trimmed();  // Get staff ID from the QLineEdit

    if (staffId.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid staff ID.");
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
    // Prepare the SQL query to fetch staff details based on ID
    QSqlQuery query(db);
    query.prepare("SELECT name, cnic, phone_no, position, salary FROM Staff WHERE id = :staffId");
    query.bindValue(":staffId", staffId);

    // Execute the query
    if (query.exec() && query.next()) {
        // Get the data from the query result
        QString name = query.value("name").toString();
        QString cnic = query.value("cnic").toString();
        QString phoneNo = query.value("phone_no").toString();
        QString position = query.value("Position").toString();
        QString salary = query.value("Salary").toString();

        // Set the data in the table
        ui->staffDetailsTable->setRowCount(1);  // Set one row to display data
        ui->staffDetailsTable->setColumnCount(5);  // 5 columns (Name, Cnic, PhoneNo, Position, Salary)
        ui->staffDetailsTable->setHorizontalHeaderLabels({"Name", "CNIC", "Phone No", "Position", "Salary"});

        // Populate table cells
        ui->staffDetailsTable->setItem(0, 0, new QTableWidgetItem(name));
        ui->staffDetailsTable->setItem(0, 1, new QTableWidgetItem(cnic));
        ui->staffDetailsTable->setItem(0, 2, new QTableWidgetItem(phoneNo));
        ui->staffDetailsTable->setItem(0, 3, new QTableWidgetItem(position));
        ui->staffDetailsTable->setItem(0, 4, new QTableWidgetItem(salary));
    } else {
        QMessageBox::critical(this, "Query Error", "Failed to fetch staff details: " + query.lastError().text());
    }
}



//-----------------------------------------------------------------------For Bubble Sorting------------------------------------------------------------
// Bubble Sort function to sort staff data based on position
void bubbleSort(QVector<QMap<QString, QString>> &staffData, const QVector<QString> &positionOrder) {
    int n = staffData.size();
    bool swapped;

    // Bubble Sort: Repeatedly swap adjacent elements if they're in the wrong order
    for (int i = 0; i < n - 1; ++i) {
        swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            int posA = positionOrder.indexOf(staffData[j]["Position"]);
            int posB = positionOrder.indexOf(staffData[j + 1]["Position"]);

            // Swap if the positions are in the wrong order
            if (posA > posB) {
                std::swap(staffData[j], staffData[j + 1]);
                swapped = true;
            }
        }
        // If no two elements were swapped in the inner loop, the array is sorted
        if (!swapped)
            break;
    }
}


void showstaffwindow::on_showAllButton_clicked()
{
    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");;
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
    query.prepare("SELECT id, name, cnic, phone_no, position, salary FROM Staff");

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to retrieve staff details: " + query.lastError().text());
        return;
    }

    QVector<QMap<QString, QString>> staffData;
    while (query.next()) {
        QMap<QString, QString> staffMember;
        staffMember["id"] = query.value("id").toString();
        staffMember["Name"] = query.value("Name").toString();
        staffMember["Cnic"] = query.value("Cnic").toString();
        staffMember["PhoneNo"] = query.value("phone_no").toString();
        staffMember["Position"] = query.value("Position").toString();
        staffMember["Salary"] = query.value("Salary").toString();
        staffData.append(staffMember);
    }

    QVector<QString> positionOrder = {"Manager", "Cook", "Waiter", "Cleaner"};
    bubbleSort(staffData, positionOrder);

    ui->staffDetailsTable->setRowCount(staffData.size());
    ui->staffDetailsTable->setColumnCount(6);
    ui->staffDetailsTable->setHorizontalHeaderLabels({"ID", "Name", "CNIC", "Phone No", "Position", "Salary"});

    for (int i = 0; i < staffData.size(); ++i) {
        const QMap<QString, QString> &staffMember = staffData[i];
        ui->staffDetailsTable->setItem(i, 0, new QTableWidgetItem(staffMember["id"]));
        ui->staffDetailsTable->setItem(i, 1, new QTableWidgetItem(staffMember["Name"]));
        ui->staffDetailsTable->setItem(i, 2, new QTableWidgetItem(staffMember["Cnic"]));
        ui->staffDetailsTable->setItem(i, 3, new QTableWidgetItem(staffMember["PhoneNo"]));
        ui->staffDetailsTable->setItem(i, 4, new QTableWidgetItem(staffMember["Position"]));
        ui->staffDetailsTable->setItem(i, 5, new QTableWidgetItem(staffMember["Salary"]));
    }
    query.exec("SELECT COUNT(*) FROM staff");

    if(query.next()) {
        int total = query.value(0).toInt();
        ui->totalStaffLabel->setText("Total Staff: " + QString::number(total));
    }

}


void showstaffwindow::on_Back_clicked()
{
    this->close();
    staffwindow*StaffWindow = new staffwindow();
    StaffWindow->show();
}

