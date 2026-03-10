#include "dinninwindow.h"
#include "ui_dinninwindow.h"
#include "user2window.h"
#include "databaseconnection.h"
#include <QDate> // Include this header for working with dates
#include<QPushButton> //for dynamic buttons
#include <QVBoxLayout>
#include <QPdfWriter>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QFileDialog>
#include <QPageLayout>
#include <QPageSize>
#include <QTimer>
#include <QFile>

//headers for database connection

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

dinninwindow::dinninwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::dinninwindow)
{
    ui->setupUi(this);
    this->showFullScreen();

    //Code for timer
    orderCheckTimer = new QTimer(this);

    connect(orderCheckTimer, &QTimer::timeout,
            this, &dinninwindow::checkForReadyOrders);

    orderCheckTimer->start(5000); // check every 5 seconds

    ui->tableWidget->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db = QSqlDatabase::database("restaurant_connection");

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Database not connected");
        return;
    }


    connectCategoryButtons();
    populateTablesComboBox();
    populateCategoryButtons();

    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Item Name" << "Quantity" << "Price"<<"Total Price"<<"Remove");
    ui->tableWidget->setColumnWidth(0, 200);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 100);
    ui->tableWidget->setColumnWidth(3, 100);
    ui->tableWidget->setColumnWidth(4, 100);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->comboBox, &QComboBox::currentTextChanged,
            this, &dinninwindow::onTableSelected);
    connect(ui->GetBillButton, &QPushButton::clicked, this, &dinninwindow::BillGenerated);

}

dinninwindow::~dinninwindow()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void dinninwindow::connectCategoryButtons()
{
    // Disconnect previous signals to avoid multiple connections
    disconnect(ui->listWidget, &QListWidget::itemClicked, this, &dinninwindow::addItemToOrder);
}

void dinninwindow::calculateTotalPrice()
{
    double totalPrice = 0.0;

    // Loop through each row in the tableWidget
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        // Get the price from the Price column (assuming it's the 3nd column, index 3)
        QTableWidgetItem *priceItem = ui->tableWidget->item(row, 3);
        if (priceItem)
        {
            totalPrice += priceItem->text().toDouble();  // Add the price to the total
        }
    }

    // Display the total price in a QLabel or other widget
    ui->totalPriceLabel->setText("Total Amount: " + QString::number(totalPrice, 'f', 3));
}

void dinninwindow::loadCategoryMenu(const QString &category)
{
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }

    db = QSqlDatabase::database("restaurant_connection");

    QSqlQuery query(db);
    query.prepare("SELECT Name, selling_price, image_path FROM menu WHERE category = :category");
    query.bindValue(":category", category);

    if(!query.exec())
    {
        QMessageBox::critical(this,"Query Error",query.lastError().text());
        return;
    }

    ui->listWidget->clear();

    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(100,100));
    ui->listWidget->setGridSize(QSize(150,150));
    ui->listWidget->setSpacing(10);

    while(query.next())
    {
        QString itemName = query.value(0).toString();
        double price = query.value(1).toDouble();
        QString imagePath = query.value(2).toString();

        // Clean the path
        imagePath = imagePath.trimmed();
        imagePath.replace("\\","/");

        qDebug() << "Checking image:" << imagePath;

        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setText(itemName);
        listItem->setData(Qt::UserRole, price);

        if(QFile::exists(imagePath))
        {
            listItem->setIcon(QIcon(imagePath));
            qDebug() << "Image loaded successfully";
        }
        else
        {
            qDebug() << "Image NOT found, using default";
            listItem->setIcon(QIcon(":/resources/default.png"));
        }

        ui->listWidget->addItem(listItem);
    }

    disconnect(ui->listWidget, &QListWidget::itemClicked,
               this, &dinninwindow::addItemToOrder);

    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &dinninwindow::addItemToOrder);
}
void dinninwindow::addItemToOrder(QListWidgetItem *item)
{
    // Get item name and price from the QListWidgetItem
    QString itemName = item->text();
    double price = item->data(Qt::UserRole).toDouble();

    // Flag to check if the item already exists in the table
    bool itemExists = false;

    // Loop through the rows to check if the item is already in the table
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QTableWidgetItem *existingItem = ui->tableWidget->item(row, 0);  // Item name column
        if (existingItem && existingItem->text() == itemName)
        {
            // If the item already exists in the table, increase quantity and update the total price
            QTableWidgetItem *quantityItem = ui->tableWidget->item(row, 1);
            int quantity = quantityItem->text().toInt();
            quantity++;  // Increment the quantity by 1 (regardless of category)
            quantityItem->setText(QString::number(quantity));

            QTableWidgetItem *priceItem = ui->tableWidget->item(row, 3);
            double originalPrice = ui->tableWidget->item(row, 2)->text().toDouble();  // Get original price from column 3
            double totalPrice = quantity * originalPrice;
            priceItem->setText(QString::number(totalPrice, 'f', 3));

            itemExists = true;  // Mark item as found
            break;  // Exit the loop after updating the item
        }
    }

    // If the item does not exist in the table, add it as a new row
    if (!itemExists)
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // Create table cells
        QTableWidgetItem *nameItem = new QTableWidgetItem(itemName);
        QTableWidgetItem *quantityItem = new QTableWidgetItem("1"); // Start with quantity 1
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(price, 'f', 3));
        QTableWidgetItem *originalPriceItem = new QTableWidgetItem(QString::number(price, 'f', 2));  // Original price

        // Create Remove Button
        QPushButton *removeButton = new QPushButton("Remove");
        connect(removeButton, &QPushButton::clicked, [this, row]() {
            onRemoveButtonClicked(row);
        });

        // Add items to the table
        ui->tableWidget->setItem(row, 0, nameItem);
        ui->tableWidget->setItem(row, 1, quantityItem);
        ui->tableWidget->setItem(row, 3, priceItem);
        ui->tableWidget->setItem(row, 2, originalPriceItem);
        ui->tableWidget->setCellWidget(row, 4, removeButton);
    }
    calculateTotalPrice();
}

