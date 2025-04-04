#include "lnoti.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QPainterPath>
#include <QStyle>
#include "database.h" // Include Database class for helper functions

// Helper function to create a circular user avatar
QPixmap createCircularAvatar(int size, const QColor &color = Qt::white) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(color));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);

    return pixmap;
}


QPixmap LNoti::getUserProfilePhoto(int userId) {
    QSqlQuery query;
    query.prepare("SELECT profile_photo FROM users_list WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec() && query.next()) {
        QByteArray photoData = query.value(0).toByteArray();
        if (!photoData.isEmpty()) {
            QPixmap photo;
            if (photo.loadFromData(photoData)) {
                // Create a circular mask for the photo
                photo = photo.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                return createCircularPixmap(photo);
            }
        }
    }

    // Return default circular avatar if no photo found
    return createCircularAvatar(40, Qt::white);
}
// Add this helper method to create circular pixmap from a square one
QPixmap LNoti::createCircularPixmap(const QPixmap &srcPixmap) {
    QPixmap targetPixmap(srcPixmap.size());
    targetPixmap.fill(Qt::transparent);

    QPainter painter(&targetPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(0, 0, srcPixmap.width(), srcPixmap.height());
    painter.setClipPath(path);

    painter.drawPixmap(0, 0, srcPixmap);

    return targetPixmap;
}


LNoti::LNoti(int clubId, QWidget *parent)
    : QWidget(parent), m_clubId(clubId)
{
    setupUI();
    loadNotificationsFromDatabase();
}



LNoti::~LNoti()
{
}

void LNoti::prepareForDestruction()
{
    // Disconnect all signals from this object
    this->disconnect();

    // Disconnect specific signals if any
    // For example, any back button connections
    // Note: You'll need to make your back button a member variable for this
}

void LNoti::setupUI()
{
    // Set background color to white and fix window size
    this->setStyleSheet("background-color: white;");
    this->setFixedSize(350, 650);

    // Main layout setup
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setContentsMargins(3, 30, 3, 20);
    m_mainLayout->setSpacing(15);

    // Create a combined header widget with back button and title
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(5, 5, 5, 5);

    // Back button using resource PNG - defined as local variable
    QPushButton* backButton = new QPushButton("", this);
    backButton->setIcon(QIcon(":/images/resources/back.png"));
    backButton->setIconSize(QSize(20, 20));
    backButton->setFlat(true);
    backButton->setStyleSheet("QPushButton { border: none; }");
    connect(backButton, &QPushButton::clicked, this, &LNoti::onBackClicked);

    // Title
    QLabel* titleLabel = new QLabel("Clubsphere", this);
    titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);

    // Add widgets to header layout
    headerLayout->addWidget(backButton, 0);
    headerLayout->addWidget(titleLabel, 1);

    // Add header to main layout
    m_mainLayout->addWidget(headerWidget);

    // Create notifications area
    setupNotificationsArea();
}

void LNoti::setupNotificationsArea()
{
    // Create the scroll area for notifications
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Remove minimum height constraint that was causing issues
    // m_scrollArea->setMinimumHeight(480); // REMOVED THIS LINE

    // Create container widget for notifications
    m_scrollContent = new QWidget(m_scrollArea);
    m_scrollContent->setStyleSheet(
        "background-color: #D9E9D8;"
        "border-radius: 12px;"
        );
    m_scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // ADDED

    // Create layout for notifications
    m_notificationsLayout = new QVBoxLayout(m_scrollContent);
    m_notificationsLayout->setContentsMargins(5, 5, 5, 10);  // Tight margins
    m_notificationsLayout->setSpacing(4);  // Reduced spacing

    // Add a title to the notifications section
    QLabel *notificationsLabel = new QLabel("Club Notifications", m_scrollContent);
    QFont notificationsFont;
    notificationsFont.setBold(true);
    notificationsFont.setPointSize(14);
    notificationsLabel->setFont(notificationsFont);
    notificationsLabel->setAlignment(Qt::AlignCenter);
    notificationsLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // ADDED
    m_notificationsLayout->addWidget(notificationsLabel);

    // Add stretch to push content to top
    m_notificationsLayout->addStretch(1); // ADDED

    // Set the container as the widget for the scroll area
    m_scrollArea->setWidget(m_scrollContent);
    m_scrollArea->setWidgetResizable(true); // ENSURE THIS IS SET

    // Add scroll area to main layout
    QHBoxLayout *scrollAreaLayout = new QHBoxLayout();
    scrollAreaLayout->setContentsMargins(10, 0, 10, 0);
    scrollAreaLayout->addWidget(m_scrollArea);
    m_mainLayout->addLayout(scrollAreaLayout, 1); // ADDED stretch factor

    // Force immediate update
    m_scrollContent->adjustSize();
    updateGeometry();
}

