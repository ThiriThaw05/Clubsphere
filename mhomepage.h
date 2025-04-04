#ifndef MHOMEPAGE_H
#define MHOMEPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <meventpage.h>


class MHomepage : public QWidget {
    Q_OBJECT

public:
    explicit MHomepage(int userId, QWidget *parent = nullptr);
    ~MHomepage();

public slots:
    void refreshEvents();

signals:
    void navigateToHome();
    void navigateToClub();
    void navigateToGoing();
    void navigateToProfile();
    void navigateToBoard();
    void navigateToSearchClubs();

private slots:
    void onHomeClicked();
    void onClubClicked();
    void onEventClicked();
    void onProfileClicked();
    void onLeaderboardClicked();
    void onSearchClubsClicked();

private:
    void setupUI();
    void loadUserClubEvents();
    QPixmap createCircularPixmap(const QPixmap& pixmap, int size);

    int m_userId;
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QHBoxLayout* m_headerIconsLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_EventsLayout;
};

#endif // MHOMEPAGE_H
