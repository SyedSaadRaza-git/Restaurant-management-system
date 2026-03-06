#include "takeawaywindow.h"
#include "ui_takeawaywindow.h"
#include "user2window.h"
#include "databaseconnection.h"
#include <QDateTime> // Include this header for working with dates
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
//headers for database connection
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
//header for notification
#include <QTimer>
takeawaywindow::takeawaywindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::takeawaywindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Take Away");

    orderCheckTimer = new QTimer(this);

    connect(orderCheckTimer, &QTimer::timeout,
            this, &takeawaywindow::checkForReadyOrders);

    orderCheckTimer->start(5000); // check every 3 seconds

    //code to expand the table and add color
    ui->tableWidget->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to database.");
        return;
    }

    // Get existing connection
    db = QSqlDatabase::database("restaurant_connection");
    connectCategoryButtons();
    populateCategoryButtons();
    checkForReadyOrders();


    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Item Name" << "Quantity" << "Price"<<"Total Price"<<"Remove");
    ui->tableWidget->setColumnWidth(0, 200);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 100);
    ui->tableWidget->setColumnWidth(3, 100);
    ui->tableWidget->setColumnWidth(4, 100);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

}
takeawaywindow::~takeawaywindow()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void takeawaywindow::connectCategoryButtons()
{
    // Disconnect previous signals to avoid multiple connections
    disconnect(ui->listWidget, &QListWidget::itemClicked, this, &takeawaywindow::addItemToOrder);
}

void takeawaywindow::calculateTotalPrice()
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

void takeawaywindow::loadCategoryMenu(const QString &category)
{    if (!db.isOpen()) {
        QMessageBox::warning(this, "Database Error", "Database is not connected.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT Name, selling_price, image_path FROM menu WHERE category = :category");
    query.bindValue(":category", category);

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }

    // Clear the list widget for the new category
    ui->listWidget->clear();

    // Set the view mode to IconMode for a grid-like layout
    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(100, 100)); // Adjust size for the icons
    ui->listWidget->setSpacing(10);              // Adjust spacing between items
    ui->listWidget->setResizeMode(QListWidget::Adjust);

    while (query.next()) {
        QString itemName = query.value(0).toString();
        double price = query.value(1).toDouble();
        QString imagePath = query.value(2).toString();

        // Create the list widget item
        QListWidgetItem *listItem = new QListWidgetItem();

        // Set the text as the item name and price
        listItem->setText(itemName);   // show only name
        listItem->setToolTip(QString::number(price, 'f', 2)); // optional hover price

        listItem->setData(Qt::UserRole, price);
        listItem->setData(Qt::UserRole + 1, itemName);

        // Load and set the icon (image) for the item
        if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
            QIcon itemIcon(imagePath);
            listItem->setIcon(itemIcon);
        } else {
            // Set a default icon if the image path is invalid
            listItem->setIcon(QIcon(":/resources/default.png")); // Replace with your default icon
        }

        // Store the price as data
        listItem->setData(Qt::UserRole, price);

        // Add the item to the list widget
        ui->listWidget->addItem(listItem);
    }

    // Connect the item click event to add the item to the order
    disconnect(ui->listWidget, &QListWidget::itemClicked, this, &takeawaywindow::addItemToOrder);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &takeawaywindow::addItemToOrder);
}

