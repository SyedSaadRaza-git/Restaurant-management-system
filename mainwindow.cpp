#include "mainwindow.h"
#include "qdatetime.h"
#include "qdir.h"
#include "qtimer.h"
#include "ui_mainwindow.h"
#include "QProcess"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,user1Window(nullptr)
    ,user2Window(nullptr)
    ,user3Window(nullptr)
    ,User4Window(nullptr)
{
    ui->setupUi(this);
    // for maximized login page
    this->showMaximized();
     setWindowTitle("Login");
     createBackup();

     QTimer *backupTimer = new QTimer(this);

     connect(backupTimer, &QTimer::timeout,
             this, &MainWindow::createBackup);

     backupTimer->start(3600000);

}

MainWindow::~MainWindow()
{
    createBackup();
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernamelineEdit->text();
    QString password = ui->passwordlineEdit->text();

    // Map of valid users and their credentials
    QMap<QString, QString> validUsers = {
        {"admin", "admin"}, // User 1 credentials
        {"order", "order"}, // User 2 credentials
        {"owner", "owner"},  // User 3 credentials
        {"kitchen", "kitchen"}
    };

    // Check credentials and open the appropriate window
    if (validUsers.contains(username) && validUsers[username] == password) {
        if (username == "admin") {
            if (!user1Window) {
                user1Window = new User1Window(); // Create User 1 window
            }
            user1Window->show(); // Show User 1 window
        } else if (username == "order") {
            if (!user2Window) {
                user2Window = new User2Window(); // Create User 2 window
            }
            user2Window->show(); // Show User 2 window
        } else if (username == "owner") {
            if (!user3Window) {
                user3Window = new User3Window(); // Create User 3 window
            }
            user3Window->show(); // Show User 3 window
        }
        else if (username == "kitchen") {
            if (!User4Window) {
                User4Window = new user4window(); // Create User 3 window
            }
            User4Window->show();
        }

        this->hide(); // Hide the login window
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password!");
    }
}

void MainWindow::createBackup()
{
    QString folder = "F:/New folder (2)/Restaurant-Managment-System/backup";

    QDir dir;
    if(!dir.exists(folder))
    {
        dir.mkpath(folder);
    }

    QString time = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm");
    QString backupFile = folder + "/backup_" + time + ".sql";

    QString program = "C:/Program Files/MySQL/MySQL Server 9.5/bin/mysqldump.exe";

    QStringList args;
    args << "--set-gtid-purged=OFF"
         << "-u" << "root"
         << "-psaad"
         << "restaurant_management";

    QProcess process;
    process.start(program, args);

    if(!process.waitForFinished())
    {
        qDebug() << "Backup process failed!";
        return;
    }

    QByteArray backupData = process.readAllStandardOutput();
    QByteArray errorData = process.readAllStandardError();

    if(!errorData.isEmpty())
    {
        qDebug() << "Backup error:" << errorData;
    }

    QFile file(backupFile);

    if(file.open(QIODevice::WriteOnly))
    {
        file.write(backupData);
        file.close();
        qDebug() << "Backup created:" << backupFile;
    }
    else
    {
        qDebug() << "File write failed!";
    }

    // Keep only last 5 backups
    QDir backupDir(folder);

    QStringList files = backupDir.entryList(QStringList() << "*.sql",
                                            QDir::Files,
                                            QDir::Time);

    if(files.size() > 5)
    {
        for(int i = 5; i < files.size(); i++)
        {
            backupDir.remove(files[i]);
        }
    }
}
