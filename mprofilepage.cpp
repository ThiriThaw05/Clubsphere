// mprofilepage.cpp
#include "mprofilepage.h"
#include <QIcon>
#include <QFont>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "database.h"

MProfilePage::MProfilePage(int userId, QWidget *parent)
    : QWidget(parent), user_id(userId)
{
    setupUI();
    loadUserData();
}

MProfilePage::~MProfilePage()
{
}

void MProfilePage::setupUI()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create a scroll area for the whole page content
    QScrollArea* mainScrollArea = new QScrollArea(this);
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainScrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(0);

    // Profile area with light green background
    profileBgWidget = new QWidget();
    profileBgWidget->setFixedHeight(200);
    profileBgWidget->setStyleSheet("background-color: #c9e4c5;");

    // Profile picture area with image from resources
    profilePicWidget = new QLabel();
    profilePicWidget->setFixedSize(120, 120);

    // Default profile image setup
    m_profilePixmap = QPixmap(":/images/resources/user.png");
    if (!m_profilePixmap.isNull()) {
        updateProfilePicture(m_profilePixmap);
    } else {
        // Fallback if image isn't found - keep the gray circle
        profilePicWidget->setStyleSheet(
            "background-color: #dddddd; border-radius: 60px;"
            );

        // Inner white circle fallback
        QWidget* innerCircle = new QWidget(profilePicWidget);
        innerCircle->setFixedSize(70, 70);
        innerCircle->move(25, 25);
        innerCircle->setStyleSheet(
            "background-color: white; border-radius: 35px;"
            );
    }

    // Edit icon button - now using PNG image
    editProfileBtn = new QPushButton();
    editProfileBtn->setFixedSize(30, 30);
    QIcon editIcon(":/images/resources/edit.png");
    if (!editIcon.isNull()) {
        editProfileBtn->setIcon(editIcon);
        editProfileBtn->setIconSize(QSize(24, 24));
        editProfileBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(128, 128, 128, 0.5);"
            "  border-radius: 15px;"
            "  border: none;"
            "}"
            );
    } else {
        // Fallback if image isn't found
        editProfileBtn->setText("✎");
        editProfileBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(128, 128, 128, 0.5);"
            "  border-radius: 15px;"
            "  border: none;"
            "  color: white;"
            "  font-weight: bold;"
            "}"
            );
    }

    // Connect edit profile button
    connect(editProfileBtn, &QPushButton::clicked, this, &MProfilePage::editProfileImage);

    // Position profile pic and edit button
    QVBoxLayout* profileBgLayout = new QVBoxLayout(profileBgWidget);
    profileBgLayout->setAlignment(Qt::AlignCenter);
    profileBgLayout->addWidget(profilePicWidget, 0, Qt::AlignCenter);

    // Position edit button relative to profilePicWidget
    editProfileBtn->setParent(profilePicWidget);
    editProfileBtn->move(profilePicWidget->width() - 20, profilePicWidget->height() - 20);

    // User information section
    QWidget* userInfoWidget = new QWidget();
    userInfoWidget->setStyleSheet("background-color: white;");

    QVBoxLayout* userInfoLayout = new QVBoxLayout(userInfoWidget);
    userInfoLayout->setContentsMargins(20, 20, 20, 20);
    userInfoLayout->setSpacing(10);

    // Username row with edit icon
    QWidget* usernameWidget = new QWidget();
    QHBoxLayout* usernameLayout = new QHBoxLayout(usernameWidget);
    usernameLayout->setContentsMargins(0, 5, 0, 5);

    QLabel* usernameLabel = new QLabel("Username");
    QFont regularFont;
    regularFont.setPointSize(11);
    usernameLabel->setFont(regularFont);
    usernameLabel->setStyleSheet("color: #666666;");

    editUsernameBtn = new QPushButton();
    QIcon editUsernameIcon(":/images/resources/edit.png");
    if (!editUsernameIcon.isNull()) {
        editUsernameBtn->setIcon(editUsernameIcon);
        editUsernameBtn->setIconSize(QSize(16, 16));
        editUsernameBtn->setFixedSize(20, 20);
        editUsernameBtn->setStyleSheet("border: none; background: transparent;");
    } else {
        // Fallback if image isn't found
        editUsernameBtn->setText("✎");
        editUsernameBtn->setFixedSize(20, 20);
        editUsernameBtn->setStyleSheet("color: #888888; background: transparent; border: none;");
    }

    // Connect edit username button
    connect(editUsernameBtn, &QPushButton::clicked, this, &MProfilePage::editUsername);

    usernameValue = new QLabel("Loading...");
    QFont valueFont;
    valueFont.setPointSize(11);
    usernameValue->setFont(valueFont);
    usernameValue->setAlignment(Qt::AlignRight);

    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addStretch(1);
    usernameLayout->addWidget(editUsernameBtn);
    usernameLayout->addWidget(usernameValue);

    // Separator line
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line1->setStyleSheet("background-color: #e0e0e0;");
    line1->setFixedHeight(1);

    // ID row
    QWidget* idWidget = new QWidget();
    QHBoxLayout* idLayout = new QHBoxLayout(idWidget);
    idLayout->setContentsMargins(0, 5, 0, 5);

    QLabel* idLabel = new QLabel("ID");
    idLabel->setFont(regularFont);
    idLabel->setStyleSheet("color: #666666;");

    idValue = new QLabel(QString::number(user_id));
    idValue->setFont(valueFont);
    idValue->setAlignment(Qt::AlignRight);

    idLayout->addWidget(idLabel);
    idLayout->addStretch(1);
    idLayout->addWidget(idValue);

    // Separator line
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line2->setStyleSheet("background-color: #e0e0e0;");
    line2->setFixedHeight(1);

    // Points row
    QWidget* pointsWidget = new QWidget();
    QHBoxLayout* pointsLayout = new QHBoxLayout(pointsWidget);
    pointsLayout->setContentsMargins(0, 5, 0, 5);

    QLabel* pointsLabel = new QLabel("Points");
    pointsLabel->setFont(regularFont);
    pointsLabel->setStyleSheet("color: #666666;");

    pointsValue = new QLabel("Loading...");
    pointsValue->setFont(valueFont);
    pointsValue->setAlignment(Qt::AlignRight);

    pointsLayout->addWidget(pointsLabel);
    pointsLayout->addStretch(1);
    pointsLayout->addWidget(pointsValue);

    // Separator line
    QFrame* line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    line3->setStyleSheet("background-color: #e0e0e0;");
    line3->setFixedHeight(1);

    // Total Clubs Joined row
    QWidget* clubsWidget = new QWidget();
    QHBoxLayout* clubsLayout = new QHBoxLayout(clubsWidget);
    clubsLayout->setContentsMargins(0, 5, 0, 5);

    QLabel* clubsLabel = new QLabel("Total Clubs Joined");
    clubsLabel->setFont(regularFont);
    clubsLabel->setStyleSheet("color: #666666;");

    clubsJoinedValue = new QLabel("Loading...");
    clubsJoinedValue->setFont(valueFont);
    clubsJoinedValue->setAlignment(Qt::AlignRight);

    clubsLayout->addWidget(clubsLabel);
    clubsLayout->addStretch(1);
    clubsLayout->addWidget(clubsJoinedValue);

    // Add rows to user info layout
    userInfoLayout->addWidget(usernameWidget);
    userInfoLayout->addWidget(line1);
    userInfoLayout->addWidget(idWidget);
    userInfoLayout->addWidget(line2);
    userInfoLayout->addWidget(pointsWidget);
    userInfoLayout->addWidget(line3);
    userInfoLayout->addWidget(clubsWidget);

    // Rankings section
    QLabel* rankingsLabel = new QLabel("Your Rankings:");
    rankingsLabel->setFont(regularFont);
    rankingsLabel->setContentsMargins(20, 15, 0, 10);

    // Create the widget for rankings directly (no separate scroll area)
    rankingsWidget = new QWidget();
    rankingsWidget->setStyleSheet("background-color: #dddddd;");

    rankingsLayout = new QVBoxLayout(rankingsWidget);
    rankingsLayout->setContentsMargins(20, 10, 20, 10);
    rankingsLayout->setSpacing(0);

    // We'll populate the rankings in loadUserData function
    rankingsLayout->addStretch(1);

    // Bottom navigation bar - keeping as in the original code
    m_bottomNavBar = new QWidget();
    m_bottomNavBar->setFixedHeight(60);
    m_bottomNavBar->setStyleSheet("background-color: white; border-top: 1px solid #e0e0e0;");

    m_bottomNavLayout = new QHBoxLayout(m_bottomNavBar);
    m_bottomNavLayout->setContentsMargins(10, 5, 10, 5);
    m_bottomNavLayout->setSpacing(0);

    // Create navigation buttons
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

    // Profile icon - Using your PNG image
    profileIcon = new QPushButton();
    QIcon profileIconImage(":/images/resources/user.png");
    if (!profileIconImage.isNull()) {
        profileIcon->setIcon(profileIconImage);
        profileIcon->setIconSize(QSize(40, 40));
    } else {
        profileIcon->setText("Profile");
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
    connect(homeIcon, &QPushButton::clicked, this, &MProfilePage::homeClicked);
    connect(clubIcon, &QPushButton::clicked, this, &MProfilePage::clubClicked);
    connect(eventIcon, &QPushButton::clicked, this, &MProfilePage::eventClicked);
    connect(profileIcon, &QPushButton::clicked, this, &MProfilePage::profileClicked);

    // Add all components to scrollLayout
    scrollLayout->addWidget(profileBgWidget);
    scrollLayout->addWidget(userInfoWidget);
    scrollLayout->addWidget(rankingsLabel);
    scrollLayout->addWidget(rankingsWidget);

    // Set the content widget for the scroll area
    mainScrollArea->setWidget(scrollContent);

    // Main layout now contains just the scroll area and bottom navbar
    m_mainLayout->addWidget(mainScrollArea, 1);
    m_mainLayout->addWidget(m_bottomNavBar);

    // Set fixed width to match window size
    setFixedWidth(350);
    setFixedHeight(650);

    // Set background color for the whole page
    setStyleSheet("background-color: white;");
}

