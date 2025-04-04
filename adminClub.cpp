#include "adminClub.h"
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QPainter>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

AdminClub::AdminClub(QWidget *parent) : QWidget(parent)
{
    setupUI();
    setupClubList();
    setupNavigation();
    setupSearchFunctionality();
}

AdminClub::~AdminClub()
{
}

void AdminClub::setupUI()
{
    // Set background color to white and keep window size
    this->setStyleSheet("background-color: white;");
    this->setFixedSize(350, 650);  // Keep same size

    // Main layout setup with reduced margins and spacing
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(15, 15, 15, 10);  // Further reduced margins
    mainLayout->setSpacing(8);  // Further reduced spacing

    // Title with smaller font
    titleLabel = new QLabel("Clubsphere", this);
    titleLabel->setFont(QFont("Arial", 14, QFont::Bold));  // Reduced font size
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Search bar with reduced height
    searchFrame = new QFrame(this);
    searchFrame->setFrameShape(QFrame::StyledPanel);
    searchFrame->setStyleSheet("QFrame { background-color: #F0F0F0; border-radius: 10px; }");  // Reduced radius

    QHBoxLayout *searchLayout = new QHBoxLayout(searchFrame);
    searchLayout->setContentsMargins(10, 5, 10, 5);  // Further reduced padding

    QLabel *searchIcon = new QLabel(this);
    searchIcon->setPixmap(QIcon(":/images/resources/search_logo.png").pixmap(16, 16));  // Smaller icon

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search");
    searchEdit->setStyleSheet("QLineEdit { border: none; background-color: transparent; }");

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(searchEdit);
    searchFrame->setFixedHeight(35);  // Explicitly set smaller height

    mainLayout->addWidget(searchFrame);

    // Clubs label and add button - reduced size
    clubsHeaderLayout = new QHBoxLayout();
    clubsHeaderLayout->setSpacing(5);  // Reduced spacing

    clubsLabel = new QLabel("Clubs:", this);
    clubsLabel->setFont(QFont("Arial", 14, QFont::Bold));  // Smaller font

    addClubButton = new QPushButton(this);
    addClubButton->setIcon(QIcon(":/images/resources/plus_logo.png"));
    addClubButton->setIconSize(QSize(12, 12));  // Smaller icon
    addClubButton->setFixedSize(30, 30);  // Smaller button
    addClubButton->setStyleSheet("QPushButton { background-color: #D9E9D8; border-radius: 15px; }");

    clubsHeaderLayout->addWidget(clubsLabel);
    clubsHeaderLayout->addStretch();
    clubsHeaderLayout->addWidget(addClubButton);

    mainLayout->addLayout(clubsHeaderLayout);

    // Scroll area for club list - MODIFIED
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // Changed to AsNeeded
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Added expanding size policy

    scrollContent = new QWidget(scrollArea);
    scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Added expanding size policy
    clubsLayout = new QVBoxLayout(scrollContent);
    clubsLayout->setAlignment(Qt::AlignTop);
    clubsLayout->setContentsMargins(0, 0, 0, 0);
    clubsLayout->setSpacing(8);  // Further reduced spacing

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1); // Added stretch factor 1 to ensure it expands

    // Connect add club button
    connect(addClubButton, &QPushButton::clicked, this, &AdminClub::onAddClubClicked);
}

void AdminClub::setupClubList()
{
    // Load clubs from database
    refreshClubList();

    // Removed the stretch that was creating empty space
    // mainLayout->addStretch();  // This line was removed
}

