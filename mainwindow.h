#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "homePage.h"
#include "registerPage.h"
#include "loginPage.h"
#include "adminHome.h"
#include "adminMember.h"
#include "adminClub.h"
#include "lhomepage.h"
#include "lgroupchat.h"
#include "lnoti.h"
#include "lboardAll.h"
#include "mhomepage.h"
#include "mclubpage.h"
#include "mgoingpage.h"
#include "mprofilepage.h"
#include "mboardpage.h"
#include "meventpage.h"
#include "mchatpage.h"
#include "searchclubs.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation methods for home/auth pages
    void showHomePage();
    void showRegisterPage();
    void showLoginPage();

    // Navigation methods for admin pages
    void showAdminHomePage();
    void showAdminMemberPage();
    void showAdminClubPage();

    // Navigation methods for leader pages
    void showLeaderHomePage();
    void showLeaderGroupChat();
    void showLeaderNotifications();
    void showLeaderboard(int clubId);

    // Navigation methods for member pages
    void showMemberHomePage();
    void showMemberClubPage();
    void showMemberGoingPage();
    void showMemberProfilePage();
    void showMemberBoardPage();
    void showMemberEventPage(int clubId);
    void showMemberChatPage(int clubId);

    // Authentication handlers
    void handleRegisterSuccess(int userId);
    void handleLoginSuccess(int userId);

private:
    // Initialize original pages
    void initializeHomePage();
    void initializeRegisterPage();
    void initializeLoginPage();
    void initializeAdminHomePage();
    void initializeAdminMemberPage();
    void initializeAdminClubPage();


    // Initialize leader pages
    void initializeLeaderHomePage();
    void initializeLeaderGroupChat();
    void initializeLeaderNotifications();
    void initializeLeaderboardPage(int clubId);

    // Initialize member pages
    void initializeMemberHomePage();
    void initializeMemberClubPage();
    void initializeMemberGoingPage();
    void initializeMemberProfilePage();
    void initializeMemberBoardPage();
    void initializeMemberEventPage(int clubId);
    void initializeMemberChatPage(int clubId);
    void initializeSearchClubsPage();
    void showSearchClubsPage();

    // Cleanup method
    void cleanupUnusedPages(QWidget* currentPage);

    // Check user role
    bool isUserLeader(int userId);

    // Original page pointers
    HomePage* homePage;
    RegisterPage* registerPage;
    LoginPage* loginPage;
    AdminHome* adminHome;
    AdminMember* adminMember;
    AdminClub* adminClub;


    // Leader page pointers
    LHomepage* leaderHomePage;
    LGroupChat* leaderGroupChat;
    LNoti* leaderNotifications;
    LeaderboardAll* leaderboardPage;

    // Member page pointers (new)
    MHomepage* memberHomePage;
    MClubPage* memberClubPage;
    MGoingPage* memberGoingPage;
    MProfilePage* memberProfilePage;
    MBoardPage* memberBoardPage;
    MEventPage* memberEventPage;
    MChatPage* memberChatPage;
    SearchClubs* searchClubsPage;


    // QStackedWidget pointer for member pages - not using
    QStackedWidget* m_stackedWidget;

    // Currently logged in user ID (0 if not logged in)
    int currentUserId;
};

#endif // MAINWINDOW_H

