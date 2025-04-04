// mgoingpage.cpp
#include "mgoingpage.h"
#include "database.h" // Include for database functions
#include <QIcon>
#include <QFont>
#include <QDebug>
#include <QFrame>
#include <QScrollArea>
#include <QSqlError>

MGoingPage::MGoingPage(int userID, QWidget *parent)
    : QWidget(parent), m_currentUserId(userID)
{
    qDebug() << "MGoingPage constructor called";
    setupUI();
    qDebug() << "MGoingPage UI setup completed";
    loadUserEvents();
}

MGoingPage::~MGoingPage()
{
    qDebug() << "MGoingPage destructor called";
}


void MGoingPage::loadUserEvents()
{

    // Add this to loadUserEvents() at the beginning
    QSqlQuery debugQuery;
    debugQuery.exec("SELECT user_id, going_events FROM users_list WHERE user_id = " + QString::number(m_currentUserId));
    if (debugQuery.next()) {
        qDebug() << "User ID:" << debugQuery.value(0).toInt();
        qDebug() << "Raw going_events value:" << debugQuery.value(1).toString();
    }


    if (m_currentUserId <= 0) {
        qDebug() << "Invalid user ID, cannot load events";
        return;
    }

    clearEvents();

    // First get the user's going events
    QSqlQuery userQuery;
    userQuery.prepare("SELECT going_events FROM users_list WHERE user_id = :userId");
    userQuery.bindValue(":userId", m_currentUserId);

    if (!userQuery.exec() || !userQuery.next()) {
        qDebug() << "Failed to query user's going events:" << userQuery.lastError().text();
        return;
    }

    QString goingEventsStr = userQuery.value(0).toString();
    QVector<int> goingEvents = Database::deserializeUserIds(goingEventsStr);


    // In loadUserEvents(), after getting goingEventsStr
    qDebug() << "Going events string:" << goingEventsStr;
    qDebug() << "Deserialized events count:" << goingEvents.size();
    qDebug() << "Deserialized events:" << goingEvents;



    if (goingEvents.isEmpty()) {
        // Add a label indicating no events
        QLabel* noEventsLabel = new QLabel("You have no upcoming events");
        noEventsLabel->setAlignment(Qt::AlignCenter);
        QFont font = noEventsLabel->font();
        font.setPointSize(12);
        noEventsLabel->setFont(font);
        m_contentLayout->addWidget(noEventsLabel);
        return;
    }

    // Create a SQL query to get event details
    QString placeholders;
    for (int i = 0; i < goingEvents.size(); ++i) {
        if (i > 0) placeholders += ",";
        placeholders += QString::number(goingEvents[i]);
    }

    QSqlQuery eventQuery;
    QString queryStr = QString("SELECT event_id, club_id, event_content, event_photo, event_code, created_timestamp "
                               "FROM events_list "
                               "WHERE event_id IN (%1) "
                               "ORDER BY created_timestamp ASC").arg(placeholders);

    if (!eventQuery.exec(queryStr)) {
        qDebug() << "Failed to query events:" << eventQuery.lastError().text();
        return;
    }

    bool firstEvent = true;

    while (eventQuery.next()) {
        int eventId = eventQuery.value(0).toInt();
        int clubId = eventQuery.value(1).toInt();
        QString eventContent = eventQuery.value(2).toString();
        QByteArray eventPhoto = eventQuery.value(3).toByteArray();
        QString eventCode = eventQuery.value(4).toString();
        int timestamp = eventQuery.value(5).toInt();

        // Convert timestamp to QDateTime
        QDateTime eventDate = QDateTime::fromSecsSinceEpoch(timestamp);

        // Get club name
        QString clubName = getClubNameById(clubId);

        // Add separator before each event except the first one
        if (!firstEvent) {
            m_contentLayout->addWidget(createSeparator());
        } else {
            firstEvent = false;
        }

        // Create and add event widget
        QWidget* eventWidget = createClubEventItem(eventId, clubName, eventContent, eventPhoto, eventCode, eventDate);
        m_contentLayout->addWidget(eventWidget);
    }
}