void LNoti::loadNotificationsFromDatabase()
{
    if (m_clubId <= 0) {
        qDebug() << "Invalid club ID for loading notifications";
        return;
    }

    // Clear existing notifications
    m_notifications.clear();
    for (QWidget* widget : m_notificationWidgets) {
        if (widget) {
            delete widget;
        }
    }
    m_notificationWidgets.clear();

    // Query the database for notifications related to this club
    QSqlQuery query;
    query.prepare("SELECT club_id, is_request, content, user_id, timestamp "
                  "FROM notification_lists "
                  "WHERE club_id = :clubId "
                  "ORDER BY timestamp DESC"); // Most recent first
    query.bindValue(":clubId", m_clubId);

    if (!query.exec()) {
        qDebug() << "Error loading notifications:" << query.lastError().text();
        return;
    }

    // Process results
    while (query.next()) {
        int clubId = query.value(0).toInt();
        bool isRequest = query.value(1).toBool();
        QString content = query.value(2).toString();
        int userId = query.value(3).toInt();
        qint64 timestamp = query.value(4).toLongLong();

        // Get username from userId
        QString username = Database::getUserNameById(userId);
        if (username.isEmpty()) {
            // Try to get from student names list
            username = Database::getStudentNameById(userId);
            if (username.isEmpty()) {
                username = QString("User %1").arg(userId);
            }
        }

        // Create notification object
        Notification notification(clubId, userId, content, isRequest, timestamp, username);
        m_notifications.append(notification);
    }

    // Add notification widgets
    for (int i = 0; i < m_notifications.size(); ++i) {
        QWidget *notificationWidget = createNotificationWidget(m_notifications[i], i);
        m_notificationWidgets.append(notificationWidget);
        m_notificationsLayout->addWidget(notificationWidget);

        // Add separator except after the last item
        if (i < m_notifications.size() - 1) {
            QFrame *line = new QFrame(m_scrollContent);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("background-color: #888888;");
            line->setFixedHeight(1);
            m_notificationsLayout->addWidget(line);
        }
    }

    // Add "No notifications" message if empty
    if (m_notifications.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No notifications available", m_scrollContent);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #555555; margin: 20px;");
        m_notificationsLayout->addWidget(emptyLabel);
    }
}

QWidget* LNoti::createHeaderWidget()
{
    QWidget *headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    headerWidget->setStyleSheet("background-color: white;");

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 0, 10, 0);

    // Use resource PNG for back button
    QPushButton *backButton = new QPushButton("", this);
    backButton->setIcon(QIcon(":/images/resources/back.png"));
    backButton->setIconSize(QSize(20, 20));
    backButton->setFlat(true);
    backButton->setStyleSheet("QPushButton { border: none; }");
    connect(backButton, &QPushButton::clicked, this, &LNoti::onBackClicked);

    // Empty widget for spacing
    QWidget* spacer = new QWidget();
    spacer->setFixedWidth(30);

    headerLayout->addWidget(backButton);
    headerLayout->addStretch(1);
    headerLayout->addWidget(spacer);

    return headerWidget;
}

