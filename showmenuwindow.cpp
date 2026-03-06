#include "showmenuwindow.h"
#include "ui_showmenuwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include "databaseconnection.h"
#include <QTableWidgetItem>
#include <QVector>
#include <QMap>
#include <algorithm>
#include "menuwindow.h"
#include "databaseconnection.h"

showmenuwindow::showmenuwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::showmenuwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Show menu");
    ui->MenuDetailsTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);
    ui->MenuDetailsTable->verticalHeader()->setVisible(false);
    ui->MenuDetailsTable->setAlternatingRowColors(true);
    ui->MenuDetailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Database not connected");
        return;
    }

}

showmenuwindow::~showmenuwindow()
{
    delete ui;
}

void showmenuwindow::on_pushButton_clicked()
{
    // Create a database connection
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
        return;
    }

    QString menuId = ui->lineEdit->text().trimmed();

    // Validate input
    if (menuId.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid Menu ID.");
        return;
    }

    // Prepare the SQL query
    QSqlQuery query(db);
    query.prepare("SELECT Name, Category, Cost_Price, selling_price FROM Menu WHERE id = :id");
    query.bindValue(":id", menuId);

    // Execute the query
    if (query.exec()) {
        if (query.next()) {
            // Fetch the data
            QString name = query.value("Name").toString();
            QString category = query.value("Category").toString();
            QString costPrice = query.value("Cost_Price").toString();
            QString price = query.value("selling_price").toString();

            // Clear the table widget before adding new data
            ui->MenuDetailsTable->clear();
            ui->MenuDetailsTable->setRowCount(1);  // Set to 1 row for the single record
            ui->MenuDetailsTable->setColumnCount(4);  // 4 columns for the details

            // Set column headers
            QStringList headers = {"Name", "Category", "Cost Price", "Price"};
            ui->MenuDetailsTable->setHorizontalHeaderLabels(headers);

            // Populate the table with data
            ui->MenuDetailsTable->setItem(0, 0, new QTableWidgetItem(name));
            ui->MenuDetailsTable->setItem(0, 1, new QTableWidgetItem(category));
            ui->MenuDetailsTable->setItem(0, 2, new QTableWidgetItem(costPrice));
            ui->MenuDetailsTable->setItem(0, 3, new QTableWidgetItem(price));

            QMessageBox::information(this, "Success", "Menu item found!");
        } else {
            // No record found
            QMessageBox::warning(this, "Not Found", "No menu item found with the given ID.");

            // Clear the table widget
            ui->MenuDetailsTable->clear();
            ui->MenuDetailsTable->setRowCount(0);
        }
    } else {
        // Handle query execution errors
        QMessageBox::critical(this, "Database Error", "Failed to search menu item: " + query.lastError().text());
    }
}



//-------------------------------------------------------------Bubble Sort for hiht-low price---------------------------------------------------------//
// Bubble Sort function to sort menu data based on price (high to low)
void bubbleSortByPrice(QVector<QMap<QString, QString>> &menuData) {
    int n = menuData.size();
    bool swapped;

    // Bubble Sort: Repeatedly swap adjacent elements if they're in the wrong order
    for (int i = 0; i < n - 1; ++i) {
        swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            double priceA = menuData[j]["Price"].toDouble();
            double priceB = menuData[j + 1]["Price"].toDouble();

            // Swap if the prices are in the wrong order (high to low)
            if (priceA < priceB) {
                std::swap(menuData[j], menuData[j + 1]);
                swapped = true;
            }
        }
        // If no two elements were swapped in the inner loop, the array is sorted
        if (!swapped)
            break;
    }
}


void showmenuwindow::on_pushButton_2_clicked()
{
    // Create a database connection
    QSqlDatabase db=QSqlDatabase::database("restaurant_connection");
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
        return;
    }

    // Create the query to fetch all menu details
    QSqlQuery query(db);
    query.prepare("SELECT id, Name,Category, Cost_Price, selling_price FROM menu");

    // Execute the query
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to retrieve menu details: " + query.lastError().text());
        return;
    }

    // Store menu data in a QVector
    QVector<QMap<QString, QString>> menuData;
    while (query.next()) {
        QMap<QString, QString> menuItem;
        menuItem["id"] = query.value("id").toString();
        menuItem["Name"] = query.value("Name").toString();
        menuItem["Category"] = query.value("Category").toString();
        menuItem["CostPrice"] = query.value("Cost_Price").toString();
        menuItem["Price"] = query.value("selling_price").toString();
        menuData.append(menuItem);
    }

    // Sort menu data by Price (high to low) using Bubble Sort
    bubbleSortByPrice(menuData);

    // Set up the table to display the data
    ui->MenuDetailsTable->setRowCount(menuData.size());  // Set row count based on data size
    ui->MenuDetailsTable->setColumnCount(5);             // Set column count for the table
    ui->MenuDetailsTable->setHorizontalHeaderLabels({"ID", "Item Name", "Category", "Cost Price", "Price"});

    // Insert the sorted data into the table widget
    for (int i = 0; i < menuData.size(); ++i) {
        const QMap<QString, QString> &menuItem = menuData[i];
        ui->MenuDetailsTable->setItem(i, 0, new QTableWidgetItem(menuItem["id"]));
        ui->MenuDetailsTable->setItem(i, 1, new QTableWidgetItem(menuItem["Name"]));
        ui->MenuDetailsTable->setItem(i, 2, new QTableWidgetItem(menuItem["Category"]));
        ui->MenuDetailsTable->setItem(i, 3, new QTableWidgetItem(menuItem["CostPrice"]));
        ui->MenuDetailsTable->setItem(i, 4, new QTableWidgetItem(menuItem["Price"]));
    }

    //To count all the dishes
    query.exec("SELECT COUNT(*) FROM menu");

    if(query.next()) {
        int total = query.value(0).toInt();
        ui->totalMenuLabel->setText("Total Dishes: " + QString::number(total));
    }


}


void showmenuwindow::on_Back_clicked()
{
    this->close();
    menuwindow*MenuWindow = new menuwindow();
    MenuWindow->show();
}

