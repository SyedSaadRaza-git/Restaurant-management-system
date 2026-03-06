#ifndef ADDSTAFFWINDOW_H
#define ADDSTAFFWINDOW_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
class AddStaffWindow;
}

class AddStaffWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddStaffWindow(QWidget *parent = nullptr);
    ~AddStaffWindow();

private slots:
    void on_saveButton_clicked();

    void on_Back_clicked();

    void on_Back_2_clicked();

private:
    Ui::AddStaffWindow *ui;
    QSqlDatabase db;
};

#endif // ADDSTAFFWINDOW_H
