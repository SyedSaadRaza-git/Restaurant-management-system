#include "user1window.h"
#include "qprocess.h"
#include "tablewindow.h"
#include "ui_user1window.h"
#include"staffwindow.h"
#include"menuwindow.h"
#include "mainwindow.h"
#include "qfiledialog.h"

User1Window::User1Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::User1Window)

{
    ui->setupUi(this);
    this->showFullScreen();
    setWindowTitle("Admin");
}
User1Window::~User1Window()
{
    delete ui;
}

void User1Window::on_StaffButton_clicked()
{
    StaffWindow = new staffwindow();
    StaffWindow->show();
    this->hide();
}


void User1Window::on_MenuButton_clicked()
{
    MenuWindow=new menuwindow();
    MenuWindow->show();
    this->hide();
}


void User1Window::on_LogOut_clicked()
{
    this->close();
    // Open the MainWindow
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}


void User1Window::on_TableButton_clicked()
{
    TableWindow=new tablewindow();
    TableWindow->show();
    this->hide();

}



void User1Window::on_restoreButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Backup File",
        "",
        "SQL Files (*.sql)"
        );

    if(filePath.isEmpty())
        return;

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,"Error","Cannot open backup file");
        return;
    }

    QByteArray sqlData = file.readAll();
    file.close();

    QString program = "C:/Program Files/MySQL/MySQL Server 9.5/bin/mysql.exe";

    QStringList args;
    args << "-u" << "root"
         << "-psaad"
         << "restaurant_management";

    QProcess process;
    process.start(program, args);

    if(!process.waitForStarted())
    {
        QMessageBox::critical(this,"Error","MySQL process failed to start");
        return;
    }

    process.write(sqlData);
    process.closeWriteChannel();

    process.waitForFinished(-1);

    QString error = process.readAllStandardError();

    if(error.contains("Warning"))
    {
        error.clear();   // ignore warning
    }

    if(!error.isEmpty())
    {
        QMessageBox::critical(this,"Restore Failed", error);
        return;
    }

    QMessageBox::information(this,"Restore","Database restored successfully!");
}

