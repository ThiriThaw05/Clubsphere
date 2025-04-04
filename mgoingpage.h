// mgoingpage.h
#ifndef MGOINGPAGE_H
#define MGOINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPainter>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>

class MGoingPage : public QWidget
{
    Q_OBJECT

public:
    explicit MGoingPage(int userId, QWidget *parent = nullptr);
    ~MGoingPage();

    // Set the current user ID
    void setUserId(int userId);

    // Load user's events
    void loadUserEvents();

private:
    void setupUI();
    QWidget* createClubEventItem(int eventId, const QString& clubName, const QString& eventContent, const QByteArray& eventPhoto, const QString& eventCode, QDateTime eventDate);
    QWidget* createSeparator();
    QPixmap createCircularPixmap(const QPixmap& pixmap, int size);

    // Helper method to update user points
    bool updateUserPoints(int userId, int pointsToAdd);

    // Helper method to remove event from user's going events
    bool removeEventFromUserGoing(int userId, int eventId);

    // Get club name by ID
    QString getClubNameById(int clubId);

    // Clear existing events from layout
    void clearEvents();

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QWidget* m_bottomNavBar;
    QHBoxLayout* m_bottomNavLayout;
    QPushButton* m_joinedButton;
    QPushButton* m_pendingButton;
    QStackedWidget* m_stackedWidget;

    // Content widget and layout for events
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;

    // Store current user ID
    int m_currentUserId;

    // Map of event widgets to event IDs for tracking
    QMap<QPushButton*, int> m_eventButtonMap;
    QMap<QLineEdit*, int> m_eventInputMap;

signals:
    void navigateToHome();
    void navigateToClub();
    void navigateToProfile();
    void navigateToGoing();

public slots:
    void showJoinedView();
    void showPendingView();

private slots:
    void homeClicked();
    void clubClicked();
    void eventClicked();
    void profileClicked();
    void verifyEventCode();
};

#endif // MGOINGPAGE_H
