// databaseconnection.cpp
#include <databaseconnection.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

bool connectToDatabase()
{
    const QString CONN = "restaurant_connection";

    if (QSqlDatabase::contains(CONN)) {
        QSqlDatabase db = QSqlDatabase::database(CONN);
        return db.isOpen();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", CONN);
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("restaurant_management");
    db.setUserName("root");
    db.setPassword("saad");

    qDebug() << "Available SQL Drivers:" << QSqlDatabase::drivers();

    if (!db.open()) {
        qDebug() << "DB ERROR:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database connection successful!";
    return true;
}

bool reconnectDatabase()
{
    QSqlDatabase db = QSqlDatabase::database("restaurant_connection");

    if (db.isOpen())
        return true;

    if (!db.open())
    {
        qDebug() << "Database reconnect failed:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database reconnected successfully";
    return true;
}
