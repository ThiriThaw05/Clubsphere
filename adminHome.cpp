#include "adminHome.h"
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QDateTime>
#include <QWidget>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include<database.h>

AdminHome::AdminHome(QWidget *parent) : QWidget(parent)
{
    Database::initialize();
    setupUI();
    setupStatsSection();
    setupTopClubsSection();
    setupTopUsersSection();
    setupNavigation();
}

AdminHome::~AdminHome()
{
}

void AdminHome::setupUI()
{
    // Set background color to white and fix window size
    this->setStyleSheet("background-color: white;");
    this->setFixedSize(350, 650);

    // Main layout setup
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 30, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    titleLabel = new QLabel("Welcome to\nClubsphere!", this);
    titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignLeft);

    mainLayout->addWidget(titleLabel);
}



int AdminHome::getTotalUsersCount()
{   Database::initialize();
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users_list");

    if (query.exec() && query.next()) {
        int totalUsers = query.value(0).toInt();
        return totalUsers;
    } else {
        qDebug() << "Error fetching total users count:" << query.lastError().text();
        return 0;
    }
}

int AdminHome::getTotalClubsCount()
{   Database::initialize();
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM clubs_list");

    if (query.exec() && query.next()) {
        int totalClubs = query.value(0).toInt();
        return totalClubs;
    } else {
        qDebug() << "Error fetching total clubs count:" << query.lastError().text();
        return 0;
    }
}

void AdminHome::setupStatsSection()
{
    // Stats section with two cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(12);

    // Define fonts for stats
    QFont statsLabelFont;
    statsLabelFont.setBold(true);
    statsLabelFont.setPointSize(9);

    QFont countFont;
    countFont.setBold(true);
    countFont.setPointSize(18);

    // Total Clubs card
    clubsFrame = createRoundedFrame();
    clubsFrame->setMinimumHeight(80);

    QVBoxLayout *clubsLayout = new QVBoxLayout(clubsFrame);
    clubsLayout->setContentsMargins(10, 10, 10, 10);

    totalClubsLabel = new QLabel("Total clubs", this);
    totalClubsLabel->setFont(statsLabelFont);

    int totalClubs=getTotalClubsCount();
    clubsCountLabel = new QLabel(QString::number(totalClubs), this);  // This can also be queried from a clubs table if exists
    clubsCountLabel->setFont(countFont);

    clubsLayout->addWidget(totalClubsLabel);
    clubsLayout->addWidget(clubsCountLabel);
    statsLayout->addWidget(clubsFrame);

    // Total Users card
    usersFrame = createRoundedFrame();
    usersFrame->setMinimumHeight(80);

    QVBoxLayout *usersLayout = new QVBoxLayout(usersFrame);
    usersLayout->setContentsMargins(10, 10, 10, 10);

    totalUsersLabel = new QLabel("Total Users", this);
    totalUsersLabel->setFont(statsLabelFont);

    // Get total users count from database
    int totalUsers = getTotalUsersCount();
    usersCountLabel = new QLabel(QString::number(totalUsers), this);
    usersCountLabel->setFont(countFont);

    usersLayout->addWidget(totalUsersLabel);
    usersLayout->addWidget(usersCountLabel);
    statsLayout->addWidget(usersFrame);

    mainLayout->addLayout(statsLayout);
}