void takeawaywindow::addItemToOrder(QListWidgetItem *item)
{
    // Get item name and price from the QListWidgetItem
    QString itemName = item->data(Qt::UserRole + 1).toString();
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




void takeawaywindow::on_LogOut_clicked()
{
    this->close();
    User2Window*user2window= new User2Window();
    user2window->show();
}


void takeawaywindow::onRemoveButtonClicked(int row)
{
    //quantity of the item and unit price
    QTableWidgetItem *quantityItem = ui->tableWidget->item(row, 1);
    QTableWidgetItem *originalPriceItem = ui->tableWidget->item(row, 2);

    int quantity = quantityItem->text().toInt();
    double unitPrice = originalPriceItem->text().toDouble();

    // Decrease the quantity
    if (quantity > 1) {
        quantity--;
        quantityItem->setText(QString::number(quantity));

        // Update the price
        double totalPrice = quantity * unitPrice;
        QTableWidgetItem *totalPriceItem = ui->tableWidget->item(row, 3);
        totalPriceItem->setText(QString::number(totalPrice, 'f', 2));
    } else {
        // If quantity is 1, remove the row
        ui->tableWidget->removeRow(row);
    }

    // Recalculate the total price
    calculateTotalPrice();
}



void takeawaywindow::on_GetBillButton_clicked()
{
if (ui->tableWidget->rowCount() == 0) {
    QMessageBox::warning(this, "Empty Order", "There are no items in the order to generate a bill.");
    return;
}

if (!db.isOpen()) {
    QMessageBox::warning(this, "Database Error", "Database is not connected.");
    return;
}

// ✅ Step 1: Insert the order into the 'orders' table
QString waiterName = "John Doe";  // You can change this based on the logged-in user
double totalOrderPrice = 0.0;

// Calculate total order price
for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
    totalOrderPrice += ui->tableWidget->item(row, 3)->text().toDouble();
}

QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");

// Insert the order details
QSqlQuery orderQuery(db);
orderQuery.prepare(
    "INSERT INTO orders (total_price, is_paid, order_date, order_type) "
    "VALUES (:total_price, 0, :order_date, 'Take Away')"
    );
orderQuery.bindValue(":total_price", totalOrderPrice);
orderQuery.bindValue(":order_date", currentDate);

if (!orderQuery.exec()) {
    QMessageBox::critical(this, "Insert Error", "Failed to save order: " + orderQuery.lastError().text());
    return;
}

// ✅ Step 2: Get the generated order number
int orderNumber = orderQuery.lastInsertId().toInt();

// ✅ Step 3: Insert each item into the 'order_items' table
for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
    QString itemName = ui->tableWidget->item(row, 0)->text();
    int quantity = ui->tableWidget->item(row, 1)->text().toInt();
    double unitPrice = ui->tableWidget->item(row, 2)->text().toDouble();
    double totalPrice = ui->tableWidget->item(row, 3)->text().toDouble();

    QSqlQuery itemQuery(db);
    itemQuery.prepare("INSERT INTO order_items (order_no, item_name, quantity, unit_price, total_price) "
                      "VALUES (:order_no, :item_name, :quantity, :unit_price, :total_price)");
    itemQuery.bindValue(":order_no", orderNumber);
    itemQuery.bindValue(":item_name", itemName);
    itemQuery.bindValue(":quantity", quantity);
    itemQuery.bindValue(":unit_price", unitPrice);
    itemQuery.bindValue(":total_price", totalPrice);

    if (!itemQuery.exec()) {
        QMessageBox::critical(this, "Insert Error", QString("Failed to save item '%1': %2").arg(itemName).arg(itemQuery.lastError().text()));
        return;
    }
}

QSqlQuery updateQuery(db);
updateQuery.prepare("UPDATE orders SET is_paid = 1, payment_status = 'Paid' "
                    "WHERE order_no = :order_no");
updateQuery.bindValue(":order_no", orderNumber);

if(!updateQuery.exec()) {
    qDebug() << "Payment update failed:" << updateQuery.lastError().text();
}

// ✅ Step 4: Show success message
QMessageBox::information(this, "Order Saved", "Order No: " + QString::number(orderNumber) + "\nAll items have been successfully saved to the database.");

// ✅ Step 5: Ask if the user wants to print the bill
QMessageBox::StandardButton reply;
reply = QMessageBox::question(this, "Print Bill", "Do you want to print the bill?", QMessageBox::Yes | QMessageBox::No);

