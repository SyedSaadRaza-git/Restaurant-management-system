#include "addmenuwindow.h"
#include "ui_addmenuwindow.h"
#include "menuwindow.h"
#include<QFileDialog>
#include <QPixmap>

//headers for database connection

#include "databaseconnection.h"
#include <QSqlQuery>
#include <QSqlError>
#include<QMessageBox>

addmenuwindow::addmenuwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addmenuwindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Add Product");
}

addmenuwindow::~addmenuwindow()
{
    delete ui;
}

void addmenuwindow::on_cancelmenuButton_clicked()
{

}


void addmenuwindow::on_savemenuButton_clicked()
{
    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Error", "Database not connected");
        return;
    }
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");

    // Get input values from line edits
    QString name = ui->ItemNamelineEdit->text().trimmed();
    QString category = ui->ItemCategorylineEdit->text().trimmed();
    QString costPrice = ui->CostPricelineEdit->text().trimmed();
    QString price = ui->PricelineEdit->text().trimmed();

    // Validate inputs
    if (name.isEmpty() || category.isEmpty() || costPrice.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    // Ensure costPrice and price are valid numbers
    bool isCostPriceValid, isPriceValid;
    double costPriceValue = costPrice.toDouble(&isCostPriceValid);
    double priceValue = price.toDouble(&isPriceValid);

    if (!isCostPriceValid || !isPriceValid) {
        QMessageBox::warning(this, "Input Error", "Cost Price and Price must be valid numbers.");
        return;
    }

    if(!reconnectDatabase())
    {
        QMessageBox::critical(this,"Database Error","Cannot connect to database.");
        return;
    }
    db = QSqlDatabase::database("restaurant_connection");

    // Prepare the SQL query
    QSqlQuery query(db);
    query.prepare("INSERT INTO Menu (Name, Category, Cost_Price, selling_Price, image_path) "
                  "VALUES (:name, :category, :Cost_Price, :selling_Price,:imagePath )");
    query.bindValue(":name", name);
    query.bindValue(":category", category);
    query.bindValue(":Cost_Price", costPriceValue);
    query.bindValue(":selling_Price", priceValue);
    query.bindValue(":imagePath", selectedImagePath);

    // Execute the query and provide feedback
    if (query.exec()) {
        QMessageBox::information(this, "Success", "Item added to menu successfully!");

        // Clear input fields after successful insertion
        ui->ItemNamelineEdit->clear();
        ui->ItemCategorylineEdit->clear();
        ui->CostPricelineEdit->clear();
        ui->PricelineEdit->clear();
        ui->imagePreviewLabel->clear();
        selectedImagePath.clear();
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to add item to menu: " + query.lastError().text());
    }
}



void addmenuwindow::on_Back_clicked()
{
    this->close();
    menuwindow*MenuWindow = new menuwindow();
    MenuWindow->show();
}


void addmenuwindow::on_Back_2_clicked()
{
    this->close();
    menuwindow*MenuWindow = new menuwindow();
    MenuWindow->show();
}


void addmenuwindow::on_browseImageButton_clicked()
{
    // Open a file dialog to select an image
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (!filePath.isEmpty()) {
        QPixmap pixmap(filePath);

        // Display the selected image in the QLabel
        ui->imagePreviewLabel->setPixmap(pixmap.scaled(ui->imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // Store the file path
        selectedImagePath = filePath;
    }
}
