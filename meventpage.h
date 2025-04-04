#ifndef MEVENTPAGE_H
#define MEVENTPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>

// EventCard class for member homepage
class MEventCard : public QFrame {
    Q_OBJECT

public:
    MEventCard(int eventId, const QString& clubName, const QDateTime& dateTime,
               const QString& description, const QByteArray& imageData,
               int goingCount, int userId, bool isAlreadyGoing, QWidget* parent = nullptr);

private slots:
    void onGoingClicked();

private:
    int m_eventId;
    int m_userId;
    bool m_isGoing;
    QLabel* m_profileImage;
    QLabel* m_clubNameLabel;
    QLabel* m_dateTimeLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_eventImageLabel;
    QPushButton* m_goingButton;
    QLabel* m_goingCountLabel;
};

class MEventPage : public QWidget
{
    Q_OBJECT

public:
    MEventPage(int clubId,int userId, QWidget *parent=nullptr);
    ~MEventPage();

signals:
    void navigateToHome();
    void navigateToClub();
    void navigateToGoing();
    void navigateToProfile();
    void navigateToBoard();

private:
    int m_clubId;
    int user_id;
    void setupUI();
    void loadClubEvents();

    // UI Components
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_eventsLayout;
};

#endif // MEVENTPAGE_H
