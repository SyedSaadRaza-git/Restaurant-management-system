#ifndef DINNINWINDOW_H
#define DINNINWINDOW_H

#include <QWidget>
#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QWidget>
#include <QSqlDatabase>
#include <databaseconnection.h>

namespace Ui {
class dinninwindow;
}

class dinninwindow : public QWidget
{
    Q_OBJECT

public:
    explicit dinninwindow(QWidget *parent = nullptr);
    ~dinninwindow();
    void BillGenerated();
    void markAsPaid();

private slots:
    void loadCategoryMenu(const QString &category);
    void addItemToOrder(QListWidgetItem *item);



    void on_Back_clicked();

    void on_GetBillButton_clicked();
    void populateTablesComboBox();
    void onTableSelected(const QString &tableNo);; // Declare the slot here
    void markOrderAsPaid(); // Declare the function to mark orders as paid
    void populateCategoryButtons();

    void on_PlaceOrderButton_clicked();

private:
    Ui::dinninwindow *ui;
    QSqlDatabase db;
     QTimer *orderCheckTimer;

    void checkForReadyOrders();
    void connectCategoryButtons();
    void calculateTotalPrice();
    void onRemoveButtonClicked(int row);
    bool isBillGenerated = false;  // Tracks if the bill was generated
    void printBill(int orderNumber);
    void  generatedBillPdf(const QString &filePath, int orderNumber);
};

#endif // DINNINWINDOW_H
