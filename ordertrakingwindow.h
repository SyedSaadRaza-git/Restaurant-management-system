#ifndef ORDERTRAKINGWINDOW_H
#define ORDERTRAKINGWINDOW_H

#include <QWidget>
#include <QSqlDatabase>


namespace Ui {
class ordertrakingwindow;
}

class ordertrakingwindow : public QWidget
{
    Q_OBJECT

public:
    explicit ordertrakingwindow(QWidget *parent = nullptr);
    ~ordertrakingwindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ordertrakingwindow *ui;
    QSqlDatabase db;
    void populateTableWidget();
    void populateOrderNumbers();
};

#endif // ORDERTRAKINGWINDOW_H