void MProfilePage::loadUserData()
{
    QSqlQuery query;
    query.prepare("SELECT name, points, profile_photo, joined_clubs FROM users_list WHERE user_id = :userId");
    query.bindValue(":userId", user_id);

    if (query.exec() && query.next()) {
        // Get user name
        userName = query.value(0).toString();
        if (userName.isEmpty()) {
            // Try to get name from SE names list
            userName = Database::getStudentNameById(user_id);
            if (userName.isEmpty()) {
                userName = "User " + QString::number(user_id);
            }

            // Save the name to the database for future use
            QSqlQuery updateNameQuery;
            updateNameQuery.prepare("UPDATE users_list SET name = :name WHERE user_id = :userId");
            updateNameQuery.bindValue(":name", userName);
            updateNameQuery.bindValue(":userId", user_id);
            updateNameQuery.exec();
        }
        usernameValue->setText(userName);

        // Get points
        int points = query.value(1).toInt();
        pointsValue->setText(QString::number(points));

        // Get profile photo
        QByteArray photoData = query.value(2).toByteArray();
        if (!photoData.isEmpty()) {
            QPixmap pixmap;
            if (pixmap.loadFromData(photoData)) {
                m_profilePixmap = pixmap;
                updateProfilePicture(m_profilePixmap);

                QPixmap roundedPixmap = createCircularPixmap(pixmap, 24);
                profileIcon->setIcon(QIcon(roundedPixmap));
            }
        }

        // Get joined clubs
        QString joinedClubsStr = query.value(3).toString();
        joinedClubs = Database::deserializeUserIds(joinedClubsStr);
        clubsJoinedValue->setText(QString::number(joinedClubs.size()));

        // Load club rankings (just a placeholder for now)
        loadClubRankings();
    } else {
        qDebug() << "Failed to load user data:" << query.lastError().text();
        usernameValue->setText("User " + QString::number(user_id));
        pointsValue->setText("0");
        clubsJoinedValue->setText("0");
    }
}