if (reply == QMessageBox::Yes) {
    printBill(orderNumber);
    ui->tableWidget->clear();
    ui->totalPriceLabel->clear();
}



}
void takeawaywindow::populateCategoryButtons()
{
   if (!db.isOpen()) {
            QMessageBox::warning(this, "Database Error", "Database is not connected.");
            return;
        }

        // Clear existing buttons from the layout
        QLayoutItem *item;
        while ((item = ui->LeftLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

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

            ui->LeftLayout->addWidget(categoryButton);
        }
}



//void takeawaywindow::printBill(int orderNumber)
void takeawaywindow::printBill(int orderNumber)
{
      // ✅ Step 1: Setup printer and painter
    QPrinter printer;
    QPrintDialog dialog(&printer, this);

    if (dialog.exec() == QDialog::Rejected) {
        QMessageBox::warning(this, "Print Cancelled", "The print operation was cancelled.");
        return;
    }

    QPainter painter(&printer);
    int yPos = 50;
    int pageWidth = printer.pageLayout().paintRectPixels(printer.resolution()).width();

    // ✅ Step 2: Retrieve order details from the 'orders' table
    QSqlQuery orderQuery(db);
    orderQuery.prepare("SELECT  total_price, order_date FROM orders WHERE order_no = :order_no");
    orderQuery.bindValue(":order_no", orderNumber);

    if (!orderQuery.exec() || !orderQuery.next()) {
        QMessageBox::critical(this, "Error", "Failed to retrieve order details.");
        return;
    }

    double totalOrderPrice = orderQuery.value(0).toDouble();
    QString orderDate = orderQuery.value(1).toDate().toString("dd-MM-yyyy");

    // ✅ Step 3: Print Bill Header
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(0, yPos, pageWidth, 30, Qt::AlignCenter, "Restaurant Bill");
    yPos += 50;

    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, yPos, "Order No: " + QString::number(orderNumber));
    yPos += 20;
    painter.drawText(10, yPos, "Date: " + orderDate);
    yPos += 20;

    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // ✅ Step 4: Print Column Headers for Item Details
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(10, yPos, "Item Name");
    painter.drawText(150, yPos, "Quantity");
    painter.drawText(220, yPos, "Unit Price");
    painter.drawText(320, yPos, "Total Price");
    yPos += 20;

    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // ✅ Step 5: Retrieve item details from the 'order_items' table
    QSqlQuery itemQuery(db);
    itemQuery.prepare("SELECT item_name, quantity, unit_price, total_price FROM order_items WHERE order_no = :order_no");
    itemQuery.bindValue(":order_no", orderNumber);

    if (!itemQuery.exec()) {
        QMessageBox::critical(this, "Error", "Failed to retrieve item details.");
        return;
    }


    // ✅ Step 6: Print each item in the order
    painter.setFont(QFont("Arial", 10));
    while (itemQuery.next()) {
        QString itemName = itemQuery.value(0).toString();
        int quantity = itemQuery.value(1).toInt();
        double unitPrice = itemQuery.value(2).toDouble();
        double totalPrice = itemQuery.value(3).toDouble();

        painter.drawText(10, yPos, itemName);
        painter.drawText(150, yPos, QString::number(quantity));
        painter.drawText(220, yPos, QString::number(unitPrice, 'f', 2));
        painter.drawText(320, yPos, QString::number(totalPrice, 'f', 2));
        yPos += 20;
    }

    // ✅ Step 7: Draw a line after all items
    painter.drawLine(10, yPos, pageWidth - 10, yPos);
    yPos += 20;

    // ✅ Step 8: Print Grand Total
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(10, yPos, "Grand Total: " + QString::number(totalOrderPrice, 'f', 2));

    // ✅ Step 9: End Painting
    painter.end();
}


void takeawaywindow::checkForReadyOrders(){
    if (!db.isOpen())
        return;

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

        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE orders SET notification_shown = 1 WHERE order_no = :order_no");
        updateQuery.bindValue(":order_no", orderNo);
        updateQuery.exec();
    }

}
