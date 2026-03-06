#ifndef USER3WINDOW_H
#define USER3WINDOW_H

#include <QWidget>
#include <showstaffwindow2.h>
#include <showmenuwindow2.h>
#include <orderdetailwindow.h>

namespace Ui {
class User3Window;
}

class User3Window : public QWidget
{
    Q_OBJECT

public:
    explicit User3Window(QWidget *parent = nullptr);
    ~User3Window();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_LogOut_clicked();

private:
    Ui::User3Window *ui;
    showstaffwindow2*ShowStaffWindow2;
    showmenuwindow2*ShowMenuWindow2;
    orderdetailwindow*orderDetailWindowInstance;
};

#endif // USER3WINDOW_H
