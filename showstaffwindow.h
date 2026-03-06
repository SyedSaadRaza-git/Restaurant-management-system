#ifndef SHOWSTAFFWINDOW_H
#define SHOWSTAFFWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSqlQuery>
#include <QSqlError>


namespace Ui {
class showstaffwindow;
}

class showstaffwindow : public QWidget
{
    Q_OBJECT

public:
    explicit showstaffwindow(QWidget *parent = nullptr);
    ~showstaffwindow();

private slots:
    void on_searchButton_clicked();

    void on_showAllButton_clicked();

    void on_Back_clicked();

private:
    Ui::showstaffwindow *ui;
    QTableWidget *staffDetailsTable;  // Table to display staff details
    QLineEdit *idLineEdit;           // Input field for staff ID
};

#endif // SHOWSTAFFWINDOW_H
