// databaseconnection.h
#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QSqlDatabase>

bool connectToDatabase();
bool reconnectDatabase();

#endif // DATABASECONNECTION_H
