#include "orderdetailwindow.h"
#include "ui_orderdetailwindow.h"
#include "user3window.h"
#include <QMessageBox>
#include "databaseconnection.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include <QVector>
#include <QPair>
#include <QSqlQuery>

orderdetailwindow::orderdetailwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::orderdetailwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    showTodayOrders();

    ui->tableWidget->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
orderdetailwindow::~orderdetailwindow()
{
    delete ui;
}

void orderdetailwindow::on_Back_clicked()
{
    this->close();
    User3Window*user3window=new User3Window();
    user3window->show();
}


void orderdetailwindow::on_showAllButton_clicked()
{
    int totalOrders = 0;

    // Connect to the database

   if (!connectToDatabase()) {
       QMessageBox::critical(this, "Error", "Database not connected");
       return;
   }
  QSqlDatabase db = QSqlDatabase::database("restaurant_connection");

   // Clear the table widget before displaying new data
   ui->tableWidget->clearContents();
   ui->tableWidget->setRowCount(0);

   // Set the number of columns and their headers
   ui->tableWidget->setColumnCount(5); // Columns for Order No, Total Price, Profit, Order Date, and Remove button
   ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                              << "Order No"
                                              << "Total Price"
                                              << "Profit"
                                              << "Order Date"
                                              << "Remove");

   //Function for reconnect database
   if(!reconnectDatabase())
   {
       QMessageBox::critical(this,"Database Error","Cannot connect to database.");
       return;
   }
   db = QSqlDatabase::database("restaurant_connection");
   // Prepare the SQL query to fetch data from `orders` and `Menu` tables
   QSqlQuery query(db);
   query.prepare(R"(
SELECT
    o.order_no,
    o.total_price,
    o.order_date,
    SUM((oi.unit_price - m.Cost_Price) * oi.quantity) AS profit
FROM orders o
JOIN order_items oi ON o.order_no = oi.order_no
JOIN Menu m ON oi.item_name = m.Name
GROUP BY o.order_no, o.total_price, o.order_date;
    )");

   if (!query.exec()) {
       QMessageBox::critical(this, "Query Error", "Failed to fetch data: " + query.lastError().text());
       return;
   }

   QVector<QPair<double, QVector<QString>>> data;
   double totalProfit = 0.0; // Variable to calculate total profit

   // Process the query results
   while (query.next()) {
       QVector<QString> row;

       QString orderNo = query.value("order_no").toString();
       double totalPrice = query.value("total_price").toDouble();
       QString orderDate = query.value("order_date").toString();
       double profit = query.value("profit").toDouble();

       totalOrders++;
       totalProfit += profit;

       row.append(orderNo);
       row.append(QString::number(totalPrice, 'f', 2));
       row.append(QString::number(profit, 'f', 2));
       row.append(orderDate);

       data.append(qMakePair(totalPrice, row));
   }
   ui->labelTotalOrders->setText("Total Orders: " + QString::number(totalOrders));
   ui->labelTotalProfit->setText("Total Profit: " + QString::number(totalProfit,'f',2));

   // Bubble Sort to sort the data in descending order of total price
   for (int i = 0; i < data.size() - 1; ++i) {
       for (int j = 0; j < data.size() - i - 1; ++j) {
           if (data[j].first < data[j + 1].first) {
               qSwap(data[j], data[j + 1]);
           }
       }
   }

   // Populate the tableWidget with sorted data
   for (int row = 0; row < data.size(); ++row) {
       ui->tableWidget->insertRow(row);

       QVector<QString> rowData = data[row].second;
       for (int col = 0; col < rowData.size(); ++col) {
           ui->tableWidget->setItem(row, col, new QTableWidgetItem(rowData[col]));
       }

       // Add a Remove Button in the last column
       QPushButton *removeButton = new QPushButton("Remove");
       connect(removeButton, &QPushButton::clicked, [=]() {
           removeOrder(rowData.at(0));  // Pass QString directly
       });
       ui->tableWidget->setCellWidget(row, 4, removeButton);
   }

   // Add a row for total profit at the end
   int totalRow = ui->tableWidget->rowCount();
   ui->tableWidget->insertRow(totalRow);

   // Merge cells for "Total Profit" label
   QTableWidgetItem *totalProfitLabel = new QTableWidgetItem("Total Profit");
   totalProfitLabel->setTextAlignment(Qt::AlignCenter);
   ui->tableWidget->setSpan(totalRow, 0, 1, 4);  // Span across first four columns
   ui->tableWidget->setItem(totalRow, 0, totalProfitLabel);

   // Display total profit in the last column
   QTableWidgetItem *totalProfitItem = new QTableWidgetItem(QString::number(totalProfit, 'f', 2));
   totalProfitItem->setTextAlignment(Qt::AlignCenter);
   ui->tableWidget->setItem(totalRow, 4, totalProfitItem);

   // Set the size of the first column (Order No) and last column (Total Profit)
   ui->tableWidget->setColumnWidth(0, 150);  // Order No
   ui->tableWidget->setColumnWidth(4, 200);  // Remove Button / Total Profit

   // Display a message with the number of rows loaded
   QMessageBox::information(this, "Data Loaded", QString::number(data.size()) + " orders loaded successfully.");

}




