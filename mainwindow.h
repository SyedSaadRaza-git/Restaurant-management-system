#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMessageBox>
#include<user1window.h>
#include<user2window.h>
#include<user3window.h>
#include<user4window.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();
    void createBackup();

private:
    Ui::MainWindow *ui;
    User1Window *user1Window;
    User2Window *user2Window;
    User3Window *user3Window;
    user4window *User4Window;
};
#endif // MAINWINDOW_H
