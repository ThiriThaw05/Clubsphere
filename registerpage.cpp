#include "registerPage.h"
#include <QFont>
#include <QPixmap>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QByteArray>
#include<database.h>

RegisterPage::RegisterPage(QWidget *parent) : QWidget(parent)
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
    logoLabel->setFixedSize(140, 140);  // Adjusted logo size to fit better
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
    // Student ID
    QLabel *studentIdLabel = new QLabel("STUDENT ID", this);
    studentIdLabel->setAlignment(Qt::AlignLeft);
    studentIdLabel->setFont(QFont("Arial", 9, QFont::Bold));

    studentIdEdit = new QLineEdit(this);
    studentIdEdit->setPlaceholderText("eg.67012345");
    studentIdEdit->setFixedSize(310, 35);  // Adjusted size to fit window
    studentIdEdit->setStyleSheet("border: 1px solid black; border-radius: 5px;");

    // Password
    QLabel *passwordLabel = new QLabel("PASSWORD", this);
    passwordLabel->setAlignment(Qt::AlignLeft);
    passwordLabel->setFont(QFont("Arial", 9, QFont::Bold));

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedSize(310, 35);  // Adjusted size to fit window
    passwordEdit->setStyleSheet("border: 1px solid black; border-radius: 5px;");

    // Confirm Password
    QLabel *confirmPasswordLabel = new QLabel("CONFIRM PASSWORD", this);
    confirmPasswordLabel->setAlignment(Qt::AlignLeft);
    confirmPasswordLabel->setFont(QFont("Arial", 9, QFont::Bold));

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setFixedSize(310, 35);  // Adjusted size to fit window
    confirmPasswordEdit->setStyleSheet("border: 1px solid black; border-radius: 5px;");

    // Checkbox Layout
    QHBoxLayout *termsLayout = new QHBoxLayout();
    termsCheckBox = new QCheckBox("", this);
    termsCheckBox->setStyleSheet("QCheckBox::indicator { width: 16px; height: 16px; }");  // Adjusted size

    QLabel *termsLabel = new QLabel("BY REGISTERING I AGREE WITH ALL TERMS AND CONDITIONS", this);
    termsLabel->setFont(QFont("Arial", 8));
    termsLabel->setWordWrap(true);
    termsLabel->setMaximumWidth(280);  // Adjusted width to fit

    termsLayout->addWidget(termsCheckBox);
    termsLayout->addWidget(termsLabel);
    termsLayout->setAlignment(Qt::AlignLeft);
    termsLayout->setContentsMargins(0, 5, 0, 5);  // Adjusted margins

    // Sign Up Button
    signUpButton = new QPushButton("SIGN UP", this);
    signUpButton->setFixedSize(310, 40);  // Adjusted size to fit window
    signUpButton->setStyleSheet("background-color: black; color: white; font-size: 12px; border-radius: 8px;");
    connect(signUpButton, &QPushButton::clicked, this, &RegisterPage::onRegisterButtonClicked);

    // Back Button
    backButton = new QPushButton("Back", this);
    backButton->setStyleSheet("color: black; text-decoration: underline; font-size: 11px; border: none;");
    backButton->setFixedWidth(70);

    // Connect back button to slot
    connect(backButton, &QPushButton::clicked, this, &RegisterPage::goBackToHome);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);  // Adjusted margins
    mainLayout->setSpacing(8);  // Adjusted spacing for better fit

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(5);  // Reduced negative spacing
    mainLayout->addWidget(logoLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(5);  // Reduced negative spacing
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addSpacing(10);

    mainLayout->addWidget(studentIdLabel);
    mainLayout->addWidget(studentIdEdit);
    mainLayout->addSpacing(8);

    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addSpacing(8);

    mainLayout->addWidget(confirmPasswordLabel);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addSpacing(8);

    mainLayout->addLayout(termsLayout);
    mainLayout->addSpacing(15);  // Adjusted spacing
    mainLayout->addWidget(signUpButton);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    setLayout(mainLayout);
}

void RegisterPage::onRegisterButtonClicked() {
    // Validate student ID
    QString studentIdStr = studentIdEdit->text();
    bool isValidId;
    int studentId = studentIdStr.toInt(&isValidId);

    // Validate input
    if (!isValidId || studentIdStr.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid Student ID!");
        return;
    }

    // Check if student ID exists in se_names_list
    QString studentName = Database::getStudentNameById(studentId);
    if (studentName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Student ID", "This student ID is not registered in the system!");
        return;
    }

    // Password validation
    QString passwordStr = passwordEdit->text();
    QString confirmPasswordStr = confirmPasswordEdit->text();

    // Check if passwords match
    if (passwordStr.isEmpty() || passwordStr != confirmPasswordStr) {
        QMessageBox::warning(this, "Password Error", "Passwords do not match or are empty!");
        return;
    }

    // Check if terms are agreed
    if (!termsCheckBox->isChecked()) {
        QMessageBox::warning(this, "Terms Error", "Please agree to the terms and conditions!");
        return;
    }

    // Initialize database if not already done
    Database::initialize();

    // Check if user already exists in users_list
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM users_list WHERE user_id = ?");
    checkQuery.addBindValue(studentId);

    if (checkQuery.exec() && checkQuery.next()) {
        int userCount = checkQuery.value(0).toInt();
        if (userCount > 0) {
            QMessageBox::warning(this, "Registration Error", "This user already exists!");
            return;
        }
    } else {
        qDebug() << "Error checking user existence:" << checkQuery.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to check user existence: " + checkQuery.lastError().text());
        return;
    }

    // Convert password to integer (as per your current implementation)
    int password = passwordStr.toInt();
    int point = 0;

    // Load default profile photo as byte array
    QByteArray profilePhotoData = Database::loadDefaultProfilePhoto();

    // Prepare insert query with profile photo BLOB and name from se_names_list
    QSqlQuery query;
    query.prepare("INSERT INTO users_list (user_id, password, points, profile_photo, name) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(studentId);
    query.addBindValue(password);
    query.addBindValue(point);
    query.addBindValue(profilePhotoData);
    query.addBindValue(studentName);

    if (query.exec()) {
        qDebug() << "User registered successfully!";

        // Log whether name was found in se_names_list
        if (studentName.isEmpty()) {
            qDebug() << "No pre-defined name found for student ID" << studentId;
        } else {
            qDebug() << "Used pre-defined name for student ID" << studentId << ":" << studentName;
        }

        // Verify user count after registration
        QSqlQuery countQuery;
        if (countQuery.exec("SELECT COUNT(*) FROM users_list")) {
            if (countQuery.next()) {
                qDebug() << "Total users after registration:" << countQuery.value(0).toInt();
            }
        }

        QMessageBox::information(this, "Success", "Registered successfully!");
        emit openadminHome(studentId); // Pass the studentId to the signal
    } else {
        qDebug() << "Registration failed. Error:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to register: " + query.lastError().text());
    }
}
void RegisterPage::goBackToHome()
{
    emit navigateToHome();
}

void RegisterPage::showRegisterPage()
{
    show();
}
