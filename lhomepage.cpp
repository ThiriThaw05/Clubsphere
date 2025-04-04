#include "lhomepage.h"
#include "Event.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QBuffer>
#include <QDebug>
#include <QIcon>
#include <QFont>
#include <QSize>
#include<QPainter>

// CreateEventDialog implementation
CreateEventDialog::CreateEventDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Create New Event");
    setMinimumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Content edit
    QLabel* contentLabel = new QLabel("Event Description:", this);
    m_contentEdit = new QTextEdit(this);
    m_contentEdit->setPlaceholderText("Enter event description...");
    m_contentEdit->setMinimumHeight(100);

    // Code edit
    QLabel* codeLabel = new QLabel("Event Code:", this);
    m_codeEdit = new QLineEdit(this);
    m_codeEdit->setPlaceholderText("Enter event code...");

    // Image selection
    QLabel* imageLabel = new QLabel("Event Image:", this);
    m_imageButton = new QPushButton("Select Image", this);
    m_imagePreview = new QLabel(this);
    m_imagePreview->setMinimumSize(200, 150);
    m_imagePreview->setAlignment(Qt::AlignCenter);
    m_imagePreview->setStyleSheet("border: 1px solid gray;");
    m_imagePreview->setText("No image selected");

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_createButton = new QPushButton("Create", this);
    m_cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(m_createButton);
    buttonLayout->addWidget(m_cancelButton);

    // Add widgets to layout
    layout->addWidget(contentLabel);
    layout->addWidget(m_contentEdit);
    layout->addWidget(codeLabel);
    layout->addWidget(m_codeEdit);
    layout->addWidget(imageLabel);
    layout->addWidget(m_imageButton);
    layout->addWidget(m_imagePreview);
    layout->addLayout(buttonLayout);

    // Connect signals
    connect(m_imageButton, &QPushButton::clicked, this, &CreateEventDialog::onSelectImage);
    connect(m_createButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString CreateEventDialog::getEventContent() const
{
    return m_contentEdit->toPlainText();
}

QString CreateEventDialog::getEventCode() const
{
    return m_codeEdit->text();
}

QByteArray CreateEventDialog::getEventImageData() const
{
    return m_imageData;
}

void CreateEventDialog::onSelectImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image",
                                                    "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            // Resize image if it's too large
            if (pixmap.width() > 800 || pixmap.height() > 600) {
                pixmap = pixmap.scaled(800, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            // Display preview
            m_imagePreview->setPixmap(pixmap.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // Convert to QByteArray
            QBuffer buffer(&m_imageData);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");
        } else {
            QMessageBox::warning(this, "Image Load Error", "Failed to load the selected image.");
        }
    }
}

// EventCard implementation - With visible timestamp and square club photo frame
EventCard::EventCard(const QString& clubName, const QDateTime& dateTime,
                     const QString& description, const QByteArray& imageData,
                     int goingCount, const QString& eventCode, int eventId, QWidget* parent)
    : QFrame(parent), m_eventId(eventId)
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { background-color: white; border-radius: 10px; }");
    setFixedWidth(338);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // Debug the dateTime value
    qDebug() << "DateTime for card:" << dateTime.toString("MMMM d, h:mm AP");

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // 1. HEADER LAYOUT
    QHBoxLayout* headerLayout = new QHBoxLayout();

    // Profile image - Make it square instead of circle
    m_profileImage = new QLabel(this);
    m_profileImage->setFixedSize(40, 40);
    m_profileImage->setStyleSheet("border: 1px solid white; background-color: white;");

    // Load club photo from database based on club name
    QSqlQuery query;
    query.prepare("SELECT club_photo FROM clubs_list WHERE club_name = :clubName");
    query.bindValue(":clubName", clubName);

    if (query.exec() && query.next()) {
        QByteArray clubPhotoData = query.value(0).toByteArray();
        if (!clubPhotoData.isEmpty()) {
            QPixmap clubPixmap;
            clubPixmap.loadFromData(clubPhotoData);
            if (!clubPixmap.isNull()) {
                // Create square profile image - no rounded corners
                QPixmap squarePixmap = clubPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                // Center the image in the square frame
                QPixmap centeredPixmap(40, 40);
                centeredPixmap.fill(Qt::white);

                QPainter painter(&centeredPixmap);
                // Calculate position to center the image
                int x = (40 - squarePixmap.width()) / 2;
                int y = (40 - squarePixmap.height()) / 2;
                painter.drawPixmap(x, y, squarePixmap);

                m_profileImage->setPixmap(centeredPixmap);
            } else {
                // Default profile image if loading fails
                QPixmap profilePix(40, 40);
                profilePix.fill(Qt::lightGray);
                m_profileImage->setPixmap(profilePix);
            }
        } else {
            // Default profile image if no photo data
            QPixmap profilePix(40, 40);
            profilePix.fill(Qt::lightGray);
            m_profileImage->setPixmap(profilePix);
        }
    } else {
        // Default profile image if query fails
        QPixmap profilePix(40, 40);
        profilePix.fill(Qt::lightGray);
        m_profileImage->setPixmap(profilePix);
    }

    // Club name and date
    QVBoxLayout* headerInfoLayout = new QVBoxLayout();
    m_clubNameLabel = new QLabel(clubName, this);
    m_clubNameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    // Modified timestamp label with improved visibility
    m_dateTimeLabel = new QLabel(dateTime.toString("MMMM d, h:mm AP"), this);
    m_dateTimeLabel->setStyleSheet("color: #505050; font-size: 12px;");


    headerInfoLayout->addWidget(m_clubNameLabel);
    headerInfoLayout->addWidget(m_dateTimeLabel);
    headerInfoLayout->setSpacing(2);

    headerLayout->addWidget(m_profileImage);
    headerLayout->addLayout(headerInfoLayout, 1);

    // Event join code on the right side of header
    QLabel* joinCodeLabel = new QLabel(QString("Join Code: %1").arg(eventCode), this);
    joinCodeLabel->setStyleSheet("color: #3366cc; font-weight: bold; font-size: 12px;");
    headerLayout->addWidget(joinCodeLabel, 1);

    // 2. CONTENT (DESCRIPTION)
    m_descriptionLabel = new QLabel(description, this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("font-size: 13px; margin: 5px 0px;");

    // 3. EVENT IMAGE (only created if there's image data)
    m_eventImageLabel = nullptr; // Initialize to nullptr
    if (!imageData.isEmpty()) {
        QPixmap pixmap;
        if (pixmap.loadFromData(imageData)) {
            m_eventImageLabel = new QLabel(this);
            m_eventImageLabel->setFixedHeight(280);
            m_eventImageLabel->setAlignment(Qt::AlignCenter);
            m_eventImageLabel->setStyleSheet("background-color: white; border-radius: 5px;");
            m_eventImageLabel->setPixmap(pixmap.scaled(335, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // 4. FOOTER LAYOUT
    QHBoxLayout* footerLayout = new QHBoxLayout();

    m_goingLabel = new QLabel("Going", this);
    m_goingLabel->setStyleSheet("font-weight: bold; background-color: transparent; padding: 5px 10px;");

    // Going count
    m_goingCountLabel = new QLabel(QString::number(goingCount), this);
    m_goingCountLabel->setStyleSheet("font-weight: bold;");

    // Delete button (replacing Not Going button)
    m_deleteButton = new QPushButton("Delete", this);
    m_deleteButton->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 15px; padding: 5px 10px; color: red; }");

    footerLayout->addWidget(m_goingLabel);
    footerLayout->addWidget(m_goingCountLabel);
    footerLayout->addStretch();
    footerLayout->addWidget(m_deleteButton);

    // Add everything to main layout in the correct order:
    // 1. Header
    mainLayout->addLayout(headerLayout);
    // 2. Content
    mainLayout->addWidget(m_descriptionLabel);
    // 3. Image (only if there is valid image data and it loaded correctly)
    if (m_eventImageLabel) {
        mainLayout->addWidget(m_eventImageLabel);
    }
    // 4. Footer
    mainLayout->addLayout(footerLayout);

    // Connect delete button
    connect(m_deleteButton, &QPushButton::clicked, this, &EventCard::onDeleteClicked);
}

void EventCard::onDeleteClicked()
{
    int response = QMessageBox::question(this, "Confirm Delete",
                                         "Are you sure you want to delete this event?",
                                         QMessageBox::Yes | QMessageBox::No);

    if (response == QMessageBox::Yes) {
        emit deleteEvent(m_eventId);
    }
}

// LHomepage implementation
LHomepage::LHomepage(int leaderId, QWidget *parent)
    : QWidget(parent), m_leaderId(leaderId)
{
    // Find the club associated with this leader
    QSqlQuery query;
    query.prepare("SELECT club_id, club_name FROM clubs_list WHERE leader_id = :leaderId");
    query.bindValue(":leaderId", leaderId);

    if (query.exec() && query.next()) {
        m_clubId = query.value(0).toInt();
        m_clubName = query.value(1).toString();
    } else {
        qDebug() << "Error finding club for leader:" << query.lastError().text();
        m_clubId = -1;
        m_clubName = "Unknown Club";
    }

    setupUI();
    loadClubEvents();
}

LHomepage::~LHomepage()
{
}

void LHomepage::setupUI()
{
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
    m_titleLabel = new QLabel(QString("%1").arg(m_clubName));
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
    connect(achievementButton, &QPushButton::clicked, this, &LHomepage::onAchievementButtonClicked);

    // Chat button
    QPushButton* chatButton = new QPushButton();
    chatButton->setIcon(QIcon(":/images/resources/chat.png"));
    chatButton->setIconSize(QSize(30, 30));
    chatButton->setFixedSize(45, 45);
    chatButton->setStyleSheet("QPushButton { background-color: #e0e0e0; border-radius: 22px; }");
    connect(chatButton, &QPushButton::clicked, this, &LHomepage::onChatButtonClicked);

    // Notification button
    QPushButton* notificationButton = new QPushButton();
    notificationButton->setIcon(QIcon(":/images/resources/noti_logo.png"));
    notificationButton->setIconSize(QSize(30, 30));
    notificationButton->setFixedSize(45, 45);
    notificationButton->setStyleSheet("QPushButton { background-color: #e0e0e0; border-radius: 22px; }");
    connect(notificationButton, &QPushButton::clicked, this, &LHomepage::onNotificationButtonClicked);

    m_headerIconsLayout->addWidget(achievementButton);
    m_headerIconsLayout->addWidget(chatButton);
    m_headerIconsLayout->addWidget(notificationButton);
    m_headerIconsLayout->setSpacing(10);

    titleLayout->addLayout(m_headerIconsLayout);

    // Create new event button
    m_createNewLayout = new QHBoxLayout();
    m_createNewButton = new QPushButton();
    m_createNewButton->setIcon(QIcon(":/images/resources/plus_logo.png"));
    m_createNewButton->setIconSize(QSize(20, 20));
    m_createNewButton->setFixedSize(45, 45);
    m_createNewButton->setStyleSheet("QPushButton { background-color: #e0e0e0; border-radius: 22px; }");
    connect(m_createNewButton, &QPushButton::clicked, this, &LHomepage::createNewEvent);

    QLabel* createNewLabel = new QLabel("Create new event");
    createNewLabel->setStyleSheet("color: #505050; font-size: 14px;");

    m_createNewLayout->addWidget(m_createNewButton);
    m_createNewLayout->addWidget(createNewLabel);
    m_createNewLayout->addStretch();

    // Add sections to header layout
    headerLayout->addLayout(titleLayout);
    headerLayout->addSpacing(15);
    headerLayout->addLayout(m_createNewLayout);

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

    // Add all to main layout
    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(m_scrollArea, 1);

    // Set fixed width to match mobile viewport
    setFixedWidth(350);
    setMinimumHeight(650);
}

void LHomepage::loadClubEvents()
{
    // Clear existing events
    QLayoutItem* item;
    while ((item = m_EventsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Load events for this club directly from the database
    QSqlQuery query;
    query.prepare("SELECT event_id, event_content, event_photo, event_code, created_timestamp, event_going_count "
                  "FROM events_list "
                  "WHERE club_id = :clubId "
                  "ORDER BY created_timestamp DESC");
    query.bindValue(":clubId", m_clubId);

    if (query.exec()) {
        bool hasEvents = false;

        while (query.next()) {
            hasEvents = true;

            int eventId = query.value(0).toInt();
            QString content = query.value(1).toString();
            QByteArray imageData = query.value(2).toByteArray();
            QString eventCode = query.value(3).toString();
            QDateTime timestamp = QDateTime::fromSecsSinceEpoch(query.value(4).toLongLong());
            int goingCount = query.value(5).toInt();

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
            EventCard* card = new EventCard(m_clubName, timestamp, content, imageData,
                                            goingCount, eventCode, eventId, this);
            connect(card, &EventCard::deleteEvent, this, &LHomepage::onDeleteEvent);
            m_EventsLayout->addWidget(card);
        }

        if (!hasEvents) {
            QLabel* noEventsLabel = new QLabel("No events found for this club", this);
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

void LHomepage::addEventCard(int eventId, const QString& clubName, const QDateTime& dateTime,
                             const QString& description, const QByteArray& imageData,
                             int goingCount, const QString& eventCode)
{
    EventCard* card = new EventCard(clubName, dateTime, description, imageData,
                                    goingCount, eventCode, eventId, this);

    connect(card, &EventCard::deleteEvent, this, &LHomepage::onDeleteEvent);

    m_EventsLayout->addWidget(card);
}

void LHomepage::createNewEvent()
{
    CreateEventDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString content = dialog.getEventContent();
        QString code = dialog.getEventCode();
        QByteArray imageData = dialog.getEventImageData();

        if (content.isEmpty()) {
            QMessageBox::warning(this, "Missing Information", "Please enter event description.");
            return;
        }

        // Create new event object
        Event event;
        event.setId(Event::generateUniqueEventId());
        event.setClubId(m_clubId);
        event.setContent(content);
        event.setPhoto(imageData);
        event.setCode(code);
        event.setGoingCount(0);
        event.setCreatedTimestamp(QDateTime::currentDateTime());

        // Save to database
        if (event.saveToDatabase()) {
            QMessageBox::information(this, "Success", "Event created successfully.");
            refreshEvents();
        } else {
            QMessageBox::critical(this, "Error", "Failed to create event.");
        }
    }
}

void LHomepage::onDeleteEvent(int eventId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM events_list WHERE event_id = :id");
    query.bindValue(":id", eventId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Event deleted successfully.");
        refreshEvents();
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete event: " + query.lastError().text());
    }
}

void LHomepage::refreshEvents()
{
    loadClubEvents();
}

void LHomepage::onChatButtonClicked()
{
    emit showGroupChat(m_clubId, m_leaderId);
}

void LHomepage::onNotificationButtonClicked()
{
    emit showNotifications();
}

void LHomepage::onAchievementButtonClicked()
{
    emit showLeaderboard(m_clubId);
}