void dinninwindow::on_Back_clicked()
{
    this->close();
    User2Window*user2window = new User2Window();
    user2window->show();


}

void dinninwindow::onRemoveButtonClicked(int row)
{
    QTableWidgetItem *quantityItem = ui->tableWidget->item(row, 1);
    QTableWidgetItem *originalPriceItem = ui->tableWidget->item(row, 2);

    if (!quantityItem || !originalPriceItem) {
        QMessageBox::warning(this, "Error", "Invalid table item.");
        return;
    }

    int quantity = quantityItem->text().toInt();
    double unitPrice = originalPriceItem->text().toDouble();

    if (quantity > 1) {
        quantity--;
        quantityItem->setText(QString::number(quantity));

        double totalPrice = quantity * unitPrice;
        QTableWidgetItem *totalPriceItem = ui->tableWidget->item(row, 3);
        if (totalPriceItem) {
            totalPriceItem->setText(QString::number(totalPrice, 'f', 2));
        }
    } else {
        ui->tableWidget->removeRow(row);
    }

    calculateTotalPrice();
}

void dinninwindow::on_GetBillButton_clicked()
{
    QString tableNo = ui->comboBox->currentText();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Database Error", "Database is not connected.");
        return;
    }

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    // Fetch unpaid order for this table
    QSqlQuery orderQuery(db);
    orderQuery.prepare("SELECT order_no, total_price, order_type FROM orders "
                       "WHERE table_no = :table_no AND is_paid = 0");
    orderQuery.bindValue(":table_no", tableNo);

    if (!orderQuery.exec() || !orderQuery.next()) {
        QMessageBox::information(this, "No Orders", "There are no unpaid orders for this table.");
        return;
    }

    int orderNumber = orderQuery.value("order_no").toInt();
    double totalPrice = orderQuery.value("total_price").toDouble();
    QString orderType = orderQuery.value("order_type").toString();

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    // Fetch items
    QSqlQuery itemQuery(db);
    itemQuery.prepare("SELECT item_name, quantity, unit_price, total_price FROM order_items "
                      "WHERE order_no = :order_no");
    itemQuery.bindValue(":order_no", orderNumber);
    if (!itemQuery.exec()) {
        QMessageBox::critical(this, "Query Error", "Failed to fetch order items: " + itemQuery.lastError().text());
        return;
    }

    // Build bill text using spaces for alignment
    QString billText;
    billText += QString("Order No: %1\nTable: %2\nOrder Type: %3\n\n").arg(orderNumber).arg(tableNo).arg(orderType);
    billText += QString("%1\t%2\t%3\t%4\n")
                    .arg("Item", "Qty", "Unit", "Total");
    billText += "----------------------------------------\n";

    while (itemQuery.next()) {
        QString name = itemQuery.value("item_name").toString();
        int qty = itemQuery.value("quantity").toInt();
        double unit = itemQuery.value("unit_price").toDouble();
        double total = itemQuery.value("total_price").toDouble();

        billText += QString("%1\t%2\t%3\t%4\n").arg(name).arg(qty).arg(unit).arg(total);
    }
    billText += "----------------------------------------\n";
    billText += QString("Total: %1\n").arg(totalPrice);

    // ✅ Ask to print first
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Print Bill", "Do you want to print the bill?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        printBill(orderNumber); // your existing function to print nicely
    } else {
        // Show the bill in a message box if user doesn't print
        QMessageBox::information(this, "Bill Preview", billText);
    }

    // Ask if order is paid
    reply = QMessageBox::question(this, "Mark Paid", "Mark this order as paid?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE orders SET is_paid = 1, payment_status = 'paid' WHERE order_no = :order_no");
        updateQuery.bindValue(":order_no", orderNumber);

        if (!updateQuery.exec()) {
            QMessageBox::critical(this, "Update Error", "Failed to update order status: " + updateQuery.lastError().text());
            return;
        }
        QMessageBox::information(this, "Paid", "Order marked as paid successfully.");
    }

    // Clear table
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->totalPriceLabel->clear();
}
// Slot for marking orders as paid
void dinninwindow::markOrderAsPaid()
{

    QString tableNo = ui->comboBox->currentText();

    if(tableNo.isEmpty()){
        QMessageBox::warning(this,"Invalid","Select a table first");
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
    query.prepare("UPDATE orders SET is_paid = 1 WHERE table_no = :tableNo AND is_paid = 0");
    query.bindValue(":tableNo", tableNo);

    if(!query.exec()){
        QMessageBox::critical(this,"Error",query.lastError().text());
        return;
    }

    QMessageBox::information(this,"Paid","Table cleared!");
    ui->tableWidget->setRowCount(0);
}

void dinninwindow::populateTablesComboBox()
{
    if (!db.isOpen()) return;

    ui->comboBox->clear();

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    QSqlQuery query(db);
    query.prepare("SELECT table_no FROM restaurant_tables ORDER BY table_no");

    if(!query.exec()){
        qDebug()<<"Table load error:"<<query.lastError().text();
        return;
    }

    while (query.next()) {
        ui->comboBox->addItem(query.value(0).toString());
    }
}
// Load orders for the selected waiter
void dinninwindow::onTableSelected(const QString &tableNo)
{
    if (!db.isOpen()) return;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    if (tableNo.isEmpty()) return;

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    QSqlQuery query(db);
    query.prepare(
        "SELECT oi.item_name, oi.quantity, oi.unit_price, oi.total_price "
        "FROM orders o "
        "JOIN order_items oi ON o.order_no = oi.order_no "
        "WHERE o.table_no = :tableNo AND o.is_paid = 0"
        );
    query.bindValue(":tableNo", tableNo);

    if(!query.exec()){
        QMessageBox::critical(this,"Query Error",query.lastError().text());
        return;
    }

    int row=0;
    while(query.next()){
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(query.value(2).toString()));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(query.value(3).toString()));

        QPushButton *removeButton = new QPushButton("Remove");
        connect(removeButton,&QPushButton::clicked,[this,row](){
            onRemoveButtonClicked(row);
        });
        ui->tableWidget->setCellWidget(row,4,removeButton);

        row++;
    }

    calculateTotalPrice();
}
// Generate bill for the selected waiter
void dinninwindow::BillGenerated()
{
    QString tableNo = ui->comboBox->currentText();

    if (tableNo.isEmpty() || tableNo == "Select Table") {
        QMessageBox::warning(this, "Selection Error",
                             "Please select a table first.");
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
    query.prepare(
        "SELECT o.order_no, oi.item_name, oi.quantity, oi.total_price "
        "FROM orders o "
        "JOIN order_items oi ON o.order_no = oi.order_no "
        "WHERE o.table_no = :tableNo AND o.is_paid = 0"
        );
    query.bindValue(":tableNo", tableNo);

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }

    QString billDetails;
    double totalAmount = 0.0;
    int orderNumber = -1;

    while (query.next()) {
        orderNumber = query.value(0).toInt();
        QString itemName = query.value(1).toString();
        int quantity = query.value(2).toInt();
        double totalPrice = query.value(3).toDouble();

        billDetails += QString("%1  x%2  -  %3\n")
                           .arg(itemName)
                           .arg(quantity)
                           .arg(totalPrice);

        totalAmount += totalPrice;
    }

    if (billDetails.isEmpty()) {
        QMessageBox::information(this, "No Pending Bill",
                                 "No unpaid order for this table.");
        return;
    }

    billDetails += QString("\n-------------------------\nTotal: %1")
                       .arg(totalAmount);

    // 🔥 SINGLE DIALOG ONLY
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Bill Details",
                                  billDetails + "\n\nMark this order as paid?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {

        //Function for reconnect database
        if(!reconnectDatabase())
        {
            QMessageBox::critical(this,"Database Error","Cannot connect to database.");
            return;
        }
        db = QSqlDatabase::database("restaurant_connection");

        QSqlQuery update(db);
        update.prepare("UPDATE orders SET is_paid = 1, payment_status='paid' "
                       "WHERE order_no = :orderNo");
        update.bindValue(":orderNo", orderNumber);

        if (!update.exec()) {
            QMessageBox::critical(this, "Update Error",
                                  update.lastError().text());
            return;
        }

        QMessageBox::information(this, "Success",
                                 "Order marked as paid.");

        // ✅ CLEAR TABLE ONLY AFTER YES
        ui->tableWidget->setRowCount(0);
    }

    // ❌ If NO → absolutely nothing happens
}
void dinninwindow::populateCategoryButtons()
{
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Database Error", "Database is not connected.");
        return;
    }

    // Clear existing buttons from the layout
    QLayoutItem *item;
    while ((item = ui->leftLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    // Execute query to get categories
    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT category FROM menu");
    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }
    //query.seek(-1);  // Reset the query to the beginning
    while (query.next()) {
        QString categoryName = query.value(0).toString();

        QPushButton *categoryButton = new QPushButton(categoryName, this);
        categoryButton->setMinimumHeight(45);
        categoryButton->setCursor(Qt::PointingHandCursor);

        categoryButton->setStyleSheet(
            "QPushButton {"
            "color:white;"
            "background-color:#000000;"
            "border:2px solid #000000;"
            "border-radius:8px;"
            "font-size:14px;"
            "font-weight:bold;"
            "padding:8px 20px;"
            "}"
            "QPushButton:hover {"
            "background-color:#222222;"
            "}"
            "QPushButton:pressed {"
            "background-color:#444444;"
            "}"
            );
        connect(categoryButton, &QPushButton::clicked, [this, categoryName]() {
            loadCategoryMenu(categoryName);
        });

        ui->leftLayout->addWidget(categoryButton);
    }
}

