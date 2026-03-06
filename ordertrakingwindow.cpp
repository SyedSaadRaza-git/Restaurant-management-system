#include "ordertrakingwindow.h"
#include "ui_ordertrakingwindow.h"
#include "databaseconnection.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include "user2window.h"

ordertrakingwindow::ordertrakingwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ordertrakingwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Order Tracking");
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db = QSqlDatabase::database("restaurant_connection");

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Database not connected");
        return;
    }


    populateTableWidget();
    populateOrderNumbers();
}

ordertrakingwindow::~ordertrakingwindow()
{
    delete ui;
}

void ordertrakingwindow::populateTableWidget()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QSqlQuery query(db);

    query.prepare(
        "SELECT order_no, total_price, status "
        "FROM orders "
        "WHERE DATE(order_date) = CURDATE() "
        "AND (status != 'Ready' OR TIMESTAMPDIFF(MINUTE, status_time, NOW()) < 5)"
        );

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", "Failed to fetch orders: " + query.lastError().text());
        return;
    }

    QStringList headers = {"Order No", "Total Price", "Status"};
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;

    while (query.next()) {

        ui->tableWidget->insertRow(row);

        QString orderNo = query.value("order_no").toString();
        QString totalPrice = query.value("total_price").toString();
        QString status = query.value("status").toString();

        ui->tableWidget->setItem(row,0,new QTableWidgetItem(orderNo));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(totalPrice));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(status));

        row++;
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}

void ordertrakingwindow::populateOrderNumbers()
{
    ui->comboBox_orderNo->clear();

    QSqlQuery query(db);

    query.prepare(
        "SELECT order_no FROM orders "
        "WHERE DATE(order_date) = CURDATE()"
        );

    if(query.exec())
    {
        while(query.next())
        {
            QString orderNo = query.value("order_no").toString();
            ui->comboBox_orderNo->addItem(orderNo);
        }
    }
    else
    {
        QMessageBox::critical(this,"Error","Failed to fetch order numbers: "+query.lastError().text());
    }
}


void ordertrakingwindow::on_pushButton_clicked()
{
    // Get the selected order number from the combo box
    QString selectedOrderNo = ui->comboBox_orderNo->currentText();

    // Validate if an order number is selected
    if (selectedOrderNo.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select an order number.");
        return;
    }

    // Clear the table widget before displaying new data
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // Fetch data from the orders table
    QSqlQuery query(db);
    query.prepare("SELECT order_no, total_price, status FROM orders WHERE order_no = :order_no");
    query.bindValue(":order_no", selectedOrderNo);

    if (query.exec()) {
        if (query.next()) {
            QString orderNo = query.value("order_no").toString();
            QString totalPrice = query.value("total_price").toString();
            QString status = query.value("status").toString();

            // Set the number of rows and columns in the table widget
            ui->tableWidget->setRowCount(1);
            ui->tableWidget->setColumnCount(3);

            // Set the table headers
            ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Order No" << "Total Price" << "Status");

            // Populate the table widget with the fetched data
            ui->tableWidget->setItem(0, 0, new QTableWidgetItem(orderNo));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(totalPrice));
            ui->tableWidget->setItem(0, 2, new QTableWidgetItem(status));
        } else {
            QMessageBox::information(this, "No Data", "No details found for the selected order number.");
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to fetch order details: " + query.lastError().text());
    }
}


void ordertrakingwindow::on_pushButton_2_clicked()
{
    this->close();
    User2Window*user2window= new User2Window();
    user2window->show();
}