QWidget* LNoti::createNotificationWidget(const Notification &notification, int index)
{
    // Main layout for notifications
    QHBoxLayout *notificationLayout = new QHBoxLayout();
    notificationLayout->setContentsMargins(10, 8, 10, 8);  // Increased side margins
    notificationLayout->setSpacing(12);  // Slightly more spacing between elements

    // User icon
    QLabel *userIcon = new QLabel();
    userIcon->setFixedSize(40, 40);
    userIcon->setStyleSheet("background-color: transparent; border-radius: 20px;");
    QPixmap userPhoto = getUserProfilePhoto(notification.userId);
    userIcon->setPixmap(userPhoto);

    // Notification details
    QVBoxLayout *notificationDetailsLayout = new QVBoxLayout();
    notificationDetailsLayout->setSpacing(4);
    notificationDetailsLayout->setContentsMargins(0, 0, 0, 0);  // Remove internal margins

    // Message label with improved text display
    QLabel *messageLabel = new QLabel();
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);  // Allow text selection
    messageLabel->setStyleSheet("margin-bottom: 3px; color: #333333;");
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QFont messageFont;
    messageFont.setPointSize(12);
    messageLabel->setFont(messageFont);

    // Format message
    QString messageText;
    if (notification.content == "join" && notification.isRequest) {
        messageText = QString("<b>%1</b> sent a request to join the club").arg(notification.username);
    } else if (notification.content == "join" && !notification.isRequest) {
        messageText = QString("<b>%1</b> joined the club").arg(notification.username);
    } else if (notification.content == "leave") {
        messageText = QString("<b>%1</b> left the club").arg(notification.username);
    } else if (notification.content == "reject") {
        messageText = QString("<b>%1</b> was rejected from joining the club").arg(notification.username);
    }
    messageLabel->setText(messageText);

    // Time label
    QLabel *timeLabel = new QLabel(formatTimestamp(notification.timestamp));
    timeLabel->setStyleSheet("color: #555555; margin-top: 2px;");
    QFont timeFont;
    timeFont.setPointSize(10);
    timeLabel->setFont(timeFont);

    notificationDetailsLayout->addWidget(messageLabel);
    notificationDetailsLayout->addWidget(timeLabel);

    // Add to main layout
    notificationLayout->addWidget(userIcon);
    notificationLayout->addLayout(notificationDetailsLayout, 1);  // Allow details to expand

    // Action buttons for join requests
    if (notification.content == "join" && notification.isRequest) {
        QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
        actionButtonsLayout->setSpacing(6);  // Reduced spacing between buttons
        actionButtonsLayout->setContentsMargins(0, 0, 0, 0);

        // Approve button
        QPushButton *approveButton = new QPushButton();
        approveButton->setFixedSize(30, 30);  // Slightly smaller buttons
        approveButton->setIcon(QIcon(":/images/resources/checkmark.png"));
        approveButton->setIconSize(QSize(16, 16));
        approveButton->setStyleSheet("QPushButton { background-color: white; border-radius: 15px; border: 1px solid #CCCCCC; }");
        if (approveButton->icon().isNull()) approveButton->setText("✓");
        connect(approveButton, &QPushButton::clicked, [this, index]() { onAcceptClicked(index); });

        // Reject button
        QPushButton *rejectButton = new QPushButton();
        rejectButton->setFixedSize(30, 30);
        rejectButton->setIcon(QIcon(":/images/resources/xmark.png"));
        rejectButton->setIconSize(QSize(16, 16));
        rejectButton->setStyleSheet("QPushButton { background-color: white; border-radius: 15px; border: 1px solid #CCCCCC; }");
        if (rejectButton->icon().isNull()) rejectButton->setText("✗");
        connect(rejectButton, &QPushButton::clicked, [this, index]() { onRejectClicked(index); });

        actionButtonsLayout->addWidget(approveButton);
        actionButtonsLayout->addWidget(rejectButton);
        notificationLayout->addLayout(actionButtonsLayout);
    }

    // Create the widget
    QWidget *widget = new QWidget();
    widget->setLayout(notificationLayout);
    widget->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.7);"
        "border-radius: 8px;"
        "padding: 2px;"
        );
    widget->setMinimumWidth(300);  // Ensure minimum width for proper text display

    return widget;
}

void LNoti::onBackClicked()
{
    emit navigateBack();
}