void dinninwindow::printBill(int orderNumber)
{
    QPrinter printer;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    printer.setPaperSize(QSizeF(80, 200), QPrinter::Millimeter);
#else
    QPageSize pageSize(QSizeF(80, 200), QPageSize::Millimeter);
    printer.setPageSize(pageSize); // For Qt 6.x
#endif

    printer.setResolution(300);

    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Rejected) {
        QMessageBox::warning(this, "Print Cancelled", "The print operation was cancelled.");
        return;
    }

    QPainter painter(&printer);

    int yPos = 50;

    // ✅ Correct width calculation
    int pageWidth = printer.pageLayout().paintRectPixels(printer.resolution()).width();

    // Title
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(0, yPos, pageWidth, 30, Qt::AlignCenter, "Restaurant Bill");
    yPos += 50;

    // Order details
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, yPos, "Order No: " + QString::number(orderNumber));
    yPos += 20;
    painter.drawText(10, yPos, "Date: " + QDate::currentDate().toString("dd-MM-yyyy"));
    yPos += 30;

    // Separator
    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // Table header
    painter.drawText(10, yPos, "Item Name");
    painter.drawText(120, yPos, "Qty");
    painter.drawText(160, yPos, "Unit Price");
    painter.drawText(220, yPos, "Total");
    yPos += 20;

    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // Loop through the tableWidget rows
    double grandTotal = 0.0;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString itemName = ui->tableWidget->item(row, 0)->text();
        QString quantity = ui->tableWidget->item(row, 1)->text();
        QString unitPrice = ui->tableWidget->item(row, 2)->text();
        QString totalPrice = ui->tableWidget->item(row, 3)->text();

        grandTotal += totalPrice.toDouble();

        painter.drawText(10, yPos, itemName);
        painter.drawText(120, yPos, quantity);
        painter.drawText(160, yPos, unitPrice);
        painter.drawText(220, yPos, totalPrice);
        yPos += 20;
    }

    // Separator
    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // Grand total
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(10, yPos, "Grand Total: " + QString::number(grandTotal, 'f', 2));

    painter.end();
}