void orderdetailwindow::on_pushButton_clicked()
{
    int totalOrders = 0;

    // Establish database connection
   QSqlDatabase db = QSqlDatabase::database("restaurant_connection");
   if (!connectToDatabase()) {
       QMessageBox::critical(this, "Database Error", "Database not connected");
       return;
   }

    // Get and validate the date input
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd"); // Format date properly
    if (date.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid date.");
        return;
    }

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");
    // Declare and prepare the SQL query
    QSqlQuery query(db);
    query.prepare(R"(
    SELECT o.order_no,
           o.total_price,
           o.order_date,
           SUM((oi.unit_price - m.Cost_Price) * oi.quantity) AS profit
    FROM orders o
    JOIN order_items oi ON o.order_no = oi.order_no
    JOIN Menu m ON oi.item_name = m.Name
    WHERE DATE(o.order_date) = :date
    GROUP BY o.order_no, o.total_price, o.order_date
)");
    query.bindValue(":date", date);

    // Execute the query and handle the results
    if (query.exec()) {
        // Clear the table widget and set up columns
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(4); // Columns: Order No, Total Price, Profit, Order Date
        ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                                   << "Order No"
                                                   << "Total Price"
                                                   << "Profit"
                                                   << "Order Date");

        int row = 0;
        double totalProfit = 0.0; // Variable to store the total profit

        // Fetch and display results
        while (query.next()) {
            ui->tableWidget->insertRow(row);

            // Retrieve data from the query
            QString orderNo = query.value("order_no").toString();
            double totalPrice = query.value("total_price").toDouble();
            QString orderDate = query.value("order_date").toString();
            double profit = query.value("profit").toDouble();

            // Sum up the profit
            totalProfit += profit;

            //Increase the total order
            totalOrders++;

            // Display data in the table widget
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(orderNo));              // Order No
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(totalPrice, 'f', 2))); // Total Price
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(profit, 'f', 2)));     // Profit
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(orderDate));            // Order Date
            row++;
        }
        ui->labelTotalOrders->setText("Total Orders: " + QString::number(row));
        ui->labelTotalProfit->setText("Total Profit: " + QString::number(totalProfit,'f',2));

        if (row == 0) {
            QMessageBox::information(this, "No Results", "No orders found for the selected date.");
        } else {
            // Display total profit in the last row of the table
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem("Total Profit")); // Label for total profit
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(totalProfit, 'f', 2))); // Total Profit value
        }
    } else {
        // Handle query execution errors
        QMessageBox::critical(this, "Database Error", "Failed to execute query: " + query.lastError().text());
    }
}


void orderdetailwindow::removeOrder(const QString &orderId)
{
    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    // Prepare the query to delete the order based on the order_id
    QSqlQuery query;
    query.prepare("DELETE FROM orders WHERE order_no = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Deletion Error", "Failed to delete the order: " + query.lastError().text());
    } else {
        QMessageBox::information(this, "Order Removed", "Order " + orderId + " has been successfully removed.");
    }
}

//Function for show currentday Orders
void orderdetailwindow::showTodayOrders()
{
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Error", "Database not connected");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");

    QString today = QDate::currentDate().toString("yyyy-MM-dd");

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Order No"
                                               << "Total Price"
                                               << "Profit"
                                               << "Order Date");

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
db = QSqlDatabase::database("restaurant_connection");
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
            o.order_no,
            o.total_price,
            o.order_date,
            SUM((oi.unit_price - m.Cost_Price) * oi.quantity) AS profit
        FROM orders o
        JOIN order_items oi ON o.order_no = oi.order_no
        JOIN Menu m ON oi.item_name = m.Name
        WHERE DATE(o.order_date) = :date
        GROUP BY o.order_no, o.total_price, o.order_date
    )");

    query.bindValue(":date", today);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", query.lastError().text());
        return;
    }

    int row = 0;
    int totalOrders = 0;
    double totalProfit = 0;

    while (query.next()) {

        ui->tableWidget->insertRow(row);

        QString orderNo = query.value("order_no").toString();
        double totalPrice = query.value("total_price").toDouble();
        QString orderDate = query.value("order_date").toString();
        double profit = query.value("profit").toDouble();

        ui->tableWidget->setItem(row,0,new QTableWidgetItem(orderNo));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString::number(totalPrice,'f',2)));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString::number(profit,'f',2)));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(orderDate));

        totalOrders++;
        totalProfit += profit;

        row++;
    }

    // Update Labels
    ui->labelTotalOrders->setText("Total Orders: " + QString::number(totalOrders));
    ui->labelTotalProfit->setText("Total Profit: " + QString::number(totalProfit,'f',2));
}

