// mhomepage.cpp
#include "mhomepage.h"
#include "database.h"
#include <QIcon>
#include <QFont>
#include <QPixmap>
#include <QFrame>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QMessageBox>



// MHomepage implementation
MHomepage::MHomepage(int userId, QWidget *parent)
    : QWidget(parent), m_userId(userId)
{
    setupUI();
    loadUserClubEvents();
}

MHomepage::~MHomepage()
{
}

void MHomepage::setupUI()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Header widget with white background
    QWidget* headerWidget = new QWidget();
    headerWidget->setStyleSheet("background-color: white;");
    QVBoxLayout* headerLayout = new QVBoxLayout(headerWidget);

    // Title and header icons
    QHBoxLayout* titleLayout = new QHBoxLayout();

    // Title
    m_titleLabel = new QLabel("Clubsphere");
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();

    // Header icons (achievement, chat, notification)
    m_headerIconsLayout = new QHBoxLayout();

    // Achievement/Leaderboard button
    QPushButton* achievementButton = new QPushButton();
    achievementButton->setIcon(QIcon(":/images/resources/medal.png"));
    achievementButton->setIconSize(QSize(30, 30));
    achievementButton->setFixedSize(45, 45);
    achievementButton->setStyleSheet("QPushButton { background-color: #e0e0e0; border-radius: 22px; }");
    connect(achievementButton, &QPushButton::clicked, this, &MHomepage::onLeaderboardClicked);

    // Join Club button
    QPushButton* joinClubButton = new QPushButton();
    joinClubButton->setIcon(QIcon(":/images/resources/join_club.png"));
    joinClubButton->setIconSize(QSize(30, 30));
    joinClubButton->setFixedSize(45, 45);
    joinClubButton->setStyleSheet("QPushButton { background-color: #e0e0e0; border-radius: 22px; }");
    connect(joinClubButton, &QPushButton::clicked, this, &MHomepage::onSearchClubsClicked);

    m_headerIconsLayout->addWidget(joinClubButton);
    m_headerIconsLayout->addWidget(achievementButton);
    m_headerIconsLayout->setSpacing(10);

    titleLayout->addLayout(m_headerIconsLayout);

    // Subtitle for Events
    QHBoxLayout* subtitleLayout = new QHBoxLayout();
    QLabel* eventsLabel = new QLabel("Your Club Events");
    QFont eventsFont = eventsLabel->font();
    eventsFont.setPointSize(14);
    eventsFont.setBold(true);
    eventsLabel->setFont(eventsFont);

    subtitleLayout->addWidget(eventsLabel);
    subtitleLayout->addStretch();

    // Add sections to header layout
    headerLayout->addLayout(titleLayout);
    headerLayout->addSpacing(10);
    headerLayout->addLayout(subtitleLayout);

    // Scroll area for events
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet("background-color: #f0f0f0;");

    m_scrollContent = new QWidget();
    m_scrollContent->setStyleSheet("background-color: #f0f0f0;");

    m_EventsLayout = new QVBoxLayout(m_scrollContent);
    m_EventsLayout->setSpacing(15);
    m_EventsLayout->setContentsMargins(10, 10, 10, 10);

    m_scrollArea->setWidget(m_scrollContent);

    // Bottom navigation bar
    QWidget* bottomNavBar = new QWidget();
    bottomNavBar->setFixedHeight(60);
    bottomNavBar->setStyleSheet("background-color: white; border-top: 1px solid #e0e0e0;");

    QHBoxLayout* bottomNavLayout = new QHBoxLayout(bottomNavBar);
    bottomNavLayout->setContentsMargins(10, 5, 10, 5);
    bottomNavLayout->setSpacing(0);

    // Create navigation buttons
    // Home icon
    QPushButton* homeIcon = new QPushButton();
    homeIcon->setIcon(QIcon(":/images/resources/home_logo.png"));
    homeIcon->setIconSize(QSize(24, 24));
    homeIcon->setFixedSize(40, 40);
    homeIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Club icon
    QPushButton* clubIcon = new QPushButton();
    clubIcon->setIcon(QIcon(":/images/resources/club_logo.png"));
    clubIcon->setIconSize(QSize(24, 24));
    clubIcon->setFixedSize(40, 40);
    clubIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Event icon
    QPushButton* eventIcon = new QPushButton();
    eventIcon->setIcon(QIcon(":/images/resources/event.png"));
    eventIcon->setIconSize(QSize(24, 24));
    eventIcon->setFixedSize(40, 40);
    eventIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // In MHomepage::setupUI() method, replace the profile icon creation with:

    // Profile icon with user photo
    QPushButton* profileIcon = new QPushButton();
    QSqlQuery profileQuery;
    profileQuery.prepare("SELECT profile_photo FROM users_list WHERE user_id = :userId");
    profileQuery.bindValue(":userId", m_userId);
    if (profileQuery.exec() && profileQuery.next() && !profileQuery.value(0).isNull()) {
        QByteArray photoData = profileQuery.value(0).toByteArray();
        QPixmap userPhoto;
        if (userPhoto.loadFromData(photoData)) {
            // Create circular profile photo using the same approach as in MProfilePage
            QPixmap roundedPixmap = createCircularPixmap(userPhoto, 24);
            profileIcon->setIcon(QIcon(roundedPixmap));
            profileIcon->setIconSize(QSize(40, 40));
        } else {
            profileIcon->setIcon(QIcon(":/images/resources/user.png"));
            profileIcon->setIconSize(QSize(40, 40)); // Match the size in MProfilePage
        }
    } else {
        profileIcon->setIcon(QIcon(":/images/resources/user.png"));
        profileIcon->setIconSize(QSize(40, 40)); // Match the size in MProfilePage
    }
    profileIcon->setFixedSize(40, 40);
    profileIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Add navigation buttons to layout
    bottomNavLayout->addStretch(1);
    bottomNavLayout->addWidget(homeIcon);
    bottomNavLayout->addStretch(1);
    bottomNavLayout->addWidget(clubIcon);
    bottomNavLayout->addStretch(1);
    bottomNavLayout->addWidget(eventIcon);
    bottomNavLayout->addStretch(1);
    bottomNavLayout->addWidget(profileIcon);
    bottomNavLayout->addStretch(1);

    // Connect navigation buttons
    connect(homeIcon, &QPushButton::clicked, this, &MHomepage::onHomeClicked);
    connect(clubIcon, &QPushButton::clicked, this, &MHomepage::onClubClicked);
    connect(eventIcon, &QPushButton::clicked, this, &MHomepage::onEventClicked);
    connect(profileIcon, &QPushButton::clicked, this, &MHomepage::onProfileClicked);

    // Add all to main layout
    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(m_scrollArea, 1);
    m_mainLayout->addWidget(bottomNavBar);

    // Set fixed width to match mobile viewport
    setFixedWidth(350);
    setMinimumHeight(650);
}

