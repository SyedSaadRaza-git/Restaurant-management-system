#ifndef UPDATESTAFFWINDOW_H
#define UPDATESTAFFWINDOW_H

#include <QWidget>

namespace Ui {
class updatestaffwindow;
}

class updatestaffwindow : public QWidget
{
    Q_OBJECT

public:
    explicit updatestaffwindow(QWidget *parent = nullptr);
    ~updatestaffwindow();

private slots:
    void on_cancelButton_clicked();

    void on_updateButton_clicked();

    void on_backButton_clicked();

private:
    Ui::updatestaffwindow *ui;
};

#endif // UPDATESTAFFWINDOW_H