void MGoingPage::setupUI()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Header with title
    QWidget* headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    headerWidget->setStyleSheet("background-color: white;");

    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(12, 4, 12, 4);

    m_titleLabel = new QLabel("My Events");
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addStretch(1);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);

    // Content area with scroll support
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: #d8e8cd;");

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(15);

    // Title label for upcoming events
    QLabel* upcomingLabel = new QLabel("Upcoming Events for you");
    QFont upcomingFont;
    upcomingFont.setPointSize(16);
    upcomingFont.setBold(true);
    upcomingLabel->setFont(upcomingFont);
    upcomingLabel->setAlignment(Qt::AlignCenter);
    m_contentLayout->addWidget(upcomingLabel);
    m_contentLayout->addSpacing(20);

    // Content will be added dynamically when loadUserEvents is called
    m_contentLayout->addStretch(1);

    scrollArea->setWidget(m_contentWidget);

    // Bottom navigation bar
    m_bottomNavBar = new QWidget();
    m_bottomNavBar->setFixedHeight(60);
    m_bottomNavBar->setStyleSheet("background-color: white; border-top: 1px solid #e0e0e0;");

    m_bottomNavLayout = new QHBoxLayout(m_bottomNavBar);
    m_bottomNavLayout->setContentsMargins(10, 5, 10, 5);
    m_bottomNavLayout->setSpacing(0);

    // Create navigation buttons with icons
    // Home icon
    QPushButton* homeIcon = new QPushButton();
    QIcon homeIconImage(":/images/resources/home_logo.png");
    if (!homeIconImage.isNull()) {
        homeIcon->setIcon(homeIconImage);
        homeIcon->setIconSize(QSize(24, 24));
    } else {
        homeIcon->setText("Home");
    }
    homeIcon->setFixedSize(40, 40);
    homeIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Club icon
    QPushButton* clubIcon = new QPushButton();
    QIcon clubIconImage(":/images/resources/club_logo.png");
    if (!clubIconImage.isNull()) {
        clubIcon->setIcon(clubIconImage);
        clubIcon->setIconSize(QSize(24, 24));
    } else {
        clubIcon->setText("Club");
    }
    clubIcon->setFixedSize(40, 40);
    clubIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Event icon
    QPushButton* eventIcon = new QPushButton();
    QIcon eventIconImage(":/images/resources/event.png");
    if (!eventIconImage.isNull()) {
        eventIcon->setIcon(eventIconImage);
        eventIcon->setIconSize(QSize(24, 24));
    } else {
        eventIcon->setText("Event");
    }
    eventIcon->setFixedSize(40, 40);
    eventIcon->setStyleSheet("QPushButton { background-color: transparent; border: none; }");

    // Profile icon
    // In MHomepage::setupUI() method, replace the profile icon creation with:

    // Profile icon with user photo
    QPushButton* profileIcon = new QPushButton();
    QSqlQuery profileQuery;
    profileQuery.prepare("SELECT profile_photo FROM users_list WHERE user_id = :userId");
    profileQuery.bindValue(":userId", m_currentUserId);
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
    m_bottomNavLayout->addStretch(1);
    m_bottomNavLayout->addWidget(homeIcon);
    m_bottomNavLayout->addStretch(1);
    m_bottomNavLayout->addWidget(clubIcon);
    m_bottomNavLayout->addStretch(1);
    m_bottomNavLayout->addWidget(eventIcon);
    m_bottomNavLayout->addStretch(1);
    m_bottomNavLayout->addWidget(profileIcon);
    m_bottomNavLayout->addStretch(1);

    // Connect signals to slots
    connect(homeIcon, &QPushButton::clicked, this, &MGoingPage::homeClicked);
    connect(clubIcon, &QPushButton::clicked, this, &MGoingPage::clubClicked);
    connect(eventIcon, &QPushButton::clicked, this, &MGoingPage::eventClicked);
    connect(profileIcon, &QPushButton::clicked, this, &MGoingPage::profileClicked);

    // Add all to main layout
    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(scrollArea, 1);
    m_mainLayout->addWidget(m_bottomNavBar);

    // Initialize the toggle buttons and stacked widget as empty but instantiated
    m_joinedButton = new QPushButton(this);
    m_joinedButton->hide();
    m_pendingButton = new QPushButton(this);
    m_pendingButton->hide();
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->hide();

    // Set fixed width to match window size
    setFixedWidth(350);
    setFixedHeight(650);

    qDebug() << "MGoingPage setupUI completed successfully";
}

