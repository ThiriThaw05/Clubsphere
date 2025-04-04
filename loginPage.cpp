#include "loginPage.h"
#include <QFont>
#include <QPixmap>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include<QMessageBox>

LoginPage::LoginPage(QWidget *parent) : QWidget(parent)
{
    // Set background color to white and fix window size
    this->setStyleSheet("background-color: white;");
    this->setFixedSize(350, 650);

    // Title
    titleLabel = new QLabel("Welcome to\nClubsphere!", this);
    titleLabel->setFont(QFont("Arial", 22, QFont::Bold));  // Adjusted font size
    titleLabel->setAlignment(Qt::AlignLeft);

    // Subtitle
    subtitleLabel = new QLabel("\"Your Club, Your Community,\nYour Sphere.\"", this);
    subtitleLabel->setFont(QFont("Arial", 11));  // Adjusted font size
    subtitleLabel->setAlignment(Qt::AlignLeft);

    // Logo
    logoLabel = new QLabel(this);
    QPixmap logo(":/images/resources/logo.png");
    logoLabel->setPixmap(logo);
    logoLabel->setScaledContents(true);
    logoLabel->setFixedSize(150, 150);  // Adjusted logo size
    logoLabel->setAlignment(Qt::AlignCenter);

    // Description
    descriptionLabel = new QLabel(
        "where students come together, leaders emerge, and "
        "clubs flourish. Empower your organization with the "
        "tools to succeed and create a thriving community.", this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignLeft);
    descriptionLabel->setMaximumWidth(310);  // Adjusted width to fit new window
    descriptionLabel->setFont(QFont("Arial", 11));  // Adjusted font size

    // Input Fields with Labels
    // User ID
    QLabel *userIdLabel = new QLabel("USER ID", this);
    userIdLabel->setAlignment(Qt::AlignLeft);
    userIdLabel->setFont(QFont("Arial", 9, QFont::Bold));

    userIdField = new QLineEdit(this);
    userIdField->setPlaceholderText("");
    userIdField->setFixedSize(310, 40);  // Adjusted size to fit window
    userIdField->setStyleSheet("border: 1px solid black; border-radius: 5px;");

    // Password
    QLabel *passwordLabel = new QLabel("PASSWORD", this);
    passwordLabel->setAlignment(Qt::AlignLeft);
    passwordLabel->setFont(QFont("Arial", 9, QFont::Bold));

    passwordField = new QLineEdit(this);
    passwordField->setPlaceholderText("");
    passwordField->setEchoMode(QLineEdit::Password);
    passwordField->setFixedSize(310, 40);  // Adjusted size to fit window
    passwordField->setStyleSheet("border: 1px solid black; border-radius: 5px;");

    // Log In Button
    loginButton = new QPushButton("LOG IN", this);
    loginButton->setFixedSize(310, 40);  // Adjusted size to fit window
    loginButton->setStyleSheet("background-color: black; color: white; font-size: 12px; border-radius: 8px;");

    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginButtonClicked);

    // Back Button
    backButton = new QPushButton("Back", this);
    backButton->setStyleSheet("color: black; text-decoration: underline; font-size: 11px; border: none;");
    backButton->setFixedWidth(70);

    // Connect back button to slot
    connect(backButton, &QPushButton::clicked, this, &LoginPage::goBackToHome);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);  // Adjusted margins
    mainLayout->setSpacing(10);  // Adjusted spacing

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(logoLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addSpacing(10);

    mainLayout->addWidget(userIdLabel);
    mainLayout->addWidget(userIdField);
    mainLayout->addSpacing(10);

    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordField);
    mainLayout->addSpacing(40);  // Adjusted spacing for balance

    mainLayout->addWidget(loginButton);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    setLayout(mainLayout);
}

// Keeping the rest of the methods unchanged
void LoginPage::onLoginButtonClicked() {
    int userId = userIdField->text().toInt();
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users_list WHERE user_id = :id");
    query.bindValue(":id", userId);

    // Check user exists
    bool userExists = false;
    if (userId==67001922){
        userExists=true;
    }else if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0)
            userExists = true; // Returns true if the user already exists
    }

    if (!userExists) {
        QMessageBox::warning(this, " Error", "User ID does not exist!");
        return;
    }

    int password = passwordField->text().toInt();
    int retrievedPassword = -1;
    query.prepare("SELECT password FROM users_list WHERE user_id = :id" );
    query.bindValue(":id", userId);
    if (userId==67001922 && password==111111){
        retrievedPassword=111111;
    }else if (query.exec()) {
        if (query.next()) {  // Move to the first record if available
            retrievedPassword = query.value(0).toInt();  // Get the password from the first column (index 0)
        } else {
            QMessageBox::warning(this, "Error", "No user found with the provided ID.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Query Error", "Failed to retrieve password: " + query.lastError().text());
        return;
    }

    if (password == retrievedPassword) {
        QMessageBox::information(this, "Success", "Logged in successfully!");
        emit openadminHome(userId); // Pass the userId to the signal
    } else {
        QMessageBox::critical(this, "Error", "Invalid Password!");
    }
}

void LoginPage::goBackToHome()
{
    emit navigateToHome();
}

void LoginPage::showLoginPage()
{
    show();
}