void AdminHome::setupTopClubsSection()
{
    // Top 3 Clubs section
    topClubsFrame = createRoundedFrame();
    topClubsFrame->setMinimumWidth(310);
    QVBoxLayout *topClubsLayout = new QVBoxLayout(topClubsFrame);
    topClubsLayout->setContentsMargins(15, 15, 15, 15);

    thisWeekClubsLabel = new QLabel("This Week", this);
    thisWeekClubsLabel->setFont(QFont("Arial", 10));

    topClubsLabel = new QLabel("TOP 3 Clubs", this);
    QFont topLabelFont;
    topLabelFont.setBold(true);
    topLabelFont.setPointSize(14);
    topClubsLabel->setFont(topLabelFont);

    // Define club font
    QFont clubFont;
    clubFont.setBold(true);
    clubFont.setPointSize(10);

    // Get top clubs from database
    auto detailedClubs = Database::getDetailedClubRankings();

    topClubsLayout->addWidget(thisWeekClubsLabel);
    topClubsLayout->addWidget(topClubsLabel);

    if (detailedClubs.size() >= 3) {
        // Create club entries with medals
        QHBoxLayout *firstClubLayout = new QHBoxLayout();
        goldMedal1 = createMedalLabel("gold", true);
        firstClubLabel = new QLabel(detailedClubs[0].second.first, this);
        firstClubLabel->setFont(clubFont);
        firstClubLayout->addWidget(goldMedal1);
        firstClubLayout->addWidget(firstClubLabel);
        firstClubLayout->addStretch();

        QHBoxLayout *secondClubLayout = new QHBoxLayout();
        silverMedal1 = createMedalLabel("silver", true);
        secondClubLabel = new QLabel(detailedClubs[1].second.first, this);
        secondClubLabel->setFont(clubFont);
        secondClubLayout->addWidget(silverMedal1);
        secondClubLayout->addWidget(secondClubLabel);
        secondClubLayout->addStretch();

        QHBoxLayout *thirdClubLayout = new QHBoxLayout();
        bronzeMedal1 = createMedalLabel("bronze", true);
        thirdClubLabel = new QLabel(detailedClubs[2].second.first, this);
        thirdClubLabel->setFont(clubFont);
        thirdClubLayout->addWidget(bronzeMedal1);
        thirdClubLayout->addWidget(thirdClubLabel);
        thirdClubLayout->addStretch();

        topClubsLayout->addLayout(firstClubLayout);
        topClubsLayout->addLayout(secondClubLayout);
        topClubsLayout->addLayout(thirdClubLayout);
    } else {
        // Handle case with fewer than 3 clubs
        QLabel* placeholderLabel = new QLabel("Not enough clubs for ranking", this);
        placeholderLabel->setAlignment(Qt::AlignCenter);
        topClubsLayout->addWidget(placeholderLabel);
    }

    mainLayout->addWidget(topClubsFrame, 0, Qt::AlignHCenter);
}

void AdminHome::setupTopUsersSection()
{
    // TOP 3 Users section
    topUsersFrame = createRoundedFrame();
    topUsersFrame->setMinimumWidth(310);
    QVBoxLayout *topUsersLayout = new QVBoxLayout(topUsersFrame);
    topUsersLayout->setContentsMargins(15, 15, 15, 15);

    thisWeekUsersLabel = new QLabel("This Week", this);
    thisWeekUsersLabel->setFont(QFont("Arial", 10));

    topUsersLabel = new QLabel("TOP 3 Users", this);
    QFont topLabelFont;
    topLabelFont.setBold(true);
    topLabelFont.setPointSize(14);
    topUsersLabel->setFont(topLabelFont);

    // Define user font
    QFont userFont;
    userFont.setBold(true);
    userFont.setPointSize(10);

    // Get top members from database
    auto detailedMembers = Database::getDetailedMemberRankings();

    topUsersLayout->addWidget(thisWeekUsersLabel);
    topUsersLayout->addWidget(topUsersLabel);

    if (detailedMembers.size() >= 3) {
        // Create user entries with medals
        QHBoxLayout *firstUserLayout = new QHBoxLayout();
        goldMedal2 = createMedalLabel("gold", true);
        firstUserLabel = new QLabel(detailedMembers[0].second.first, this);
        firstUserLabel->setFont(userFont);
        firstUserLayout->addWidget(goldMedal2);
        firstUserLayout->addWidget(firstUserLabel);
        firstUserLayout->addStretch();

        QHBoxLayout *secondUserLayout = new QHBoxLayout();
        silverMedal2 = createMedalLabel("silver", true);
        secondUserLabel = new QLabel(detailedMembers[1].second.first, this);
        secondUserLabel->setFont(userFont);
        secondUserLayout->addWidget(silverMedal2);
        secondUserLayout->addWidget(secondUserLabel);
        secondUserLayout->addStretch();

        QHBoxLayout *thirdUserLayout = new QHBoxLayout();
        bronzeMedal2 = createMedalLabel("bronze", true);
        thirdUserLabel = new QLabel(detailedMembers[2].second.first, this);
        thirdUserLabel->setFont(userFont);
        thirdUserLayout->addWidget(bronzeMedal2);
        thirdUserLayout->addWidget(thirdUserLabel);
        thirdUserLayout->addStretch();

        topUsersLayout->addLayout(firstUserLayout);
        topUsersLayout->addLayout(secondUserLayout);
        topUsersLayout->addLayout(thirdUserLayout);
    } else {
        // Handle case with fewer than 3 users
        QLabel* placeholderLabel = new QLabel("Not enough users for ranking", this);
        placeholderLabel->setAlignment(Qt::AlignCenter);
        topUsersLayout->addWidget(placeholderLabel);
    }

    mainLayout->addWidget(topUsersFrame, 0, Qt::AlignHCenter);

    // Add spacer to push navigation bar to bottom
    mainLayout->addStretch();
}

