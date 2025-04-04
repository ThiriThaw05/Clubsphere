#include "adminMember.h"
#include <QPixmap>
#include <QIcon>
#include<QPainter>
#include "adminMember.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include<database.h>

AdminMember::AdminMember(QWidget *parent) : QWidget(parent)
{
    Database::initialize();
    setupUI();
    setupMemberList();
    setupNavigation();
    setupSearchFunctionality(); // Added to connect search functionality
}

AdminMember::~AdminMember()
{
}

void AdminMember::setupUI()
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
    titleLabel = new QLabel("Clubsphere", this);
    titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Search bar
    searchFrame = new QFrame(this);
    searchFrame->setFrameShape(QFrame::StyledPanel);
    searchFrame->setStyleSheet("QFrame { background-color: #F0F0F0; border-radius: 12px; }");

    QHBoxLayout *searchLayout = new QHBoxLayout(searchFrame);
    searchLayout->setContentsMargins(15, 8, 15, 8);

    QLabel *searchIcon = new QLabel(this);
    searchIcon->setPixmap(QIcon(":/images/resources/search_logo.png").pixmap(20, 20));

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search");
    searchEdit->setStyleSheet("QLineEdit { border: none; background-color: transparent; }");

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(searchEdit);

    mainLayout->addWidget(searchFrame);

    // Members label
    membersLabel = new QLabel("Users:", this);
    membersLabel->setFont(QFont("Arial", 18, QFont::Bold));
    mainLayout->addWidget(membersLabel);

    // Create a container widget to properly manage the scroll area
    QWidget *scrollContainer = new QWidget(this);
    QVBoxLayout *scrollContainerLayout = new QVBoxLayout(scrollContainer);
    scrollContainerLayout->setContentsMargins(0, 0, 0, 0);
    scrollContainerLayout->setSpacing(0);

    // Scroll area for member list - key changes here
    scrollArea = new QScrollArea(scrollContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // Changed from AlwaysOff to AsNeeded
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set a minimum height for the scroll area to ensure it takes up space
    scrollArea->setMinimumHeight(350); // Adjust this value based on your UI needs

    scrollContent = new QWidget(scrollArea);
    scrollContent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    membersLayout = new QVBoxLayout(scrollContent);
    membersLayout->setAlignment(Qt::AlignTop);
    membersLayout->setContentsMargins(0, 0, 0, 0);
    membersLayout->setSpacing(15);

    scrollArea->setWidget(scrollContent);
    scrollContainerLayout->addWidget(scrollArea);

    // Add the scroll container to the main layout
    mainLayout->addWidget(scrollContainer, 1); // Give it a stretch factor of 1 to fill available space
}


void AdminMember::setupMemberList()
{
    // Ensure database is freshly initialized
    Database::initialize();

    // Clear any existing member cards
    QLayoutItem* item;
    while ((item = membersLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Force close and reopen the database connection
    {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen()) {
            db.close();
        }
        db.open();
    }

    // Verify database connection
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open in setupMemberList!";
        return;
    }

    Database::initialize();

    // Query users from the database with additional debugging
    QSqlQuery query;
    query.prepare("SELECT user_id, name, points, profile_photo FROM users_list ORDER BY points DESC");

    if (query.exec()) {
        int userCount = 0;
        QVector<QString> debugUsers;

        while (query.next()) {
            int userId = query.value(0).toInt();
            QString userName = query.value(1).toString();
            int points = query.value(2).toInt();
            QByteArray profilePhoto = query.value(3).toByteArray();

            // Debug: Store user details
            debugUsers.append(userName);

            // Use name if available, otherwise default to "User + ID"
            if (userName.isEmpty()) {
                userName = "User " + QString::number(userId);
            }

            createMemberCard(userName, QString::number(userId), points, profilePhoto);
            userCount++;
        }

        // Extensive debugging
        qDebug() << "Total users retrieved:" << userCount;
        qDebug() << "Users Details:";
        for (const auto& user : debugUsers) {
            qDebug() << "User:" << user ;
        }

        // Verify total users in database again
        QSqlQuery countQuery;
        if (countQuery.exec("SELECT COUNT(*) FROM users_list")) {
            if (countQuery.next()) {
                qDebug() << "Actual total users in database:" << countQuery.value(0).toInt();
            }
        }
    } else {
        qDebug() << "Error fetching users:" << query.lastError().text();
    }


}