void dinninwindow::markAsPaid()
{
    // Example: logic to mark the selected table/order as paid
    // You can fill this later with your actual code
    qDebug() << "markAsPaid called!";
}


void dinninwindow::on_PlaceOrderButton_clicked()
{

        if (ui->tableWidget->rowCount() == 0) {
            QMessageBox::warning(this, "Empty Order", "No items to place order.");
            return;
        }

        if (!db.isOpen()) {
            QMessageBox::warning(this, "Database Error", "Database is not connected.");
            return;
        }



        QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
        QString tableNo = ui->comboBox->currentText();
        if (tableNo.isEmpty() || tableNo == "Select Table") {
            QMessageBox::warning(this, "Table Required",
                                 "Please select a table before placing the order.");
            return;
        }

        //Function for reconnect database
        if(!reconnectDatabase())
        {
            QMessageBox::critical(this,"Database Error","Cannot connect to database.");
            return;
        }
        db = QSqlDatabase::database("restaurant_connection");

        // Save order as unpaid & dine-in
        QSqlQuery insertOrderQuery(db);
        insertOrderQuery.prepare(
            "INSERT INTO orders (table_no, total_price, is_paid, payment_status, order_type, order_date) "
            "VALUES (:table_no, :total_price, :is_paid, :payment_status, :order_type, :order_date)"
            );

        double totalOrderPrice = 0.0;
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            totalOrderPrice += ui->tableWidget->item(row, 3)->text().toDouble();
        }

        insertOrderQuery.bindValue(":table_no", tableNo);
        insertOrderQuery.bindValue(":total_price", totalOrderPrice);
        insertOrderQuery.bindValue(":is_paid", 0); // unpaid
        insertOrderQuery.bindValue(":payment_status", "unpaid");
        insertOrderQuery.bindValue(":order_type", "dine_in"); // This marks the order type
        insertOrderQuery.bindValue(":order_date", currentDate);

        if (!insertOrderQuery.exec()) {
            QMessageBox::critical(this, "Insert Error", "Failed to save the order: " + insertOrderQuery.lastError().text());
            return;
        }

        // Get auto-generated order number
        int orderNumber = insertOrderQuery.lastInsertId().toInt();

        // Save each item
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QString itemName = ui->tableWidget->item(row, 0)->text();
            int quantity = ui->tableWidget->item(row, 1)->text().toInt();
            double unitPrice = ui->tableWidget->item(row, 2)->text().toDouble();
            double totalPrice = ui->tableWidget->item(row, 3)->text().toDouble();

            //Function for reconnect database
            if(!reconnectDatabase())
            {
                QMessageBox::critical(this,"Database Error","Cannot connect to database.");
                return;
            }
            db = QSqlDatabase::database("restaurant_connection");

            QSqlQuery insertItemQuery(db);
            insertItemQuery.prepare(
                "INSERT INTO order_items (order_no, item_name, quantity, unit_price, total_price) "
                "VALUES (:order_no, :item_name, :quantity, :unit_price, :total_price)"
                );
            insertItemQuery.bindValue(":order_no", orderNumber);
            insertItemQuery.bindValue(":item_name", itemName);
            insertItemQuery.bindValue(":quantity", quantity);
            insertItemQuery.bindValue(":unit_price", unitPrice);
            insertItemQuery.bindValue(":total_price", totalPrice);

            if (!insertItemQuery.exec()) {
                QMessageBox::critical(this, "Insert Error", QString("Failed to save item '%1': %2").arg(itemName).arg(insertItemQuery.lastError().text()));
                return;
            }
        }

        QMessageBox::information(this, "Order Saved", "Dine-In Order has been saved as unpaid.\nYou can continue adding items or select another table.");
}

void dinninwindow::checkForReadyOrders(){
    if (!db.isOpen())
        return;

    //Function for reconnect database
    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    QSqlQuery query(db);
    query.prepare("SELECT order_no FROM orders WHERE status='Ready' AND notification_shown = 0");

    if(!query.exec()){
        qDebug() << query.lastError().text();
        return;
    }

    while(query.next())
    {
        QString orderNo = query.value(0).toString();

        QMessageBox::information(this,"Order Ready",
                                 "Order " + orderNo + " is ready!");

        //Function for reconnect database
        if(!reconnectDatabase())
        {
            QMessageBox::critical(this,"Database Error","Cannot connect to database.");
            return;
        }
        db = QSqlDatabase::database("restaurant_connection");

        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE orders SET notification_shown = 1 WHERE order_no = :order_no");
        updateQuery.bindValue(":order_no", orderNo);
        updateQuery.exec();
    }
}
