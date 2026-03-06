#ifndef UPDATEMENU_H
#define UPDATEMENU_H

#include <QWidget>

namespace Ui {
class updatemenu;
}

class updatemenu : public QWidget
{
    Q_OBJECT

public:
    explicit updatemenu(QWidget *parent = nullptr);
    ~updatemenu();

private slots:
    void on_backButton_clicked();

    void on_cancelButton_clicked();

    void on_updateButton_clicked();

private:
    Ui::updatemenu *ui;
};

#endif // UPDATEMENU_H
