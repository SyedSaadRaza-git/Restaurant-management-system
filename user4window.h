#ifndef USER4WINDOW_H
#define USER4WINDOW_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
class user4window;
}

class user4window : public QWidget
{
    Q_OBJECT

public:
    explicit user4window(QWidget *parent = nullptr);
    ~user4window();

private slots:
    void on_LogOut_clicked();

    void on_btn_ok_clicked();
    void loadOrders();
    void populateTableWidget();


private:
    Ui::user4window *ui;
    QSqlDatabase db;
};

#endif // USER4WINDOW_H
