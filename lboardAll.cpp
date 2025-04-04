#include "lboardAll.h"
#include "database.h"
#include <QIcon>
#include <QFont>
#include <QFrame>
#include <QRadioButton>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

LeaderboardAll::LeaderboardAll(int club_id, QWidget *parent)
    : QWidget(parent), m_clubId(club_id), m_currentTab(ClubMembers)
{
    setupUI();
    showClubMembersLeaderboard();
}

LeaderboardAll::~LeaderboardAll()
{
}

void LeaderboardAll::setupUI()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Set background color for the entire page
    setStyleSheet("background-color: #d6e3c6;");

    // Header with page title
    QWidget* headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    headerWidget->setStyleSheet("background-color: #d6e3c6;");

    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(12, 4, 12, 4);

    // Back button using PNG
    QPushButton* backButton = new QPushButton();
    backButton->setIcon(QIcon(":/images/resources/back.png"));
    backButton->setIconSize(QSize(32, 32));
    backButton->setFixedSize(32, 32);
    backButton->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    connect(backButton, &QPushButton::clicked, this, &LeaderboardAll::navigateBack);

    // Page title
    m_titleLabel = new QLabel("LeaderBoard");
    QFont titleFont;
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(backButton, 0, Qt::AlignLeft);
    headerLayout->addWidget(m_titleLabel, 1, Qt::AlignCenter);
    headerLayout->addSpacing(32); // Balance the back button

    // Tab switching widget
    QWidget* tabWidget = new QWidget();
    tabWidget->setStyleSheet("background-color: #d6e3c6;");
    QHBoxLayout* tabLayout = new QHBoxLayout(tabWidget);
    tabLayout->setContentsMargins(20, 10, 20, 10);

    // Create the tab container with rounded corners
    QWidget* tabContainer = new QWidget();
    tabContainer->setStyleSheet(
        "QWidget { background-color: white; border-radius: 25px; }"
        );
    tabContainer->setFixedHeight(50); // Set fixed height for the container

    QHBoxLayout* tabContainerLayout = new QHBoxLayout(tabContainer);
    tabContainerLayout->setContentsMargins(5, 5, 5, 5);
    tabContainerLayout->setSpacing(0);

    // Tab group for exclusive selection
    m_tabGroup = new QButtonGroup(this);

    // Club Members tab
    m_clubMembersTab = new QRadioButton("Club Members");
    m_clubMembersTab->setStyleSheet(
        "QRadioButton { background-color: white; border-radius: 20px; padding: 8px 15px; font-weight: bold; font-size: 9px; }"
        "QRadioButton:checked { background-color: #eaf5dd; }"
        "QRadioButton::indicator { width: 0; height: 0; }"
        );
    m_clubMembersTab->setFixedHeight(40);
    m_clubMembersTab->setChecked(true);

    // Members tab
    m_membersTab = new QRadioButton("All Members");
    m_membersTab->setStyleSheet(
        "QRadioButton { background-color: white; border-radius: 20px; padding: 8px 15px; font-weight: bold; font-size: 9px; }"
        "QRadioButton:checked { background-color: #eaf5dd; }"
        "QRadioButton::indicator { width: 0; height: 0; }"
        );
    m_membersTab->setFixedHeight(40);

    // Clubs tab
    m_clubsTab = new QRadioButton("All Clubs");
    m_clubsTab->setStyleSheet(
        "QRadioButton { background-color: white; border-radius: 20px; padding: 8px 15px; font-weight: bold; font-size: 9px; }"
        "QRadioButton:checked { background-color: #eaf5dd; }"
        "QRadioButton::indicator { width: 0; height: 0; }"
        );
    m_clubsTab->setFixedHeight(40);

    m_tabGroup->addButton(m_clubMembersTab);
    m_tabGroup->addButton(m_membersTab);
    m_tabGroup->addButton(m_clubsTab);

    tabContainerLayout->addWidget(m_clubMembersTab, 1);
    tabContainerLayout->addWidget(m_membersTab, 1);
    tabContainerLayout->addWidget(m_clubsTab, 1);

    tabLayout->addWidget(tabContainer);

    // Connect tab signals
    connect(m_clubMembersTab, &QRadioButton::clicked, this, &LeaderboardAll::onClubMembersTabClicked);
    connect(m_membersTab, &QRadioButton::clicked, this, &LeaderboardAll::onMembersTabClicked);
    connect(m_clubsTab, &QRadioButton::clicked, this, &LeaderboardAll::onClubsTabClicked);

    // Container for top 3 leaders
    m_topThreeWidget = new QWidget();
    m_topThreeWidget->setStyleSheet("background-color: #d6e3c6; border-bottom-left-radius: 30px; border-bottom-right-radius: 30px;");

    // Scroll area for the list items below top 3
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet("background-color: transparent; border: none;");

    // Container for the list items that will be inside the scroll area
    QWidget* listContainer = new QWidget();
    listContainer->setStyleSheet("background-color: #d6e3c6;");
    m_leaderListLayout = new QVBoxLayout(listContainer);
    m_leaderListLayout->setContentsMargins(10, 10, 10, 10);
    m_leaderListLayout->setSpacing(20);

    // Set the list container as the scroll area widget
    m_scrollArea->setWidget(listContainer);

    // Add all to main layout
    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(tabWidget);
    m_mainLayout->addWidget(m_topThreeWidget);
    m_mainLayout->addWidget(m_scrollArea, 1); // Scroll area takes remaining space

    // Set fixed dimensions
    setFixedWidth(350);
    setFixedHeight(650);
}