QPixmap MGoingPage::createCircularPixmap(const QPixmap& pixmap, int size)
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

QWidget* MGoingPage::createClubEventItem(int eventId, const QString& clubName, const QString& eventContent, const QByteArray& eventPhoto, const QString& eventCode, QDateTime eventDate)
{
    QWidget* itemWidget = new QWidget();
    QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
    itemLayout->setContentsMargins(0, 10, 0, 10);
    itemLayout->setSpacing(10);

    // Create top row with club logo and info
    QWidget* topRowWidget = new QWidget();
    QHBoxLayout* topRowLayout = new QHBoxLayout(topRowWidget);
    topRowLayout->setContentsMargins(0, 0, 0, 0);

    // Club logo
    QLabel* logoLabel = new QLabel();
    logoLabel->setFixedSize(80, 80);
    logoLabel->setStyleSheet("background-color: #1f3864; border-radius: 5px;");

    // Try to load the image from the event_photo
    if (!eventPhoto.isEmpty()) {
        QPixmap logoPixmap;
        if (logoPixmap.loadFromData(eventPhoto)) {
            logoLabel->setPixmap(logoPixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // If no image is set, create a placeholder
    if (logoLabel->pixmap().isNull()) {
        QPixmap placeholder(80, 80);
        placeholder.fill(Qt::transparent);
        QPainter painter(&placeholder);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(QColor("#1f3864")));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, 80, 80);

        // Draw 'E' for event
        painter.setPen(QPen(Qt::white));
        QFont font = painter.font();
        font.setPointSize(30);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "E");

        logoLabel->setPixmap(placeholder);
    }

    topRowLayout->addWidget(logoLabel);

    // Club info
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(10, 0, 0, 0);
    infoLayout->setSpacing(2);

    QLabel* nameLabel = new QLabel(clubName);
    QFont nameFont;
    nameFont.setPointSize(14);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    QLabel* eventContentLabel = new QLabel(eventContent);
    eventContentLabel->setWordWrap(true);

    QLabel* dateLabel = new QLabel("Date: " + eventDate.toString("dd/MM/yyyy hh:mm AP"));

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(eventContentLabel);
    infoLayout->addWidget(dateLabel);
    infoLayout->addStretch();

    topRowLayout->addWidget(infoWidget, 1);

    // Add top row to main layout
    itemLayout->addWidget(topRowWidget);

    // Create bottom row with input field and enter button
    QWidget* bottomRowWidget = new QWidget();
    QHBoxLayout* bottomRowLayout = new QHBoxLayout(bottomRowWidget);
    bottomRowLayout->setContentsMargins(0, 0, 0, 0);
    bottomRowLayout->setSpacing(10);

    // Input field
    QLineEdit* codeInput = new QLineEdit();
    codeInput->setPlaceholderText("Enter event code here...");
    codeInput->setFixedHeight(40);
    codeInput->setStyleSheet(
        "QLineEdit {"
        "    background-color: white;"
        "    border-radius: 20px;"
        "    padding-left: 10px;"
        "    padding-right: 10px;"
        "}"
        );

    // Enter button
    QPushButton* enterButton = new QPushButton("Enter");
    enterButton->setFixedSize(100, 40);
    enterButton->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    border-radius: 20px;"
        "    font-weight: bold;"
        "    color: #43a047;"
        "}"
        );

    bottomRowLayout->addWidget(codeInput, 1);
    bottomRowLayout->addWidget(enterButton);

    // Add bottom row to main layout
    itemLayout->addWidget(bottomRowWidget);

    // Store the event ID associated with the input field and button
    m_eventInputMap[codeInput] = eventId;
    m_eventButtonMap[enterButton] = eventId;

    // Connect the button to the verification function
    connect(enterButton, &QPushButton::clicked, this, &MGoingPage::verifyEventCode);

    return itemWidget;
}