// Add this helper method to create a circular pixmap for the navigation bar:
QPixmap MProfilePage::createCircularPixmap(const QPixmap& pixmap, int size)
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

void MProfilePage::loadClubRankings()
{
    // Clear existing rankings
    QLayoutItem* item;
    while ((item = rankingsLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    QFont regularFont;
    regularFont.setPointSize(12);
    QFont rankFont;
    rankFont.setPointSize(12);
    rankFont.setBold(true);

    for (int clubId : joinedClubs) {
        QSqlQuery clubQuery;
        clubQuery.prepare("SELECT club_name FROM clubs_list WHERE club_id = :clubId");
        clubQuery.bindValue(":clubId", clubId);

        if (clubQuery.exec() && clubQuery.next()) {
            QString clubName = clubQuery.value(0).toString();

            // Create a simple row widget
            QWidget* rowWidget = new QWidget();
            rowWidget->setFixedHeight(40); // Fixed height for consistent rows

            QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
            rowLayout->setContentsMargins(15, 0, 15, 0); // Left and right padding
            rowLayout->setSpacing(0); // No spacing between elements

            // Club name label (left-aligned)
            QLabel* clubLabel = new QLabel(clubName);
            clubLabel->setFont(regularFont);
            clubLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            rowLayout->addWidget(clubLabel, 1); // Takes available space

            // Rank label (right-aligned)
            int mRank = Database::calculateUserRankInClub(user_id, clubId);
            QLabel* rankLabel = new QLabel("Rank: " + QString::number(mRank));
            rankLabel->setFont(rankFont);
            rankLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            rowLayout->addWidget(rankLabel);

            // Add subtle separator (optional)
            QFrame* separator = new QFrame();
            separator->setFrameShape(QFrame::HLine);
            separator->setFrameShadow(QFrame::Sunken);
            separator->setStyleSheet("color: #e0e0e0;");
            separator->setFixedHeight(1);

            // Add to layout
            rankingsLayout->addWidget(rowWidget);
            rankingsLayout->addWidget(separator);
        }
    }

    // If no clubs are joined
    if (joinedClubs.isEmpty()) {
        QLabel* noClubsLabel = new QLabel("No clubs joined");
        noClubsLabel->setAlignment(Qt::AlignCenter);
        noClubsLabel->setStyleSheet("color: #999999;");
        rankingsLayout->addWidget(noClubsLabel);
    }

    rankingsLayout->addStretch(1); // Push content to top
}

void MProfilePage::updateProfilePicture(const QPixmap &pixmap)
{
    // Scale the pixmap to fit the label
    QPixmap scaledPixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Create a circular mask
    QBitmap mask(120, 120);
    mask.fill(Qt::white);
    QPainter painter(&mask);
    painter.setBrush(Qt::black);
    painter.drawEllipse(0, 0, 120, 120);

    // Apply the mask to make the image circular
    scaledPixmap.setMask(mask);
    profilePicWidget->setPixmap(scaledPixmap);
}

void MProfilePage::editProfileImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Profile Picture"),
                                                    "",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        QPixmap newPixmap(fileName);
        if (!newPixmap.isNull()) {
            // Update the profile picture
            m_profilePixmap = newPixmap;
            updateProfilePicture(m_profilePixmap);

            // Update the profile icon in the nav bar
            if (!profileIcon->icon().isNull()) {
                profileIcon->setIcon(QIcon(newPixmap));
            }

            // Save to database
            saveProfileImageToDatabase();
        }
    }
}