void AdminClub::createClubCard(const QString &name, int clubId, int members, const QString &leader)
{
    QFrame *clubFrame = new QFrame(this);
    clubFrame->setFrameShape(QFrame::NoFrame);
    clubFrame->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *cardLayout = new QHBoxLayout(clubFrame);
    cardLayout->setContentsMargins(0, 4, 0, 4);  // Further reduced padding

    // Club image - smaller size
    QLabel *clubImage = new QLabel(this);
    QPixmap defaultClubImage(":/images/resources/default_club.png");
    if (defaultClubImage.isNull()) {
        // Create a placeholder if image is not found
        defaultClubImage = QPixmap(60, 60);  // Smaller image
        defaultClubImage.fill(Qt::lightGray);

        // Draw placeholder image icon
        QPainter painter(&defaultClubImage);
        painter.setPen(Qt::darkGray);
        painter.drawRect(15, 15, 40, 30);
        painter.drawLine(15, 20, 30, 40);
        painter.drawEllipse(40, 20, 8, 8);
    }

    clubImage->setPixmap(defaultClubImage.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    clubImage->setFixedSize(60, 60);  // Smaller image
    clubImage->setStyleSheet("background-color: #E0E0E0;");

    // Club info with reduced spacing
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(1);  // Minimal spacing

    // Modified: Display club name with ID in smaller font
    QString nameWithId = name + " (ID: " + QString::number(clubId) + ")";
    QLabel *nameLabel = new QLabel(nameWithId, this);
    nameLabel->setFont(QFont("Arial", 12, QFont::Bold));  // Smaller font

    QLabel *membersLabel = new QLabel(QString::number(members) + " members", this);
    membersLabel->setFont(QFont("Arial", 9));  // Smaller font

    QLabel *leaderLabel = new QLabel("Leader: " + leader, this);
    leaderLabel->setFont(QFont("Arial", 9));  // Smaller font

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(membersLabel);
    infoLayout->addWidget(leaderLabel);

    // Action buttons - smaller size
    QVBoxLayout *actionsLayout = new QVBoxLayout();
    actionsLayout->setSpacing(4);  // Reduced spacing

    QPushButton *deleteButton = new QPushButton("", this);
    deleteButton->setIcon(QIcon(":/images/resources/trash.png"));
    deleteButton->setIconSize(QSize(16, 16));  // Smaller icon
    deleteButton->setFixedSize(28, 28);  // Smaller button
    deleteButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 8px; padding: 4px; }");

    actionsLayout->addWidget(deleteButton);

    cardLayout->addWidget(clubImage);
    cardLayout->addLayout(infoLayout, 1);
    cardLayout->addLayout(actionsLayout);

    clubsLayout->addWidget(clubFrame);

    // Add thinner separator line except for the last item
    if (clubsLayout->count() < 4) {
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("background-color: #E0E0E0;");
        line->setMaximumHeight(1);
        clubsLayout->addWidget(line);
    }
}

void AdminClub::setupNavigation()
{
    // Bottom navigation bar
    navigationFrame = new QFrame(this);
    navigationFrame->setFrameShape(QFrame::NoFrame);
    navigationFrame->setLineWidth(0);
    navigationFrame->setStyleSheet("QFrame { background-color: white; }");

    QHBoxLayout *navLayout = new QHBoxLayout(navigationFrame);
    navLayout->setContentsMargins(15, 10, 15, 10);  // Match AdminMember navigation margins
    navLayout->setSpacing(45);  // Match AdminMember navigation spacing

    // Create navigation icons
    homeButton = new QPushButton("", this);
    homeButton->setIcon(QIcon(":/images/resources/home_logo.png"));
    homeButton->setIconSize(QSize(20, 20));
    homeButton->setStyleSheet("QPushButton { border: none; }");

    profileButton = new QPushButton("", this);
    profileButton->setIcon(QIcon(":/images/resources/member_logo.png"));
    profileButton->setIconSize(QSize(20, 20));
    profileButton->setStyleSheet("QPushButton { border: none; }");

    groupsButton = new QPushButton("", this);
    groupsButton->setIcon(QIcon(":/images/resources/club_logo.png"));
    groupsButton->setIconSize(QSize(30, 30));
    groupsButton->setStyleSheet("QPushButton { border: none; background-color: #D9E9D8; border-radius: 5px; padding: 5px; }");

    navLayout->addWidget(homeButton);
    navLayout->addWidget(profileButton);
    navLayout->addWidget(groupsButton);

    mainLayout->addWidget(navigationFrame);

    // Connect navigation buttons
    connect(homeButton, &QPushButton::clicked, this, &AdminClub::onHomeButtonClicked);
    connect(profileButton, &QPushButton::clicked, this, &AdminClub::onProfileButtonClicked);
    connect(groupsButton, &QPushButton::clicked, this, &AdminClub::onGroupsButtonClicked);
}

void AdminClub::onHomeButtonClicked()
{
    qDebug() << "Home button clicked in AdminClub";
    emit navigateToHome();
}

void AdminClub::onProfileButtonClicked()
{
    qDebug() << "Profile button clicked in AdminClub";
    emit navigateToMembers();
}

void AdminClub::onGroupsButtonClicked()
{
    qDebug() << "Groups button clicked in AdminClub - already on this page";
    // No navigation needed as we're already on this page
}

void AdminClub::onAddClubClicked()
{
    qDebug() << "Add club button clicked in AdminClub";

    AddClubDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Dialog was accepted, refresh club list
        refreshClubList();
    }
}