void LNoti::onAcceptClicked(int index) {
    if (index >= 0 && index < m_notifications.size()) {
        Notification notification = m_notifications[index];
        if (notification.content == "join" && notification.isRequest) {
            // Update notification status in database
            QSqlQuery query;
            query.prepare("UPDATE notification_lists SET is_request = 0 "
                          "WHERE club_id = :clubId AND user_id = :userId AND content = 'join' AND is_request = 1");
            query.bindValue(":clubId", notification.clubId);
            query.bindValue(":userId", notification.userId);
            if (!query.exec()) {
                qDebug() << "Error updating notification:" << query.lastError().text();
                return;
            }

            // Get current club members
            query.prepare("SELECT club_members FROM clubs_list WHERE club_id = :clubId");
            query.bindValue(":clubId", notification.clubId);
            if (!query.exec() || !query.next()) {
                qDebug() << "Error retrieving club members:" << query.lastError().text();
                return;
            }

            // Deserialize current members
            QVector<int> members;
            QString membersStr = query.value("club_members").toString();
            if (!membersStr.isEmpty()) {
                QStringList membersList = membersStr.split(',');
                for (const QString& member : membersList) {
                    members.append(member.toInt());
                }
            }

            // Add new member if not already a member
            if (!members.contains(notification.userId)) {
                members.append(notification.userId);

                // Serialize back to string
                QStringList memberStrList;
                for (int memberId : members) {
                    memberStrList.append(QString::number(memberId));
                }
                QString newMembersStr = memberStrList.join(',');

                // Update club_members in database
                query.prepare("UPDATE clubs_list SET club_members = :members WHERE club_id = :clubId");
                query.bindValue(":members", newMembersStr);
                query.bindValue(":clubId", notification.clubId);
                if (!query.exec()) {
                    qDebug() << "Error updating club members:" << query.lastError().text();
                    return;
                }
            }

            // Update user's club status
            updateUserClubStatus(notification.userId, notification.clubId, true);

            // Immediately update the notification in our local list
            m_notifications[index].isRequest = false;

            // Refresh the specific notification widget instead of all
            clearNotifications();
            loadNotificationsFromDatabase();
        }
    }
}


void LNoti::onRejectClicked(int index)
{
    if (index >= 0 && index < m_notifications.size()) {
        Notification &notification = m_notifications[index];

        if (notification.content == "join" && notification.isRequest) {
            // Update notification status in database
            QSqlQuery query;
            query.prepare("UPDATE notification_lists SET is_request = 0, content = 'reject' "
                          "WHERE club_id = :clubId AND user_id = :userId AND content = 'join' AND is_request = 1");
            query.bindValue(":clubId", notification.clubId);
            query.bindValue(":userId", notification.userId);

            if (!query.exec()) {
                qDebug() << "Error updating notification:" << query.lastError().text();
                return;
            }

            // Update user's club status
            updateUserClubStatus(notification.userId, notification.clubId, false);

            // Refresh notification list
            clearNotifications();
            loadNotificationsFromDatabase();
        }
    }
}

void LNoti::updateUserClubStatus(int userId, int clubId, bool accepted)
{
    // Get current pending_clubs and joined_clubs for user
    QSqlQuery query;
    query.prepare("SELECT pending_clubs, joined_clubs FROM users_list WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    QString pendingClubs, joinedClubs;

    if (query.exec() && query.next()) {
        pendingClubs = query.value(0).toString();
        joinedClubs = query.value(1).toString();
    } else {
        qDebug() << "Error getting user clubs:" << query.lastError().text();
        return;
    }

    // Convert to vector
    QVector<int> pendingClubsVec = Database::deserializeUserIds(pendingClubs);
    QVector<int> joinedClubsVec = Database::deserializeUserIds(joinedClubs);

    // Remove from pending clubs
    pendingClubsVec.removeAll(clubId);

    // Add to joined clubs if accepted
    if (accepted && !joinedClubsVec.contains(clubId)) {
        joinedClubsVec.append(clubId);
    }

    // Convert back to string
    QString newPendingClubs = Database::serializeUserIds(pendingClubsVec);
    QString newJoinedClubs = Database::serializeUserIds(joinedClubsVec);

    // Update user record
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users_list SET pending_clubs = :pendingClubs, joined_clubs = :joinedClubs "
                        "WHERE user_id = :userId");
    updateQuery.bindValue(":pendingClubs", newPendingClubs);
    updateQuery.bindValue(":joinedClubs", newJoinedClubs);
    updateQuery.bindValue(":userId", userId);

    if (!updateQuery.exec()) {
        qDebug() << "Error updating user clubs:" << updateQuery.lastError().text();
    }
}
void LNoti::clearNotifications()
{

    // Clear existing notification widgets
    QLayoutItem* item;
    while ((item = m_notificationsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_notificationWidgets.clear();

}

QString LNoti::formatTimestamp(qint64 timestamp)
{
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
    QDateTime now = QDateTime::currentDateTime();

    // Different formats based on time difference
    if (dateTime.date() == now.date()) {
        // Today - show time
        return "Today at " + dateTime.toString("h:mm ap");
    } else if (dateTime.daysTo(now) == 1) {
        // Yesterday
        return "Yesterday at " + dateTime.toString("h:mm ap");
    } else if (dateTime.daysTo(now) < 7) {
        // Within a week
        return dateTime.toString("dddd h:mm ap");
    } else {
        // More than a week ago
        return dateTime.toString("MMM d, yyyy h:mm ap");
    }
}
