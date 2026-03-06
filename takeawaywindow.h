#ifndef TAKEAWAYWINDOW_H
#define TAKEAWAYWINDOW_H

#include <QWidget>
#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlDatabase>

namespace Ui {
class takeawaywindow;
}

class takeawaywindow : public QWidget
{
    Q_OBJECT

public:
    explicit takeawaywindow(QWidget *parent = nullptr);
    ~takeawaywindow();
    void onGetBillClicked();
    void populateCategoryButtons();
private slots:
    void loadCategoryMenu(const QString &category);
    void addItemToOrder(QListWidgetItem *item);

    void on_LogOut_clicked();

    void on_GetBillButton_clicked();


private:
    Ui::takeawaywindow *ui;
    QTimer *orderCheckTimer;
    QSqlDatabase db;
    void connectCategoryButtons();
    void calculateTotalPrice();
    void onRemoveButtonClicked(int row);
    void createCategoryButtons();
    void printBill(int orderNumber);
    void checkForReadyOrders();
    void  generateBillPdf(const QString &filePath, int orderNumber);

    //void initializeOrderID();  // Declare the method here
    //int currentOrderID;  // To store the current order ID
    //bool orderIdSet;     // Flag to check if order ID has been set

};

#endif // TAKEAWAYWINDOW_H