QPixmap MHomepage::createCircularPixmap(const QPixmap& pixmap, int size)
{
    QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap roundedPixmap(size, size);
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(scaledPixmap));
    painter.drawEllipse(0, 0, size, size);

    return roundedPixmap;
}
void MHomepage::loadUserClubEvents()
{
    // Clear existing events
    QLayoutItem* item;
    while ((item = m_EventsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Get user's joined clubs
    QSqlQuery userQuery;
    userQuery.prepare("SELECT joined_clubs, going_events FROM users_list WHERE user_id = :userId");
    userQuery.bindValue(":userId", m_userId);

    QVector<int> joinedClubs;
    QVector<int> goingEvents;

    if (userQuery.exec() && userQuery.next()) {
        QString joinedClubsStr = userQuery.value(0).toString();
        QString goingEventsStr = userQuery.value(1).toString();

        joinedClubs = Database::deserializeUserIds(joinedClubsStr);
        goingEvents = Database::deserializeUserIds(goingEventsStr);
    } else {
        qDebug() << "Error retrieving user's joined clubs:" << userQuery.lastError().text();
        QLabel* errorLabel = new QLabel("Error retrieving your club list. Please try again later.", this);
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("font-size: 14px; color: #ff0000; margin: 20px;");
        m_EventsLayout->addWidget(errorLabel);
        return;
    }

    if (joinedClubs.isEmpty()) {
        QLabel* noClubsLabel = new QLabel("You haven't joined any clubs yet. Join clubs to see their events here!", this);
        noClubsLabel->setAlignment(Qt::AlignCenter);
        noClubsLabel->setStyleSheet("font-size: 14px; color: #505050; margin: 20px;");
        noClubsLabel->setWordWrap(true);
        m_EventsLayout->addWidget(noClubsLabel);
        return;
    }

    // Build the SQL query to get events from joined clubs
    QString clubIdList;
    for (int i = 0; i < joinedClubs.size(); ++i) {
        if (i > 0) clubIdList += ", ";
        clubIdList += QString::number(joinedClubs[i]);
    }

    // Load events from joined clubs
    QSqlQuery query;
    query.prepare(QString("SELECT e.event_id, c.club_name, e.event_content, e.event_photo, "
                          "e.created_timestamp, e.event_going_count, e.club_id "
                          "FROM events_list e "
                          "JOIN clubs_list c ON e.club_id = c.club_id "
                          "WHERE e.club_id IN (%1) "
                          "ORDER BY e.created_timestamp DESC").arg(clubIdList));

    if (query.exec()) {
        bool hasEvents = false;

        while (query.next()) {
            hasEvents = true;

            int eventId = query.value(0).toInt();
            QString clubName = query.value(1).toString();
            QString content = query.value(2).toString();
            QByteArray imageData = query.value(3).toByteArray();
            QDateTime timestamp = QDateTime::fromSecsSinceEpoch(query.value(4).toLongLong());
            int goingCount = query.value(5).toInt();
            bool isAlreadyGoing = goingEvents.contains(eventId);

            // Add separator except for the first event
            if (m_EventsLayout->count() > 0) {
                QFrame* line = new QFrame();
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                line->setStyleSheet("background-color: #d0d0d0;");
                line->setFixedHeight(1);
                m_EventsLayout->addWidget(line);
            }

            // Add the event card
            MEventCard* card = new MEventCard(eventId, clubName, timestamp, content, imageData,
                                            goingCount, m_userId, isAlreadyGoing, this);
            m_EventsLayout->addWidget(card);
        }

        if (!hasEvents) {
            QLabel* noEventsLabel = new QLabel("No events found for your clubs", this);
            noEventsLabel->setAlignment(Qt::AlignCenter);
            noEventsLabel->setStyleSheet("font-size: 14px; color: #505050; margin: 20px;");
            m_EventsLayout->addWidget(noEventsLabel);
        }
    } else {
        qDebug() << "Error loading events:" << query.lastError().text();
        QLabel* errorLabel = new QLabel("Error loading events: " + query.lastError().text(), this);
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("font-size: 14px; color: #ff0000; margin: 20px;");
        m_EventsLayout->addWidget(errorLabel);
    }
}

void MHomepage::refreshEvents()
{
    loadUserClubEvents();
}

void MHomepage::onHomeClicked()
{
    emit navigateToHome();
}

void MHomepage::onClubClicked()
{
    emit navigateToClub();
}

void MHomepage::onEventClicked()
{
    emit navigateToGoing();
}

void MHomepage::onProfileClicked()
{
    emit navigateToProfile();
}

void MHomepage::onLeaderboardClicked()
{
    emit navigateToBoard();
}

void MHomepage::onSearchClubsClicked()
{
    emit navigateToSearchClubs();
}