void LeaderboardAll::setupTopLeaders()
{
    // Clear previous content
    if (m_topThreeWidget->layout()) {
        QLayoutItem* item;
        while ((item = m_topThreeWidget->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete m_topThreeWidget->layout();
    }

    QHBoxLayout* topLeadersLayout = new QHBoxLayout(m_topThreeWidget);
    topLeadersLayout->setContentsMargins(10, 20, 10, 30);
    topLeadersLayout->setSpacing(0);

    switch (m_currentTab) {
    case ClubMembers: {
        // Get club members with ranking from database
        auto clubMembers = Database::getMembersWithRank(m_clubId);

        if (clubMembers.size() >= 3) {
            // Second place (left)
            QString name2 = clubMembers[1].second.first;
            int rank2 = clubMembers[1].second.second;
            int userId2 = clubMembers[1].first;
            QWidget* secondPlace = createLeaderItem(name2, userId2, true, 2);

            // First place (center)
            QString name1 = clubMembers[0].second.first;
            int rank1 = clubMembers[0].second.second;
            int userId1 = clubMembers[0].first;
            QWidget* firstPlace = createLeaderItem(name1, userId1, true, 1);

            // Third place (right)
            QString name3 = clubMembers[2].second.first;
            int rank3 = clubMembers[2].second.second;
            int userId3 = clubMembers[2].first;
            QWidget* thirdPlace = createLeaderItem(name3, userId3, true, 3);

            topLeadersLayout->addWidget(secondPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(firstPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(thirdPlace, 1, Qt::AlignBottom);
        } else {
            // Handle case with fewer than 3 members
            QLabel* placeholderLabel = new QLabel("Not enough members for ranking");
            placeholderLabel->setAlignment(Qt::AlignCenter);
            topLeadersLayout->addWidget(placeholderLabel);
        }
        break;
    }
    case Members: {
        // Get top members from database
        auto detailedMembers = Database::getDetailedMemberRankings();

        if (detailedMembers.size() >= 3) {
            // Second place (left)
            QString name2 = detailedMembers[1].second.first;
            int points2 = detailedMembers[1].second.second.first;
            int userId2 = detailedMembers[1].first;  // Add this line
            QWidget* secondPlace = createLeaderItem(name2, userId2, true, 2);  // Pass userId2 instead of points2

            // First place (center)
            QString name1 = detailedMembers[0].second.first;
            int points1 = detailedMembers[0].second.second.first;
            int userId1 = detailedMembers[0].first;  // Add this line
            QWidget* firstPlace = createLeaderItem(name1, userId1, true, 1);  // Pass userId1 instead of points1

            // Third place (right)
            QString name3 = detailedMembers[2].second.first;
            int points3 = detailedMembers[2].second.second.first;
            int userId3 = detailedMembers[2].first;  // Add this line
            QWidget* thirdPlace = createLeaderItem(name3, userId3, true, 3);  // Pass userId3 instead of points3

            topLeadersLayout->addWidget(secondPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(firstPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(thirdPlace, 1, Qt::AlignBottom);
        } else {
            // Handle case with fewer than 3 members
            QLabel* placeholderLabel = new QLabel("Not enough members for ranking");
            placeholderLabel->setAlignment(Qt::AlignCenter);
            topLeadersLayout->addWidget(placeholderLabel);
        }
        break;
    }
    case Clubs: {
        // Get top clubs from database
        auto detailedClubs = Database::getDetailedClubRankings();

        if (detailedClubs.size() >= 3) {
            // Second place (left)
            QString name2 = detailedClubs[1].second.first;
            int points2 = detailedClubs[1].second.second.first;
            QWidget* secondPlace = createLeaderItem(name2, points2, true, 2);

            // First place (center)
            QString name1 = detailedClubs[0].second.first;
            int points1 = detailedClubs[0].second.second.first;
            QWidget* firstPlace = createLeaderItem(name1, points1, true, 1);

            // Third place (right)
            QString name3 = detailedClubs[2].second.first;
            int points3 = detailedClubs[2].second.second.first;
            QWidget* thirdPlace = createLeaderItem(name3, points3, true, 3);

            topLeadersLayout->addWidget(secondPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(firstPlace, 1, Qt::AlignBottom);
            topLeadersLayout->addWidget(thirdPlace, 1, Qt::AlignBottom);
        } else {
            // Handle case with fewer than 3 clubs
            QLabel* placeholderLabel = new QLabel("Not enough clubs for ranking");
            placeholderLabel->setAlignment(Qt::AlignCenter);
            topLeadersLayout->addWidget(placeholderLabel);
        }
        break;
    }
    }
}

void LeaderboardAll::setupListItems()
{
    // Clear previous items
    QLayoutItem* item;
    while ((item = m_leaderListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    switch (m_currentTab) {
    case ClubMembers: {
        // Get club members with ranking from database
        auto clubMembers = Database::getMembersWithRank(m_clubId);

        // Skip the top 3 members and add the rest
        for (int i = 3; i < clubMembers.size(); i++) {
            int userId = clubMembers[i].first;
            QString name = clubMembers[i].second.first;
            int rank = clubMembers[i].second.second;

            // Get member points from users_list
            QSqlQuery query;
            query.prepare("SELECT points FROM users_list WHERE user_id = :userId");
            query.bindValue(":userId", userId);

            int points = 0;
            if (query.exec() && query.next()) {
                points = query.value(0).toInt();
            }

            // Create a list item with rank display
            QWidget* itemWidget = new QWidget();
            itemWidget->setStyleSheet("background-color: white; border-radius: 15px;");

            QHBoxLayout* rowLayout = new QHBoxLayout(itemWidget);
            rowLayout->setContentsMargins(10, 10, 10, 10);
            rowLayout->setSpacing(10);

            // Rank number
            QLabel* rankLabel = new QLabel(QString::number(rank) + ".");
            QFont rankFont;
            rankFont.setPointSize(14);
            rankFont.setBold(true);
            rankLabel->setFont(rankFont);
            rankLabel->setFixedWidth(30);
            rowLayout->addWidget(rankLabel);

            // Profile picture - try to load from database first
            QLabel* profilePic = new QLabel();
            profilePic->setFixedSize(40, 40);

            QPixmap userPixmap;
            bool imageLoaded = false;

            // Try to load user profile from database
            query.prepare("SELECT profile_photo FROM users_list WHERE user_id = :userId");
            query.bindValue(":userId", userId);

            if (query.exec() && query.next()) {
                QByteArray photoData = query.value(0).toByteArray();
                if (!photoData.isEmpty()) {
                    if (userPixmap.loadFromData(photoData)) {
                        // Create circular profile photo
                        QPixmap circularPhoto(40, 40);
                        circularPhoto.fill(Qt::transparent);

                        QPainter painter(&circularPhoto);
                        painter.setRenderHint(QPainter::Antialiasing);
                        painter.setBrush(QBrush(userPixmap.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                        painter.setPen(Qt::NoPen);
                        painter.drawEllipse(0, 0, 40, 40);
                        painter.end();

                        userPixmap = circularPhoto;
                        imageLoaded = true;
                    }
                }
            }

            // Fallback to default user image if needed
            if (!imageLoaded) {
                userPixmap = QPixmap(40, 40);
                userPixmap.fill(Qt::transparent);

                QPixmap defaultUser(":/images/resources/user.png");
                if (!defaultUser.isNull()) {
                    QPainter painter(&userPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setBrush(QBrush(defaultUser.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(0, 0, 40, 40);
                    painter.end();
                } else {
                    // Create a default user icon
                    QPainter painter(&userPixmap);
                    painter.setPen(QPen(Qt::gray, 2));
                    painter.setBrush(Qt::lightGray);
                    painter.drawEllipse(5, 5, 30, 30);
                    painter.drawEllipse(15, 10, 10, 10); // Head
                    painter.drawRect(12, 22, 16, 13); // Body
                    painter.end();
                }
            }

            profilePic->setPixmap(userPixmap);
            profilePic->setStyleSheet("border-radius: 20px; background-color: transparent;");
            rowLayout->addWidget(profilePic);

            // Name with ID
            QLabel* nameLabel = new QLabel(name + " (" + QString::number(userId) + ")");
            QFont nameFont;
            nameFont.setPointSize(12);
            nameLabel->setFont(nameFont);
            rowLayout->addWidget(nameLabel, 1);

            // Points
            QLabel* pointsLabel = new QLabel(QString::number(points));
            QFont pointsFont;
            pointsFont.setPointSize(14);
            pointsFont.setBold(true);
            pointsLabel->setFont(pointsFont);
            rowLayout->addWidget(pointsLabel);

            m_leaderListLayout->addWidget(itemWidget);
        }
        break;
    }
    case Members: {
        // Get member rankings from database
        auto detailedMembers = Database::getDetailedMemberRankings();

        // Skip the top 3 members and add the rest
        for (int i = 3; i < detailedMembers.size(); i++) {
            int userId = detailedMembers[i].first;
            QString name = detailedMembers[i].second.first;
            int points = detailedMembers[i].second.second.first;
            int rank = detailedMembers[i].second.second.second;

            // Create a list item with rank display
            QWidget* itemWidget = new QWidget();
            itemWidget->setStyleSheet("background-color: white; border-radius: 15px;");

            QHBoxLayout* rowLayout = new QHBoxLayout(itemWidget);
            rowLayout->setContentsMargins(10, 10, 10, 10);
            rowLayout->setSpacing(10);

            // Rank number
            QLabel* rankLabel = new QLabel(QString::number(rank) + ".");
            QFont rankFont;
            rankFont.setPointSize(14);
            rankFont.setBold(true);
            rankLabel->setFont(rankFont);
            rankLabel->setFixedWidth(30);
            rowLayout->addWidget(rankLabel);

            // Profile picture - try to load from database first
            QLabel* profilePic = new QLabel();
            profilePic->setFixedSize(40, 40);

            QPixmap userPixmap;
            bool imageLoaded = false;

            // Try to load user profile from database
            QSqlQuery query;
            query.prepare("SELECT profile_photo FROM users_list WHERE user_id = :userId");
            query.bindValue(":userId", userId);

            if (query.exec() && query.next()) {
                QByteArray photoData = query.value(0).toByteArray();
                if (!photoData.isEmpty()) {
                    if (userPixmap.loadFromData(photoData)) {
                        // Create circular profile photo
                        QPixmap circularPhoto(40, 40);
                        circularPhoto.fill(Qt::transparent);

                        QPainter painter(&circularPhoto);
                        painter.setRenderHint(QPainter::Antialiasing);
                        painter.setBrush(QBrush(userPixmap.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                        painter.setPen(Qt::NoPen);
                        painter.drawEllipse(0, 0, 40, 40);
                        painter.end();

                        userPixmap = circularPhoto;
                        imageLoaded = true;
                    }
                }
            }

            // Fallback to default user image if needed
            if (!imageLoaded) {
                userPixmap = QPixmap(40, 40);
                userPixmap.fill(Qt::transparent);

                QPixmap defaultUser(":/images/resources/user.png");
                if (!defaultUser.isNull()) {
                    QPainter painter(&userPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setBrush(QBrush(defaultUser.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(0, 0, 40, 40);
                    painter.end();
                } else {
                    // Create a default user icon
                    QPainter painter(&userPixmap);
                    painter.setPen(QPen(Qt::gray, 2));
                    painter.setBrush(Qt::lightGray);
                    painter.drawEllipse(5, 5, 30, 30);
                    painter.drawEllipse(15, 10, 10, 10); // Head
                    painter.drawRect(12, 22, 16, 13); // Body
                    painter.end();
                }
            }

            profilePic->setPixmap(userPixmap);
            profilePic->setStyleSheet("border-radius: 20px; background-color: transparent;");
            rowLayout->addWidget(profilePic);

            // Name with ID
            QLabel* nameLabel = new QLabel(name + " (" + QString::number(userId) + ")");
            QFont nameFont;
            nameFont.setPointSize(12);
            nameLabel->setFont(nameFont);
            rowLayout->addWidget(nameLabel, 1);

            // Points
            QLabel* pointsLabel = new QLabel(QString::number(points));
            QFont pointsFont;
            pointsFont.setPointSize(14);
            pointsFont.setBold(true);
            pointsLabel->setFont(pointsFont);
            rowLayout->addWidget(pointsLabel);

            m_leaderListLayout->addWidget(itemWidget);
        }
        break;
    }
    case Clubs: {
        // Get club rankings from database
        auto detailedClubs = Database::getDetailedClubRankings();

        // Skip the top 3 clubs and add the rest
        for (int i = 3; i < detailedClubs.size(); i++) {
            int clubId = detailedClubs[i].first;
            QString name = detailedClubs[i].second.first;
            int points = detailedClubs[i].second.second.first;
            int rank = detailedClubs[i].second.second.second;

            // Create a list item with rank display
            QWidget* itemWidget = new QWidget();
            itemWidget->setStyleSheet("background-color: white; border-radius: 15px;");

            QHBoxLayout* rowLayout = new QHBoxLayout(itemWidget);
            rowLayout->setContentsMargins(10, 10, 10, 10);
            rowLayout->setSpacing(10);

            // Rank number
            QLabel* rankLabel = new QLabel(QString::number(rank) + ".");
            QFont rankFont;
            rankFont.setPointSize(14);
            rankFont.setBold(true);
            rankLabel->setFont(rankFont);
            rankLabel->setFixedWidth(30);
            rowLayout->addWidget(rankLabel);

            // Club picture - try to load from database first
            QLabel* clubPic = new QLabel();
            clubPic->setFixedSize(40, 40);

            QPixmap clubPixmap;
            bool imageLoaded = false;

            // Try to load club image from database
            QSqlQuery query;
            query.prepare("SELECT club_photo FROM clubs_list WHERE club_id = :clubId");
            query.bindValue(":clubId", clubId);

            if (query.exec() && query.next()) {
                QByteArray photoData = query.value(0).toByteArray();
                if (!photoData.isEmpty()) {
                    if (clubPixmap.loadFromData(photoData)) {
                        // Create circular club photo
                        QPixmap circularPhoto(40, 40);
                        circularPhoto.fill(Qt::transparent);

                        QPainter painter(&circularPhoto);
                        painter.setRenderHint(QPainter::Antialiasing);
                        painter.setBrush(QBrush(clubPixmap.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                        painter.setPen(Qt::NoPen);
                        painter.drawEllipse(0, 0, 40, 40);
                        painter.end();

                        clubPixmap = circularPhoto;
                        imageLoaded = true;
                    }
                }
            }

            // Fallback to default club image if needed
            if (!imageLoaded) {
                clubPixmap = QPixmap(40, 40);
                clubPixmap.fill(Qt::transparent);

                QPixmap defaultClub(":/images/resources/club.png");
                if (!defaultClub.isNull()) {
                    QPainter painter(&clubPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setBrush(QBrush(defaultClub.scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(0, 0, 40, 40);
                    painter.end();
                } else {
                    // Create a default club icon
                    QPainter painter(&clubPixmap);
                    painter.setPen(QPen(Qt::gray, 2));
                    painter.setBrush(Qt::lightGray);
                    painter.drawRoundedRect(5, 5, 30, 30, 5, 5);
                    painter.drawLine(10, 15, 30, 15);
                    painter.drawLine(10, 22, 30, 22);
                    painter.drawLine(10, 29, 30, 29);
                    painter.end();
                }
            }

            clubPic->setPixmap(clubPixmap);
            clubPic->setStyleSheet("border-radius: 20px; background-color: transparent;");
            rowLayout->addWidget(clubPic);

            // Name with ID
            QLabel* nameLabel = new QLabel(name + " (" + QString::number(clubId) + ")");
            QFont nameFont;
            nameFont.setPointSize(12);
            nameLabel->setFont(nameFont);
            rowLayout->addWidget(nameLabel, 1);

            // Points
            QLabel* pointsLabel = new QLabel(QString::number(points));
            QFont pointsFont;
            pointsFont.setPointSize(14);
            pointsFont.setBold(true);
            pointsLabel->setFont(pointsFont);
            rowLayout->addWidget(pointsLabel);

            m_leaderListLayout->addWidget(itemWidget);
        }
        break;
    }
    }

    // Add stretch to push items to the top
    m_leaderListLayout->addStretch(1);
}

QWidget* LeaderboardAll::createLeaderItem(const QString& name, int idOrPoints, bool isTopThree, int rank)
{
    QWidget* itemWidget = new QWidget();

    if (isTopThree) {
        itemWidget->setStyleSheet("background-color: transparent;");
        itemWidget->setFixedHeight(220); // Increased height to accommodate all elements

        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 5, 5, 5);
        itemLayout->setSpacing(8); // Consistent spacing between elements
        itemLayout->setAlignment(Qt::AlignCenter);

        // Medal label - adjust positioning based on rank
        QLabel* medalLabel = new QLabel();
        QPixmap medalPixmap;
        if (rank == 1) {
            medalPixmap.load(":/images/resources/gold_medal.png");
            if (medalPixmap.isNull()) {
                qDebug() << "Failed to load gold medal image";
                // Use fallback - create a gold-colored circle
                medalPixmap = QPixmap(40, 40);
                medalPixmap.fill(Qt::transparent);
                QPainter painter(&medalPixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setBrush(QColor(255, 215, 0)); // Gold color
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(0, 0, 40, 40);
                painter.end();
            }
        } else if (rank == 2) {
            medalPixmap.load(":/images/resources/silver_medal.png");
            if (medalPixmap.isNull()) {
                qDebug() << "Failed to load silver medal image";
                // Use fallback - create a silver-colored circle
                medalPixmap = QPixmap(40, 40);
                medalPixmap.fill(Qt::transparent);
                QPainter painter(&medalPixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setBrush(QColor(192, 192, 192)); // Silver color
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(0, 0, 40, 40);
                painter.end();
            }
        } else {
            medalPixmap.load(":/images/resources/bronze_medal.png");
            if (medalPixmap.isNull()) {
                qDebug() << "Failed to load bronze medal image";
                // Use fallback - create a bronze-colored circle
                medalPixmap = QPixmap(40, 40);
                medalPixmap.fill(Qt::transparent);
                QPainter painter(&medalPixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setBrush(QColor(205, 127, 50)); // Bronze color
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(0, 0, 40, 40);
                painter.end();
            }
        }

        medalLabel->setPixmap(medalPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        medalLabel->setFixedSize(40, 40);

       /* // Add spacing above medal based on rank
        if (rank == 1) {
            itemLayout->addSpacing(15); // Extra space for gold medal
        } else if (rank == 2) {
            itemLayout->addSpacing(10); // Less space for silver
        }*/
        itemLayout->addWidget(medalLabel, 0, Qt::AlignCenter);

        // Profile picture container with fixed size
        QWidget* picContainer = new QWidget();
        picContainer->setFixedSize(100, 100); // Give enough space for the photo

        QVBoxLayout* picLayout = new QVBoxLayout(picContainer);
        picLayout->setContentsMargins(0, 0, 0, 0);
        picLayout->setSpacing(0);

        QLabel* profilePic = new QLabel();
        profilePic->setFixedSize(80, 80);
        profilePic->setAlignment(Qt::AlignCenter);


        // Try to load profile/club image from database
        QPixmap profilePixmap;
        bool imageLoaded = false;
        int points = 0;

        if (m_currentTab == ClubMembers || m_currentTab == Members) {
            // For ClubMembers, idOrPoints is userId
            int userId = idOrPoints;

            // Try to load user profile from database
            QSqlQuery query;
            query.prepare("SELECT profile_photo, points FROM users_list WHERE user_id = :userId");
            query.bindValue(":userId", userId);

            if (query.exec() && query.next()) {
                QByteArray photoData = query.value(0).toByteArray();
                points = query.value(1).toInt();

                if (!photoData.isEmpty()) {
                    if (profilePixmap.loadFromData(photoData)) {
                        // Create circular profile photo
                        QPixmap circularPhoto(80, 80);
                        circularPhoto.fill(Qt::transparent);

                        QPainter painter(&circularPhoto);
                        painter.setRenderHint(QPainter::Antialiasing);
                        painter.setBrush(QBrush(profilePixmap.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                        painter.setPen(Qt::NoPen);
                        painter.drawEllipse(0, 0, 80, 80);
                        painter.end();

                        profilePixmap = circularPhoto;
                        imageLoaded = true;
                    }
                }
            }

            // Fallback to default user image if needed
            if (!imageLoaded) {
                profilePixmap = QPixmap(80, 80);
                profilePixmap.fill(Qt::transparent);

                QPixmap defaultUser(":/images/resources/user.png");
                if (!defaultUser.isNull()) {
                    QPainter painter(&profilePixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setBrush(QBrush(defaultUser.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(0, 0, 80, 80);
                    painter.end();
                } else {
                    // Create a default user icon
                    QPainter painter(&profilePixmap);
                    painter.setPen(QPen(Qt::gray, 2));
                    painter.setBrush(Qt::lightGray);
                    painter.drawEllipse(10, 10, 60, 60);
                    painter.drawEllipse(30, 20, 20, 20); // Head
                    painter.drawRect(25, 45, 30, 25); // Body
                    painter.end();
                }
            }
        } else {
            // For Clubs tab, idOrPoints is points directly
            points = idOrPoints;

            // Try to load club image from database
            QSqlQuery query;
            query.prepare("SELECT club_photo FROM clubs_list WHERE club_name = :name");
            query.bindValue(":name", name);

            if (query.exec() && query.next()) {
                QByteArray photoData = query.value(0).toByteArray();
                if (!photoData.isEmpty()) {
                    if (profilePixmap.loadFromData(photoData)) {
                        // Create circular club photo
                        QPixmap circularPhoto(80, 80);
                        circularPhoto.fill(Qt::transparent);

                        QPainter painter(&circularPhoto);
                        painter.setRenderHint(QPainter::Antialiasing);
                        painter.setBrush(QBrush(profilePixmap.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                        painter.setPen(Qt::NoPen);
                        painter.drawEllipse(0, 0, 80, 80);
                        painter.end();

                        profilePixmap = circularPhoto;
                        imageLoaded = true;
                    }
                }
            }

            // Fallback to default club image if needed
            if (!imageLoaded) {
                profilePixmap = QPixmap(80, 80);
                profilePixmap.fill(Qt::transparent);

                QPixmap defaultClub(":/images/resources/club.png");
                if (!defaultClub.isNull()) {
                    QPainter painter(&profilePixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    painter.setBrush(QBrush(defaultClub.scaled(80, 80, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(0, 0, 80, 80);
                    painter.end();
                } else {
                    // Create a default club icon
                    QPainter painter(&profilePixmap);
                    painter.setPen(QPen(Qt::gray, 2));
                    painter.setBrush(Qt::lightGray);
                    painter.drawRoundedRect(10, 10, 60, 60, 10, 10);
                    painter.drawLine(20, 30, 60, 30);
                    painter.drawLine(20, 45, 60, 45);
                    painter.drawLine(20, 60, 60, 60);
                    painter.end();
                }
            }
        }

        profilePic->setPixmap(profilePixmap);
        profilePic->setStyleSheet("border-radius: 40px; background-color: transparent;");

        picLayout->addWidget(profilePic, 0, Qt::AlignCenter);
        itemLayout->addWidget(picContainer);

        // Name label with fixed maximum width to prevent overflow
        QLabel* nameLabel = new QLabel(name);
        nameLabel->setMaximumWidth(120); // Prevent text from being too wide
        QFont nameFont;
        nameFont.setPointSize(10); // Slightly reduced font size
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setWordWrap(true);
        nameLabel->setStyleSheet("color: #333;");
        itemLayout->addWidget(nameLabel);

        // Points label
        QLabel* pointsLabel = new QLabel(QString::number(points) + " pts");
        QFont pointsFont;
        pointsFont.setPointSize(12); // Reduced font size for better balance
        pointsFont.setBold(true);
        pointsLabel->setFont(pointsFont);
        pointsLabel->setAlignment(Qt::AlignCenter);
        pointsLabel->setStyleSheet("color: #4a6741;");
        itemLayout->addWidget(pointsLabel);

        // Adjust bottom spacing based on rank
        if (rank == 3) {
            itemLayout->addSpacing(10); // Extra space for bronze position
        }

        return itemWidget;
    } else {
        return nullptr;
    }
}


void LeaderboardAll::clearLeaderboard()
{
    setupTopLeaders();
    setupListItems();
}

void LeaderboardAll::showClubMembersLeaderboard()
{
    m_currentTab = ClubMembers;
    m_titleLabel->setText("Club Members Leaderboard");
    clearLeaderboard();
}

void LeaderboardAll::showMembersLeaderboard()
{
    m_currentTab = Members;
    m_titleLabel->setText("All Members Leaderboard");
    clearLeaderboard();
}

void LeaderboardAll::showClubsLeaderboard()
{
    m_currentTab = Clubs;
    m_titleLabel->setText("All Clubs Leaderboard");
    clearLeaderboard();
}

void LeaderboardAll::onClubMembersTabClicked()
{
    showClubMembersLeaderboard();
}

void LeaderboardAll::onMembersTabClicked()
{
    showMembersLeaderboard();
}

void LeaderboardAll::onClubsTabClicked()
{
    showClubsLeaderboard();
}
