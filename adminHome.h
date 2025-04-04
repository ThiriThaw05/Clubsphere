#ifndef ADMINHOME_H
#define ADMINHOME_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

class AdminHome : public QWidget
{
    Q_OBJECT

public:
    AdminHome(QWidget *parent = nullptr);
    ~AdminHome();

signals:
    void navigateToMembers();
    void navigateToClubs();

private slots:
    void onHomeButtonClicked();
    void onProfileButtonClicked();
    void onGroupsButtonClicked();

private:
    int getTotalUsersCount();
    int getTotalClubsCount();
    void setupUI();
    void setupStatsSection();
    void setupTopClubsSection();
    void setupTopUsersSection();
    void setupNavigation();

    QLabel* createMedalLabel(const QString &color, bool withRibbon);
    QFrame* createRoundedFrame();

    // Main layout
    QVBoxLayout *mainLayout;

    // UI elements
    QLabel *titleLabel;

    // Stats section
    QFrame *clubsFrame;
    QLabel *totalClubsLabel;
    QLabel *clubsCountLabel;
    QFrame *usersFrame;
    QLabel *totalUsersLabel;
    QLabel *usersCountLabel;

    // Top clubs section
    QFrame *topClubsFrame;
    QLabel *thisWeekClubsLabel;
    QLabel *topClubsLabel;
    QLabel *goldMedal1;
    QLabel *silverMedal1;
    QLabel *bronzeMedal1;
    QLabel *firstClubLabel;
    QLabel *secondClubLabel;
    QLabel *thirdClubLabel;


    // Top users section
    QFrame *topUsersFrame;
    QLabel *thisWeekUsersLabel;
    QLabel *topUsersLabel;
    QLabel *goldMedal2;
    QLabel *silverMedal2;
    QLabel *bronzeMedal2;
    QLabel *firstUserLabel;
    QLabel *secondUserLabel;
    QLabel *thirdUserLabel;

    // Navigation
    QFrame *navigationFrame;
    QPushButton *homeButton;
    QPushButton *profileButton;
    QPushButton *groupsButton;
};

#endif // ADMINHOME_H