void MGoingPage::verifyEventCode()
{
    // Get the button that was clicked
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button || !m_eventButtonMap.contains(button)) {
        qDebug() << "Invalid button or event ID mapping";
        return;
    }

    int eventId = m_eventButtonMap[button];

    // Find the associated input field
    QLineEdit* inputField = nullptr;
    for (auto it = m_eventInputMap.begin(); it != m_eventInputMap.end(); ++it) {
        if (it.value() == eventId) {
            inputField = it.key();
            break;
        }
    }

    if (!inputField) {
        qDebug() << "No input field found for event ID:" << eventId;
        return;
    }

    QString enteredCode = inputField->text().trimmed();
    if (enteredCode.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter an event code.");
        return;
    }

    // Query the database to get the correct event code
    QSqlQuery query;
    query.prepare("SELECT event_code FROM events_list WHERE event_id = :eventId");
    query.bindValue(":eventId", eventId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to query event code:" << query.lastError().text();
        return;
    }

    QString correctCode = query.value(0).toString();

    if (enteredCode == correctCode) {
        // Update user points (+10 points)
        if (updateUserPoints(m_currentUserId, 10)) {
            // Remove the event from the user's going events list
            if (removeEventFromUserGoing(m_currentUserId, eventId)) {
                QMessageBox::information(this, "Success", "Event Participation Successful!\nYou gained 10 points!");

                // Refresh the events list
                loadUserEvents();
            }
        }
    } else {
        QMessageBox::warning(this, "Invalid Code", "The entered code is incorrect. Please try again.");
    }
}

bool MGoingPage::updateUserPoints(int userId, int pointsToAdd)
{
    QSqlQuery query;
    query.prepare("UPDATE users_list SET points = points + :points WHERE user_id = :userId");
    query.bindValue(":points", pointsToAdd);
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Failed to update user points:" << query.lastError().text();
        return false;
    }

    return true;
}

bool MGoingPage::removeEventFromUserGoing(int userId, int eventId)
{
    // First get the current going events list
    QSqlQuery query;
    query.prepare("SELECT going_events FROM users_list WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to query user's going events:" << query.lastError().text();
        return false;
    }

    QString goingEventsStr = query.value(0).toString();
    QVector<int> goingEvents = Database::deserializeUserIds(goingEventsStr);



    // Remove the event ID from the list
    goingEvents.removeAll(eventId);

    // Update the user's going events list
    QString newGoingEventsStr = Database::serializeUserIds(goingEvents);

    query.prepare("UPDATE users_list SET going_events = :goingEvents WHERE user_id = :userId");
    query.bindValue(":goingEvents", newGoingEventsStr);
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Failed to update user's going events:" << query.lastError().text();
        return false;
    }

    return true;
}

QString MGoingPage::getClubNameById(int clubId)
{
    QSqlQuery query;
    query.prepare("SELECT club_name FROM clubs_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return "Unknown Club";
}

void MGoingPage::clearEvents()
{
    // Clear the mappings
    m_eventButtonMap.clear();
    m_eventInputMap.clear();

    // Remove all widgets from the content layout except the title and spacing
    while (m_contentLayout->count() > 3) {
        QLayoutItem* item = m_contentLayout->takeAt(3);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

QWidget* MGoingPage::createSeparator()
{
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setLineWidth(1);
    separator->setStyleSheet("background-color: #888888;");
    return separator;
}

void MGoingPage::showJoinedView()
{
    // Not used in our implementation, but kept for compatibility
    qDebug() << "showJoinedView called";
}

void MGoingPage::showPendingView()
{
    // Not used in our implementation, but kept for compatibility
    qDebug() << "showPendingView called";
}

void MGoingPage::homeClicked()
{
    qDebug() << "Home button clicked";
    emit navigateToHome();
}

void MGoingPage::clubClicked()
{
    qDebug() << "Club button clicked";
    emit navigateToClub();
}

void MGoingPage::eventClicked()
{
    qDebug() << "Event button clicked";
    // Already on events page, just log it
    qDebug() << "Already on events page";
}

void MGoingPage::profileClicked()
{
    qDebug() << "Profile button clicked";
    emit navigateToProfile();
}
