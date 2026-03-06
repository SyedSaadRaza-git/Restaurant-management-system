#ifndef ORDERDETAILWINDOW_H
#define ORDERDETAILWINDOW_H

#include <QWidget>

namespace Ui {
class orderdetailwindow;
}

class orderdetailwindow : public QWidget
{
    Q_OBJECT

public:
    explicit orderdetailwindow(QWidget *parent = nullptr);
    ~orderdetailwindow();



private slots:
    void on_Back_clicked();

    void on_showAllButton_clicked();
    void removeOrder(const QString &order_no);
    void showTodayOrders();


    void on_pushButton_clicked();

private:
    Ui::orderdetailwindow *ui;
};

#endif // ORDERDETAILWINDOW_H
