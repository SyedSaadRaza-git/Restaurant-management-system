#ifndef SHOWSTAFFWINDOW2_H
#define SHOWSTAFFWINDOW2_H

#include <QWidget>

namespace Ui {
class showstaffwindow2;
}

class showstaffwindow2 : public QWidget
{
    Q_OBJECT

public:
    explicit showstaffwindow2(QWidget *parent = nullptr);
    ~showstaffwindow2();

private slots:
    void on_Back_clicked();

    void on_searchButton_clicked();

    void on_showAllButton_clicked();

private:
    Ui::showstaffwindow2 *ui;
};

#endif // SHOWSTAFFWINDOW2_H
