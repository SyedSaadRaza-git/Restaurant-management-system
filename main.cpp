#include "databaseconnection.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!connectToDatabase()) {
        QMessageBox::critical(nullptr, "Error", "Database not connected");
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
