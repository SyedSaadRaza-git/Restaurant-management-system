#ifndef USER2WINDOW_H
#define USER2WINDOW_H

#include <QWidget>
#include<takeawaywindow.h>
#include<dinninwindow.h>
#include<ordertrakingwindow.h>

namespace Ui {
class User2Window;
}

class User2Window : public QWidget
{
    Q_OBJECT

public:
    explicit User2Window(QWidget *parent = nullptr);
    ~User2Window();

private slots:
    void on_TakeawayButton_clicked();

    void on_DinninButton_clicked();

    void on_LogOut_clicked();

    void on_pushButton_clicked();

private:
    Ui::User2Window *ui;
    takeawaywindow*TakeAwayWindow;
    dinninwindow*DinnInWindow;
    ordertrakingwindow*OrderTrakingWindow;
};

#endif // USER2WINDOW_H
