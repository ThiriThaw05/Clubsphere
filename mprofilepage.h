// mprofilepage.h
#ifndef MPROFILEPAGE_H
#define MPROFILEPAGE_H
#include<QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QSqlQuery>
#include<QScrollBar>

class MProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit MProfilePage(int userId, QWidget *parent = nullptr);
    ~MProfilePage();

signals:
    void navigateToHome();
    void navigateToClub();
    void navigateToGoing();
    void navigateToProfile();

private slots:
    void homeClicked();
    void clubClicked();
    void eventClicked();
    void profileClicked();
    void editProfileImage();
    void editUsername();

private:
    void setupUI();
    void loadUserData();
    void loadClubRankings();
    void updateProfilePicture(const QPixmap &pixmap);
    void saveProfileImageToDatabase();
    QPixmap createCircularPixmap(const QPixmap& pixmap, int size);

    int user_id;
    QString userName;
    QVector<int> joinedClubs;
    QPixmap m_profilePixmap;

    // UI Components
    QVBoxLayout* m_mainLayout;
    QWidget* m_bottomNavBar;
    QHBoxLayout* m_bottomNavLayout;

    // Profile components
    QWidget* profileBgWidget;
    QLabel* profilePicWidget;
    QPushButton* editProfileBtn;
    QPushButton* editUsernameBtn;
    QLabel* usernameValue;
    QLabel* idValue;
    QLabel* pointsValue;
    QLabel* clubsJoinedValue;

    // Rankings components
    QScrollArea* rankingsScrollArea;
    QWidget* rankingsWidget;
    QVBoxLayout* rankingsLayout;

    // Navigation components
    QPushButton* profileIcon;
};

#endif // MPROFILEPAGE_H