void AdminHome::setupNavigation()
{
    // Bottom navigation bar
    navigationFrame = new QFrame(this);
    navigationFrame->setFrameShape(QFrame::NoFrame);
    navigationFrame->setLineWidth(0);
    navigationFrame->setStyleSheet("QFrame { background-color: white; }");

    QHBoxLayout *navLayout = new QHBoxLayout(navigationFrame);
    navLayout->setContentsMargins(15, 10, 15, 10);
    navLayout->setSpacing(45);  // Increased spacing to fit the wider window

    // Create navigation icons
    homeButton = new QPushButton("", this);
    homeButton->setIcon(QIcon(":/images/resources/home_logo.png"));
    homeButton->setIconSize(QSize(20, 20));
    // Highlight the home button since we're on the home page
    homeButton->setStyleSheet("QPushButton { border: none; background-color: #D9E9D8; border-radius: 10px; padding: 5px; }");

    // Initialize the profileButton
    profileButton = new QPushButton("", this);
    profileButton->setObjectName("profileButton");
    profileButton->setIcon(QIcon(":/images/resources/member_logo.png"));
    profileButton->setIconSize(QSize(20, 20));
    profileButton->setStyleSheet("QPushButton { border: none; }");

    groupsButton = new QPushButton("", this);
    groupsButton->setIcon(QIcon(":/images/resources/club_logo.png"));
    groupsButton->setIconSize(QSize(30, 30));
    groupsButton->setStyleSheet("QPushButton { border: none; }");

    navLayout->addWidget(homeButton);
    navLayout->addWidget(profileButton);
    navLayout->addWidget(groupsButton);

    mainLayout->addWidget(navigationFrame);

    // Set styles for frames - Light green background
    QString frameStyle = "QFrame { background-color: #D9E9D8; border-radius: 12px; }";
    clubsFrame->setStyleSheet(frameStyle);
    usersFrame->setStyleSheet(frameStyle);
    topClubsFrame->setStyleSheet(frameStyle);
    topUsersFrame->setStyleSheet(frameStyle);

    // Connect navigation buttons
    connect(homeButton, &QPushButton::clicked, this, &AdminHome::onHomeButtonClicked);
    connect(profileButton, &QPushButton::clicked, this, &AdminHome::onProfileButtonClicked);
    connect(groupsButton, &QPushButton::clicked, this, &AdminHome::onGroupsButtonClicked);
}

void AdminHome::onHomeButtonClicked()
{
    qDebug() << "Home button clicked in AdminHome - already on this page";
    // No navigation needed as we're already on this page
}

void AdminHome::onProfileButtonClicked()
{
    qDebug() << "Profile button clicked in AdminHome";
    emit navigateToMembers();
}

void AdminHome::onGroupsButtonClicked()
{
    qDebug() << "Groups button clicked in AdminHome";
    emit navigateToClubs();
}

QLabel* AdminHome::createMedalLabel(const QString &color, bool withRibbon)
{
    QLabel* medalLabel = new QLabel(this);

    // Use image files instead of drawing medals
    QString imagePath;
    if (color == "gold") {
        imagePath = ":/images/resources/gold_medal.png";
    } else if (color == "silver") {
        imagePath = ":/images/resources/silver_medal.png";
    } else if (color == "bronze") {
        imagePath = ":/images/resources/bronze_medal.png";
    }

    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        medalLabel->setPixmap(pixmap);
    } else {
        // Fallback if image isn't found
        medalLabel->setText(color);
    }

    medalLabel->setFixedSize(24, 24);

    return medalLabel;
}

QFrame* AdminHome::createRoundedFrame()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setLineWidth(0);
    frame->setMinimumWidth(130);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    return frame;
}
