#ifndef REMOVEMENUWINDOW_H
#define REMOVEMENUWINDOW_H

#include <QWidget>

namespace Ui {
class removemenuwindow;
}

class removemenuwindow : public QWidget
{
    Q_OBJECT

public:
    explicit removemenuwindow(QWidget *parent = nullptr);
    ~removemenuwindow();

private slots:
    void on_removeButton_clicked();

    void on_Back_clicked();

private:
    Ui::removemenuwindow *ui;
};

#endif // REMOVEMENUWINDOW_H
