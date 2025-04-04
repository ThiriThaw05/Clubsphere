#ifndef LHOMEPAGE_H
#define LHOMEPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QDateTime>

// Dialog for creating new events
class CreateEventDialog : public QDialog
{
    Q_OBJECT

public:
    CreateEventDialog(QWidget* parent = nullptr);
    QString getEventContent() const;
    QString getEventCode() const;
    QByteArray getEventImageData() const;

private slots:
    void onSelectImage();

private:
    QTextEdit* m_contentEdit;
    QLineEdit* m_codeEdit;
    QPushButton* m_imageButton;
    QLabel* m_imagePreview;
    QPushButton* m_createButton;
    QPushButton* m_cancelButton;
    QByteArray m_imageData;
};

// Event card widget
class EventCard : public QFrame
{
    Q_OBJECT

public:
    EventCard(const QString& clubName, const QDateTime& dateTime,
              const QString& description, const QByteArray& imageData,
              int goingCount, const QString& eventCode, int eventId, QWidget* parent = nullptr);

signals:
    void deleteEvent(int eventId);

private slots:
    void onDeleteClicked();

private:
    QLabel* m_profileImage;
    QLabel* m_clubNameLabel;
    QLabel* m_dateTimeLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_eventImageLabel;  // Now can be nullptr if no image
    QLabel* m_goingLabel;
    QLabel* m_goingCountLabel;
    QPushButton* m_deleteButton;
    int m_eventId;
};
// Leader homepage widget
class LHomepage : public QWidget
{
    Q_OBJECT

public:
    LHomepage(int leaderId, QWidget *parent = nullptr);
    ~LHomepage();

signals:
    void showGroupChat(int clubId, int currentUserId);
    void showNotifications();
    void showLeaderboard(int clubId);

private slots:
    void createNewEvent();
    void onDeleteEvent(int eventId);
    void onChatButtonClicked();
    void onNotificationButtonClicked();
    void onAchievementButtonClicked();

private:
    void setupUI();
    void loadClubEvents();
    void refreshEvents();
    void addEventCard(int eventId, const QString& clubName, const QDateTime& dateTime,
                      const QString& description, const QByteArray& imageData,
                      int goingCount, const QString& eventCode);

    int m_leaderId;
    int m_clubId;
    QString m_clubName;

    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_headerIconsLayout;
    QHBoxLayout* m_createNewLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_EventsLayout;
    QLabel* m_titleLabel;
    QPushButton* m_createNewButton;
};

#endif // LHOMEPAGE_H