void MProfilePage::saveProfileImageToDatabase()
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    m_profilePixmap.save(&buffer, "PNG");

    QSqlQuery query;
    query.prepare("UPDATE users_list SET profile_photo = :photo WHERE user_id = :userId");
    query.bindValue(":photo", imageData);
    query.bindValue(":userId", user_id);

    if (!query.exec()) {
        qDebug() << "Failed to save profile image:" << query.lastError().text();
        QMessageBox::warning(this, tr("Error"), tr("Failed to save profile image to database."));
    }
}

void MProfilePage::editUsername()
{
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Edit Username"),
                                            tr("Enter new username:"), QLineEdit::Normal,
                                            userName, &ok);
    if (ok && !newName.isEmpty()) {
        userName = newName;
        usernameValue->setText(newName);

        // Save to database
        QSqlQuery query;
        query.prepare("UPDATE users_list SET name = :name WHERE user_id = :userId");
        query.bindValue(":name", newName);
        query.bindValue(":userId", user_id);

        if (!query.exec()) {
            qDebug() << "Failed to update username:" << query.lastError().text();
            QMessageBox::warning(this, tr("Error"), tr("Failed to save username to database."));
        }
    }
}

void MProfilePage::homeClicked()
{
    emit navigateToHome();
}

void MProfilePage::clubClicked()
{
    emit navigateToClub();
}

void MProfilePage::eventClicked()
{
    emit navigateToGoing();
}

void MProfilePage::profileClicked()
{
    // Already on profile page, no need to navigate
    emit navigateToProfile();
}
