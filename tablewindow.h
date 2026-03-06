#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QWidget>

namespace Ui {
class tablewindow;
}

class tablewindow : public QWidget
{
    Q_OBJECT

public:
    explicit tablewindow(QWidget *parent = nullptr);
    ~tablewindow();

private slots:
    void on_backButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void loadTables();

private:
    Ui::tablewindow *ui;
};

#endif // TABLEWINDOW_H