void AdminMember::createMemberCard(const QString &name, const QString &id, int points, const QByteArray &profileImageData)
{
    QFrame *memberFrame = new QFrame(this);
    memberFrame->setFrameShape(QFrame::NoFrame);
    memberFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *cardLayout = new QHBoxLayout(memberFrame);
    cardLayout->setContentsMargins(0, 10, 0, 10);

    // Profile image
    QLabel *profileImage = new QLabel(this);
    QPixmap userAvatar;

    // Try to load the profile image from the database
    if (!profileImageData.isEmpty()) {
        if (userAvatar.loadFromData(profileImageData)) {
            qDebug() << "Loaded profile image from database for user:" << id;
        } else {
            qDebug() << "Failed to load profile image from database for user:" << id;
            userAvatar = QPixmap(":/images/resources/default_avatar.png");
        }
    } else {
        qDebug() << "No profile image data for user:" << id;
        userAvatar = QPixmap(":/images/resources/default_avatar.png");
    }

    // If we still don't have a valid avatar, create a placeholder
    if (userAvatar.isNull()) {
        userAvatar = QPixmap(60, 60);
        userAvatar.fill(Qt::lightGray);
    }

    // Resize the avatar to the correct size
    userAvatar = userAvatar.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Make the avatar circular
    QPixmap circularAvatar(userAvatar.size());
    circularAvatar.fill(Qt::transparent);

    QPainter painter(&circularAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(userAvatar));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(circularAvatar.rect());

    profileImage->setPixmap(circularAvatar);
    profileImage->setFixedSize(60, 60);

    // Member info
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(3);

    QLabel *nameLabel = new QLabel(name, this);
    nameLabel->setFont(QFont("Arial", 14, QFont::Bold));

    QLabel *idLabel = new QLabel("ID: " + id, this);
    idLabel->setFont(QFont("Arial", 10));

    QLabel *pointsLabel = new QLabel("Points: " + QString::number(points), this);
    pointsLabel->setFont(QFont("Arial", 10));

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(idLabel);
    infoLayout->addWidget(pointsLabel);

    cardLayout->addWidget(profileImage);
    cardLayout->addLayout(infoLayout, 1);

    membersLayout->addWidget(memberFrame);

    // Add separator line except for the last item
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #E0E0E0;");
    line->setMaximumHeight(1);
    membersLayout->addWidget(line);
}


void AdminMember::setupSearchFunctionality()
{
    // Connect search edit to search slot
    connect(searchEdit, &QLineEdit::textChanged, this, &AdminMember::searchMembers);
}

void AdminMember::searchMembers(const QString &searchText)
{
    // Skip filtering if search text is empty
    if (searchText.isEmpty()) {
        // Show all members
        for (int i = 0; i < membersLayout->count(); ++i) {
            QLayoutItem* item = membersLayout->itemAt(i);
            if (item && item->widget()) {
                item->widget()->setVisible(true);
            }
        }
        return;
    }

    // Hide/show member cards based on search text
    for (int i = 0; i < membersLayout->count(); ++i) {
        QLayoutItem* item = membersLayout->itemAt(i);
        if (item && item->widget()) {
            QFrame* memberFrame = qobject_cast<QFrame*>(item->widget());
            if (memberFrame) {
                // Find name label in the frame (typically the first QLabel with bold font)
                bool visible = false;
                QList<QLabel*> labels = memberFrame->findChildren<QLabel*>();

                for (QLabel* label : labels) {
                    QFont font = label->font();
                    if (font.bold() && font.pointSize() > 12) {  // This should target the name label
                        QString name = label->text();
                        visible = name.contains(searchText, Qt::CaseInsensitive);
                        break;
                    }
                }

                memberFrame->setVisible(visible);
            }
        }
    }
}

void AdminMember::setupNavigation()
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
    homeButton->setStyleSheet("QPushButton { border: none; }");

    profileButton = new QPushButton("", this);
    profileButton->setObjectName("profileButton");
    profileButton->setIcon(QIcon(":/images/resources/member_logo.png"));
    profileButton->setIconSize(QSize(20, 20));
    profileButton->setStyleSheet("QPushButton { border: none; background-color: #D9E9D8; border-radius: 5px; padding: 5px; }");

    groupsButton = new QPushButton("", this);
    groupsButton->setIcon(QIcon(":/images/resources/club_logo.png"));
    groupsButton->setIconSize(QSize(30, 30));
    groupsButton->setStyleSheet("QPushButton { border: none; }");

    navLayout->addWidget(homeButton);
    navLayout->addWidget(profileButton);
    navLayout->addWidget(groupsButton);

    mainLayout->addWidget(navigationFrame);

    // Connect navigation buttons
    connect(homeButton, &QPushButton::clicked, this, &AdminMember::onHomeButtonClicked);
    connect(profileButton, &QPushButton::clicked, this, &AdminMember::onProfileButtonClicked);
    connect(groupsButton, &QPushButton::clicked, this, &AdminMember::onGroupsButtonClicked);
}

void AdminMember::onHomeButtonClicked()
{
    qDebug() << "Home button clicked in AdminMember";
    emit navigateToHome();
}

void AdminMember::onProfileButtonClicked()
{
    qDebug() << "Profile button clicked in AdminMember - already on this page";
    // No navigation needed as we're already on this page
}

void AdminMember::onGroupsButtonClicked()
{
    qDebug() << "Groups button clicked in AdminMember";
    emit navigateToClubs();
}

QFrame* AdminMember::createRoundedFrame()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setLineWidth(0);
    frame->setMinimumWidth(130);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    return frame;
}
