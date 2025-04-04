#include "Navigation.h"
#include <QIcon>

NavigationBar::NavigationBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *navLayout = new QHBoxLayout(this);
    navLayout->setContentsMargins(15, 10, 15, 10);
    navLayout->setSpacing(35);
    // this->setFixedHeight(15);

    homeButton = new QPushButton(QIcon(":/images/resources/home_logo.png"), "", this);
    profileButton = new QPushButton(QIcon(":/images/resources/member_logo.png"), "", this);
    groupsButton = new QPushButton(QIcon(":/images/resources/club_logo.png"), "", this);
    notificationsButton = new QPushButton(QIcon(":/images/resources/noti_logo.png"), "", this);

    homeButton->setIconSize(QSize(20, 20));
    profileButton->setIconSize(QSize(20, 20));
    groupsButton->setIconSize(QSize(20, 20));
    notificationsButton->setIconSize(QSize(20, 20));

    // Set the style to match what you had in AdminHome
    homeButton->setStyleSheet("QPushButton { border: none; }");
    profileButton->setStyleSheet("QPushButton { border: none; }");
    groupsButton->setStyleSheet("QPushButton { border: none; }");
    notificationsButton->setStyleSheet("QPushButton { border: none; }");

    navLayout->addWidget(homeButton);
    navLayout->addWidget(profileButton);
    navLayout->addWidget(groupsButton);
    navLayout->addWidget(notificationsButton);

    connect(homeButton, &QPushButton::clicked, this, &NavigationBar::onHomeClicked);
    connect(profileButton, &QPushButton::clicked, this, &NavigationBar::onProfileClicked);
    connect(groupsButton, &QPushButton::clicked, this, &NavigationBar::onGroupsClicked);
    connect(notificationsButton, &QPushButton::clicked, this, &NavigationBar::onNotificationsClicked);
}

// Implement the slots that were declared in the header
void NavigationBar::onHomeClicked()
{
    emit navigateToHome();
}

void NavigationBar::onProfileClicked()
{
    emit navigateToProfile();
}

void NavigationBar::onGroupsClicked()
{
    emit navigateToGroups();
}

void NavigationBar::onNotificationsClicked()
{
    emit navigateToNotifications();
}
