#include "homePage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QPixmap>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    // Set background color to white and fix window size
    this->setStyleSheet("background-color: white;");
    this->setFixedSize(350, 650);

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 60, 20, 40);  // Adjusted margins to fit window
    layout->setSpacing(15);  // Adjusted spacing

    // Logo
    QLabel *logo = new QLabel(this);
    QPixmap pixmap(":/images/resources/logo.png");
    if (!pixmap.isNull()) {
        logo->setPixmap(pixmap);
        logo->setScaledContents(true);
        logo->setFixedSize(240, 240);  // Adjusted logo size to fit window
    }
    layout->addWidget(logo, 0, Qt::AlignCenter);

    // Heading
    QLabel *heading = new QLabel("<p style='font-size:18px; font-weight:bold; text-align:center;'>"
                                 "Your Club, Your Community,<br>Your Sphere</p>", this);
    heading->setAlignment(Qt::AlignCenter);
    layout->addWidget(heading);

    // Description
    QLabel *description = new QLabel("Where students come together, leaders emerge, and clubs flourish. "
                                     "Empower your organization with the tools to succeed and create a thriving community.", this);
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignCenter);
    description->setStyleSheet("font-size: 12px; color: black; padding: 0 10px;");
    description->setMaximumWidth(310);  // Adjusted width to fit window
    layout->addWidget(description);

    // Add spacing to push buttons down for better balance
    layout->addSpacing(40);

    // Button Layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    loginButton = new QPushButton("Log In", this);
    registerButton = new QPushButton("Register", this);
    loginButton->setFixedSize(130, 40);  // Adjusted button size
    loginButton->setStyleSheet("background-color: black; color: white; border-radius: 8px; font-size: 14px;");
    buttonLayout->addWidget(loginButton);
    registerButton->setFixedSize(130, 40);  // Adjusted button size
    registerButton->setStyleSheet("background-color: white; color: black; border: 2px solid black; border-radius: 8px; font-size: 14px;");
    buttonLayout->addWidget(registerButton);

    // Connect signals
    connect(registerButton, &QPushButton::clicked, this, [=]() {
        this->hide();
        emit openRegisterPage();
    });

    connect(loginButton, &QPushButton::clicked, this, [=]() {
        this->hide();
        emit openLoginPage();
    });

    layout->addLayout(buttonLayout);
    layout->setAlignment(buttonLayout, Qt::AlignCenter);

    // Add some spacing at the bottom for balance
    layout->addSpacing(30);

    setLayout(layout);
    setStyleSheet("background-color: white;");
}

void HomePage::showHomePage()
{
    show();
}
