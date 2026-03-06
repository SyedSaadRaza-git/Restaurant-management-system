#include "mainwindow.h"
#include "ui_mainwindow.h"


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
}

MainWindow::~MainWindow()
{
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

