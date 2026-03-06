#include "user4window.h"
#include "ui_user4window.h"
#include <mainwindow.h>
#include "databaseconnection.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QTimer>

user4window::user4window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::user4window)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Kitchen");

    //Function for refresh the page:
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        loadOrders();
        populateTableWidget();
    });
    timer->start(10000);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to database.");
        return;
    }

    // Get existing connection
    db = QSqlDatabase::database("restaurant_connection");

    // Load orders into the ComboBox
    loadOrders();
    populateTableWidget();


}

user4window::~user4window()
{
    delete ui;
}



void user4window::on_LogOut_clicked()
{
    this->close();
    // Open the MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}

void user4window::loadOrders(){
    QSqlQuery query(db);
    if (!query.exec(
            "SELECT order_no FROM orders "
            "WHERE DATE(order_date) = CURDATE() "
            "AND (status != 'Ready' OR TIMESTAMPDIFF(MINUTE, status_time, NOW()) < 5)"
            )) {
        QMessageBox::critical(nullptr, "Query Error",
                              "Failed to fetch order numbers: " + query.lastError().text());
        return;
    }

    ui->comboBox_orderNo->clear();

    while (query.next()) {
        QString orderNo = query.value(0).toString();
        ui->comboBox_orderNo->addItem(orderNo);
    }
}


void user4window::on_btn_ok_clicked()
{
    QString orderNo = ui->comboBox_orderNo->currentText();

    if (orderNo.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select an order.");
        return;
    }

    QString status;
    if (ui->radioButton_InProgress->isChecked()) {
        status = "In Progress";
    } else if (ui->radioButton_Ready->isChecked()) {
        status = "Ready";
    } else {
        QMessageBox::warning(this, "Error", "Please select a status.");
        return;
    }

    QSqlQuery query(db);
    if(status == "Ready")
    {
        query.prepare("UPDATE orders SET status=:status, status_time=NOW() WHERE order_no=:id");
    }
    else
    {
        query.prepare("UPDATE orders SET status=:status WHERE order_no=:id");
    }

    query.bindValue(":status", status);
    query.bindValue(":id", orderNo);

    query.bindValue(":id", orderNo);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Order status updated successfully!");

        loadOrders();  // Refresh the ComboBox
        populateTableWidget();  // Refresh the table widget
    } else {
        QMessageBox::critical(this, "Error", "Failed to update order status.");
    }
}



void user4window::populateTableWidget()
{
    // Clear the table widget
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // Define and execute the query
    QSqlQuery query(db);
    if (!query.exec(
            "SELECT order_no, total_price, status "
            "FROM orders "
            "WHERE DATE(order_date) = CURDATE() "
            "AND (status != 'Ready' OR TIMESTAMPDIFF(MINUTE, status_time, NOW()) < 5)"
            )) {
        QMessageBox::critical(this, "Query Error", "Failed to fetch orders: " + query.lastError().text());
        return;
    }

    // Set column headers
    QStringList headers = {"Order No", "Total Price", "Status"};
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Populate the table widget with query results
    int row = 0;
    while (query.next()) {
        // Insert a new row
        ui->tableWidget->insertRow(row);

        // Fetch data from the query
        QString orderNo = query.value("order_no").toString();
        QString totalPrice = query.value("total_price").toString();
        QString status = query.value("status").toString();

        // Populate the row
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(orderNo));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(totalPrice));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(status));

        // Move to the next row
        row++;
    }

    // Adjust table columns to fit content
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

}