void AdminClub::refreshClubList()
{
    // Clear existing club cards
    QLayoutItem *item;
    while ((item = clubsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Load all clubs from database
    QVector<Club> clubs = Club::loadAllClubs();

    // Add club cards
    for (const Club& club : clubs) {
        createClubCard(club);
    }

    // If no clubs, show a message
    if (clubs.isEmpty()) {
        QLabel *noClubsLabel = new QLabel("No clubs found. Click the + button to create one.", this);
        noClubsLabel->setAlignment(Qt::AlignCenter);
        clubsLayout->addWidget(noClubsLabel);
    }
}

void AdminClub::createClubCard(const Club& club)
{
    QFrame *clubFrame = new QFrame(this);
    clubFrame->setFrameShape(QFrame::NoFrame);
    clubFrame->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *cardLayout = new QHBoxLayout(clubFrame);
    cardLayout->setContentsMargins(0, 4, 0, 4);  // Further reduced padding

    // Club image - smaller size
    QLabel *clubImage = new QLabel(this);
    QPixmap clubPhotoPixmap;

    if (!club.getPhoto().isEmpty()) {
        clubPhotoPixmap.loadFromData(club.getPhoto());
    } else {
        clubPhotoPixmap = QPixmap(":/images/resources/default_club.png");
    }

    if (clubPhotoPixmap.isNull()) {
        // Create a placeholder if image is not found
        clubPhotoPixmap = QPixmap(60, 60);  // Smaller image
        clubPhotoPixmap.fill(Qt::lightGray);

        // Draw placeholder image icon
        QPainter painter(&clubPhotoPixmap);
        painter.setPen(Qt::darkGray);
        painter.drawRect(15, 15, 40, 30);
        painter.drawLine(15, 20, 30, 40);
        painter.drawEllipse(40, 20, 8, 8);
    }

    clubImage->setPixmap(clubPhotoPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // Smaller image
    clubImage->setFixedSize(60, 60);  // Smaller image size
    clubImage->setStyleSheet("background-color: white;");

    // Club info with reduced spacing
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(1);  // Minimal spacing

    // Club name with ID in smaller font
    QString nameWithId = club.getName() + " (ID: " + QString::number(club.getId()) + ")";
    QLabel *nameLabel = new QLabel(nameWithId, this);
    nameLabel->setFont(QFont("Arial", 12, QFont::Bold));  // Smaller font

    QLabel *membersLabel = new QLabel(QString::number(club.getMemberCount()) + " members", this);
    membersLabel->setFont(QFont("Arial", 9));  // Smaller font

    // Leader info with smaller font
    QLabel *leaderLabel = new QLabel("Leader ID: " + QString::number(club.getLeaderId()), this);
    leaderLabel->setFont(QFont("Arial", 9));  // Smaller font

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(membersLabel);
    infoLayout->addWidget(leaderLabel);

    // Action buttons - smaller size
    QVBoxLayout *actionsLayout = new QVBoxLayout();
    actionsLayout->setSpacing(4);  // Reduced spacing

    QPushButton *deleteButton = new QPushButton("", this);
    deleteButton->setIcon(QIcon(":/images/resources/trash.png"));
    deleteButton->setIconSize(QSize(16, 16));  // Smaller icon
    deleteButton->setFixedSize(28, 28);  // Smaller button
    deleteButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 8px; padding: 4px; }");

    // Connect delete button
    // Replace the current delete button connection in the createClubCard function with this:

    connect(deleteButton, &QPushButton::clicked, [this, clubId = club.getId(), clubName = club.getName()]() {
        qDebug() << "Delete clicked for club ID:" << clubId;

        // Ask for confirmation
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                                  QString("Are you sure you want to delete the club '%1'?\nThis will remove the club from all members' lists.").arg(clubName),
                                                                  QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // Call the deleteClub function
            deleteClub(clubId);

            // Refresh the club list after deletion
            refreshClubList();

            QMessageBox::information(this, "Success", QString("Club '%1' has been deleted successfully.").arg(clubName));
        }
    });


    actionsLayout->addWidget(deleteButton);

    cardLayout->addWidget(clubImage);
    cardLayout->addLayout(infoLayout, 1);
    cardLayout->addLayout(actionsLayout);

    clubsLayout->addWidget(clubFrame);

    // Add thinner separator line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #E0E0E0;");
    line->setMaximumHeight(1);
    clubsLayout->addWidget(line);
}



// Implementation in adminClub.cpp
void AdminClub::deleteClub(int clubId)
{
    QSqlQuery query;

    // 1. Get the club members list before deleting the club
    QVector<int> clubMemberIds;
    query.prepare("SELECT club_members FROM clubs_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);

    if (query.exec() && query.next()) {
        QString clubMembers = query.value(0).toString();
        clubMemberIds = Database::deserializeUserIds(clubMembers);
    }

    // 2. Remove this club from each member's joined_clubs list
    for (int userId : clubMemberIds) {
        query.prepare("SELECT joined_clubs, going_events FROM users_list WHERE user_id = :userId");
        query.bindValue(":userId", userId);

        if (query.exec() && query.next()) {
            // Get current joined clubs and going events
            QString joinedClubs = query.value(0).toString();
            QString goingEvents = query.value(1).toString();

            // Remove the club from joined clubs
            QVector<int> joinedClubsVector = Database::deserializeUserIds(joinedClubs);
            joinedClubsVector.removeAll(clubId);
            QString updatedJoinedClubs = Database::serializeUserIds(joinedClubsVector);

            // Get club's events
            QSqlQuery eventsQuery;
            eventsQuery.prepare("SELECT event_ids FROM clubs_list WHERE club_id = :clubId");
            eventsQuery.bindValue(":clubId", clubId);

            QVector<int> updatedGoingEvents;
            QVector<int> goingEventsVector = Database::deserializeUserIds(goingEvents);

            if (eventsQuery.exec() && eventsQuery.next()) {
                QString clubEvents = eventsQuery.value(0).toString();
                QVector<int> clubEventsVector = Database::deserializeUserIds(clubEvents);

                // For each event the user is going to
                for (int eventId : goingEventsVector) {
                    // Check if this event belongs to the club the user is leaving
                    QSqlQuery checkQuery;
                    checkQuery.prepare("SELECT 1 FROM events_list WHERE event_id = :eventId AND club_id = :clubId");
                    checkQuery.bindValue(":eventId", eventId);
                    checkQuery.bindValue(":clubId", clubId);

                    if (checkQuery.exec() && checkQuery.next()) {
                        // This event belongs to the club, remove it from going events
                        // and decrement the count
                        QSqlQuery updateEventQuery;
                        updateEventQuery.prepare("UPDATE events_list SET event_going_count = event_going_count - 1 WHERE event_id = :eventId");
                        updateEventQuery.bindValue(":eventId", eventId);
                        updateEventQuery.exec();
                    } else {
                        // This event doesn't belong to the club, keep it
                        updatedGoingEvents.append(eventId);
                    }
                }
            } else {
                // If no events found, keep all original events
                updatedGoingEvents = goingEventsVector;
            }

            // Update user record
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE users_list SET joined_clubs = :joinedClubs, going_events = :goingEvents WHERE user_id = :userId");
            updateQuery.bindValue(":joinedClubs", updatedJoinedClubs);
            updateQuery.bindValue(":goingEvents", Database::serializeUserIds(updatedGoingEvents));
            updateQuery.bindValue(":userId", userId);
            updateQuery.exec();
        }
    }

    // 3. Remove leader from clubleaders_list
    query.prepare("DELETE FROM clubleaders_list WHERE assigned_club_id = :clubId");
    query.bindValue(":clubId", clubId);
    query.exec();

    // 4. Remove notifications related to this club
    query.prepare("DELETE FROM notification_lists WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);
    query.exec();

    // 5. Delete all events belonging to this club
    query.prepare("DELETE FROM events_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);
    query.exec();

    // 6. Delete all messages belonging to this club
    query.prepare("DELETE FROM messages_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);
    query.exec();


    // 7. Finally delete the club itself
    query.prepare("DELETE FROM clubs_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);
    query.exec();
}
QFrame* AdminClub::createRoundedFrame()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setLineWidth(0);
    frame->setMinimumWidth(130);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    return frame;
}

void AdminClub::setupSearchFunctionality()
{
    // Connect search edit to search slot
    connect(searchEdit, &QLineEdit::textChanged, this, &AdminClub::searchClubs);
}

void AdminClub::searchClubs(const QString &searchText)
{
    // Skip filtering if search text is empty
    if (searchText.isEmpty()) {
        // Show all clubs
        for (int i = 0; i < clubsLayout->count(); ++i) {
            QLayoutItem* item = clubsLayout->itemAt(i);
            if (item && item->widget()) {
                item->widget()->setVisible(true);
            }
        }
        return;
    }

    // Hide/show club cards based on search text
    for (int i = 0; i < clubsLayout->count(); ++i) {
        QLayoutItem* item = clubsLayout->itemAt(i);
        if (item && item->widget()) {
            QFrame* clubFrame = qobject_cast<QFrame*>(item->widget());
            if (clubFrame) {
                // Find name label in the frame (typically the first QLabel with bold font)
                bool visible = false;
                QList<QLabel*> labels = clubFrame->findChildren<QLabel*>();

                for (QLabel* label : labels) {
                    QFont font = label->font();
                    if (font.bold() && font.pointSize() > 10) {  // Adjusted threshold for the smaller font
                        QString clubName = label->text();
                        visible = clubName.contains(searchText, Qt::CaseInsensitive);
                        break;
                    }
                }

                clubFrame->setVisible(visible);
            }
        }
    }
}
