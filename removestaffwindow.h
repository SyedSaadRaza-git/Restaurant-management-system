#ifndef REMOVESTAFFWINDOW_H
#define REMOVESTAFFWINDOW_H

#include <QWidget>

namespace Ui {
class removestaffwindow;
}

class removestaffwindow : public QWidget
{
    Q_OBJECT

public:
    explicit removestaffwindow(QWidget *parent = nullptr);
    ~removestaffwindow();

private slots:
    void on_RemoveButton_clicked();

    void on_Back_clicked();

private:
    Ui::removestaffwindow *ui;
};

#endif // REMOVESTAFFWINDOW_H
