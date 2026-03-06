#ifndef STAFFWINDOW_H
#define STAFFWINDOW_H

#include <QWidget>
#include<addstaffwindow.h>
#include<removestaffwindow.h>
#include<showstaffwindow.h>
#include<updatestaffwindow.h>

namespace Ui {
class staffwindow;
}

class staffwindow : public QWidget
{
    Q_OBJECT

public:
    explicit staffwindow(QWidget *parent = nullptr);
    ~staffwindow();

private slots:
    void on_addstaffButton_clicked();

    void on_removestaffButton_clicked();

    void on_showstaffButton_clicked();

    void on_Back_clicked();

    void on_updateButton_clicked();

private:
    Ui::staffwindow *ui;
    AddStaffWindow*addstaffwindow;
    removestaffwindow*RemoveStaffWindow;
    showstaffwindow*ShowStaffWindow;
    updatestaffwindow*UpdateStaffWindow;
};

#endif // STAFFWINDOW_H
