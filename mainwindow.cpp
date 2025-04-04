#include "mainwindow.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    homePage(nullptr), registerPage(nullptr), loginPage(nullptr),
    adminHome(nullptr), adminMember(nullptr), adminClub(nullptr),
    leaderHomePage(nullptr), leaderGroupChat(nullptr), leaderNotifications(nullptr), leaderboardPage(nullptr),
    memberHomePage(nullptr), memberClubPage(nullptr), memberGoingPage(nullptr),
    memberProfilePage(nullptr), memberBoardPage(nullptr), memberEventPage(nullptr), memberChatPage(nullptr),
    searchClubsPage(nullptr),currentUserId(0)
{
    // Set initial window properties
    this->setWindowTitle("Club Management System");
    this->setFixedSize(350, 650);

    // Initialize only the home page on startup
    initializeHomePage();
    homePage->show();
}

MainWindow::~MainWindow()
{
    // Clean up any pages that might still exist
    if (homePage) delete homePage;
    if (registerPage) delete registerPage;
    if (loginPage) delete loginPage;
    if (adminHome) delete adminHome;
    if (adminMember) delete adminMember;
    if (adminClub) delete adminClub;
    if (leaderHomePage) delete leaderHomePage;
    if (leaderGroupChat) delete leaderGroupChat;
    if (leaderNotifications) delete leaderNotifications;
    if (leaderboardPage) delete leaderboardPage;
    if (memberHomePage) delete memberHomePage;
    if (memberClubPage) delete memberClubPage;
    if (memberGoingPage) delete memberGoingPage;
    if (memberProfilePage) delete memberProfilePage;
    if (memberBoardPage) delete memberBoardPage;
    if (memberEventPage) delete memberEventPage;
    if (memberChatPage) delete memberChatPage;
    if (searchClubsPage) delete searchClubsPage;
}

// Initialize admin pages
void MainWindow::initializeHomePage()
{
    if (!homePage) {
        homePage = new HomePage();
        // Connect HomePage signals
        connect(homePage, &HomePage::openRegisterPage, this, &MainWindow::showRegisterPage);
        connect(homePage, &HomePage::openLoginPage, this, &MainWindow::showLoginPage);
    }
}

void MainWindow::initializeRegisterPage()
{
    if (!registerPage) {
        registerPage = new RegisterPage();
        // Connect RegisterPage signals
        connect(registerPage, &RegisterPage::navigateToHome, this, &MainWindow::showHomePage);
        connect(registerPage, &RegisterPage::openadminHome, this, &MainWindow::handleRegisterSuccess);
    }
}

void MainWindow::initializeLoginPage()
{
    if (!loginPage) {
        loginPage = new LoginPage();
        // Connect LoginPage signals
        connect(loginPage, &LoginPage::navigateToHome, this, &MainWindow::showHomePage);
        connect(loginPage, &LoginPage::openadminHome, this, &MainWindow::handleLoginSuccess);
    }
}

void MainWindow::initializeAdminHomePage()
{
    if (!adminHome) {
        adminHome = new AdminHome();
        // Connect AdminHome signals
        connect(adminHome, &AdminHome::navigateToMembers, this, &MainWindow::showAdminMemberPage);
        connect(adminHome, &AdminHome::navigateToClubs, this, &MainWindow::showAdminClubPage);

    }
}

void MainWindow::initializeAdminMemberPage()
{
    if (!adminMember) {
        adminMember = new AdminMember();
        // Connect AdminMember signals
        connect(adminMember, &AdminMember::navigateToHome, this, &MainWindow::showAdminHomePage);
        connect(adminMember, &AdminMember::navigateToClubs, this, &MainWindow::showAdminClubPage);

    }
}

void MainWindow::initializeAdminClubPage()
{
    if (!adminClub) {
        adminClub = new AdminClub();
        // Connect AdminClub signals
        connect(adminClub, &AdminClub::navigateToHome, this, &MainWindow::showAdminHomePage);
        connect(adminClub, &AdminClub::navigateToMembers, this, &MainWindow::showAdminMemberPage);

    }
}


void MainWindow::initializeLeaderHomePage()
{
    if (!leaderHomePage) {
        qDebug() << "Initializing Leader Home Page";
        leaderHomePage = new LHomepage(currentUserId); // Pass the currentUserId as leaderId
        // Connect signals
        connect(leaderHomePage, &LHomepage::showGroupChat, this, &MainWindow::showLeaderGroupChat);
        connect(leaderHomePage, &LHomepage::showNotifications, this, &MainWindow::showLeaderNotifications);
        connect(leaderHomePage, &LHomepage::showLeaderboard, this, &MainWindow::showLeaderboard);
    }
}

void MainWindow::initializeLeaderGroupChat()
{
    // Find the club associated with this leader
    QSqlQuery query;
    query.prepare("SELECT club_id, club_name FROM clubs_list WHERE leader_id = :currentUserId");
    query.bindValue(":currentUserId", currentUserId);
    int clubId;

    if (query.exec() && query.next()) {
        clubId = query.value(0).toInt();
    } else {
        qDebug() << "Error finding club for leader:" << query.lastError().text();
        clubId = -1;
    }

    if (!leaderGroupChat) {
        qDebug() << "Initializing Leader Group Chat";
        leaderGroupChat = new LGroupChat(clubId, currentUserId);
        // Connect signals
        connect(leaderGroupChat, &LGroupChat::navigateToHomePage, this, &MainWindow::showLeaderHomePage);
    }
}

void MainWindow::initializeLeaderNotifications()
{
    QSqlQuery query;
    query.prepare("SELECT club_id, club_name FROM clubs_list WHERE leader_id = :currentUserId");
    query.bindValue(":currentUserId", currentUserId);
    int clubId;

    if (query.exec() && query.next()) {
        clubId = query.value(0).toInt();
    } else {
        qDebug() << "Error finding club for leader:" << query.lastError().text();
        clubId = -1;
    }
    if (!leaderNotifications) {
        qDebug() << "Initializing Leader Notifications Page";
        leaderNotifications = new LNoti(clubId);
        // Connect signals
        connect(leaderNotifications, &LNoti::navigateBack, this, &MainWindow::showLeaderHomePage);
    }
}

void MainWindow::initializeLeaderboardPage(int clubId)
{
    if (!leaderboardPage) {
        qDebug() << "Initializing Leaderboard Page";
        leaderboardPage = new LeaderboardAll(clubId);
        // Connect signals
        connect(leaderboardPage, &LeaderboardAll::navigateBack, this, &MainWindow::showLeaderHomePage);

    }

}

// Initialize new member pages
void MainWindow::initializeMemberHomePage()
{
    if (!memberHomePage) {
        memberHomePage = new MHomepage(currentUserId);
        // Connect navigation signals
        connect(memberHomePage, &MHomepage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberHomePage, &MHomepage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberHomePage, &MHomepage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberHomePage, &MHomepage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
        connect(memberHomePage, &MHomepage::navigateToBoard, this, &MainWindow::showMemberBoardPage);
        connect(memberHomePage, &MHomepage::navigateToSearchClubs, this, &MainWindow::showSearchClubsPage);
    }
}

void MainWindow::initializeMemberClubPage()
{
    if (!memberClubPage) {
        qDebug() << "Initializing Member Club Page";
        memberClubPage = new MClubPage(currentUserId);
        // Connect navigation signals
        connect(memberClubPage, &MClubPage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberClubPage, &MClubPage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberClubPage, &MClubPage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberClubPage, &MClubPage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
        connect(memberClubPage, &MClubPage::navigateToEvent, this, &MainWindow::showMemberEventPage);
        connect(memberClubPage, &MClubPage::navigateToChat, this, &MainWindow::showMemberChatPage);
    }
}

void MainWindow::initializeMemberGoingPage()
{
    if (!memberGoingPage) {
        qDebug() << "Initializing Member Going Page";
        memberGoingPage = new MGoingPage(currentUserId);
        // Connect navigation signals
        connect(memberGoingPage, &MGoingPage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberGoingPage, &MGoingPage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberGoingPage, &MGoingPage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberGoingPage, &MGoingPage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
    }
}

void MainWindow::initializeMemberProfilePage()
{
    if (!memberProfilePage) {
        qDebug() << "Initializing Member Profile Page";
        memberProfilePage = new MProfilePage(currentUserId);
        // Connect navigation signals
        connect(memberProfilePage, &MProfilePage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberProfilePage, &MProfilePage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberProfilePage, &MProfilePage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberProfilePage, &MProfilePage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
    }
}

void MainWindow::initializeMemberBoardPage()
{
    if (!memberBoardPage) {
        qDebug() << "Initializing Member Board Page";
        memberBoardPage = new MBoardPage();
        // Connect navigation signals
        connect(memberBoardPage, &MBoardPage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberBoardPage, &MBoardPage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberBoardPage, &MBoardPage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberBoardPage, &MBoardPage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
        connect(memberBoardPage, &MBoardPage::navigateToBoard, this, &MainWindow::showMemberBoardPage);
    }
}

void MainWindow::initializeMemberEventPage(int clubId)

{
    if (!memberEventPage) {
        qDebug() << "Initializing Member Event Page";
        memberEventPage = new MEventPage(clubId , currentUserId );
        // Connect navigation signals
        connect(memberEventPage, &MEventPage::navigateToHome, this, &MainWindow::showMemberHomePage);
        connect(memberEventPage, &MEventPage::navigateToClub, this, &MainWindow::showMemberClubPage);
        connect(memberEventPage, &MEventPage::navigateToGoing, this, &MainWindow::showMemberGoingPage);
        connect(memberEventPage, &MEventPage::navigateToProfile, this, &MainWindow::showMemberProfilePage);
    }
}

void MainWindow::initializeMemberChatPage(int clubId)
{
    if (!memberChatPage) {
        qDebug() << "Initializing Member Chat Page";
        memberChatPage = new MChatPage(clubId, currentUserId);
        // Connect signals
        connect(memberChatPage, &MChatPage::navigateToClub, this, &MainWindow::showMemberClubPage);
    }
}

void MainWindow::initializeSearchClubsPage()
{
    if (!searchClubsPage) {
        qDebug() << "Initializing Search Clubs Page";
        searchClubsPage = new SearchClubs(currentUserId);
        // Connect navigation signals
        connect(searchClubsPage, &SearchClubs::navigateToHome, this, &MainWindow::showMemberHomePage);

    }
}



// Navigation methods for home/auth pages
void MainWindow::showHomePage()
{
    qDebug() << "Showing Home Page";
    initializeHomePage();
    homePage->show();
    // Clean up unused pages
    cleanupUnusedPages(homePage);
}

void MainWindow::showRegisterPage()
{
    qDebug() << "Showing Register Page";
    initializeRegisterPage();
    registerPage->show();
    // Clean up unused pages
    cleanupUnusedPages(registerPage);
}

void MainWindow::showLoginPage()
{
    qDebug() << "Showing Login Page";
    initializeLoginPage();
    loginPage->show();
    // Clean up unused pages
    cleanupUnusedPages(loginPage);
}

// Navigation methods for admin pages
void MainWindow::showAdminHomePage()
{
    qDebug() << "Showing Admin Home Page";
    initializeAdminHomePage();
    adminHome->show();
    adminHome->raise();
    adminHome->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(adminHome);
}

void MainWindow::showAdminMemberPage()
{
    qDebug() << "Showing Admin Member Page";
    initializeAdminMemberPage();
    adminMember->show();
    adminMember->raise();
    adminMember->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(adminMember);
}

void MainWindow::showAdminClubPage()
{
    qDebug() << "Showing Admin Club Page";
    initializeAdminClubPage();
    adminClub->show();
    adminClub->raise();
    adminClub->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(adminClub);
}

// Navigation methods for leader pages
void MainWindow::showLeaderHomePage()
{
    qDebug() << "Showing Leader Home Page";
    initializeLeaderHomePage();
    leaderHomePage->show();
    leaderHomePage->raise();
    leaderHomePage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(leaderHomePage);
}

void MainWindow::showLeaderGroupChat()
{
    qDebug() << "Showing Leader Group Chat";
    initializeLeaderGroupChat();
    leaderGroupChat->show();
    leaderGroupChat->raise();
    leaderGroupChat->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(leaderGroupChat);
}

void MainWindow::showLeaderNotifications()
{
    qDebug() << "Showing Leader Notifications";
    initializeLeaderNotifications();
    leaderNotifications->show();
    leaderNotifications->raise();
    leaderNotifications->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(leaderNotifications);
}

void MainWindow::showLeaderboard(int clubId)
{
    qDebug() << "Showing Leaderboard";
    initializeLeaderboardPage(clubId);
    leaderboardPage->show();
    leaderboardPage->raise();
    leaderboardPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(leaderboardPage);
}

// Navigation methods for member pages
void MainWindow::showMemberHomePage()
{
    qDebug() << "Showing Member Home Page";
    initializeMemberHomePage();
    memberHomePage->show();
    memberHomePage->raise();
    memberHomePage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberHomePage);
}

void MainWindow::showMemberClubPage()
{
    qDebug() << "Showing Member Club Page";
    initializeMemberClubPage();
    memberClubPage->show();
    memberClubPage->raise();
    memberClubPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberClubPage);
}

void MainWindow::showMemberGoingPage()
{
    qDebug() << "Showing Member Going Page";
    initializeMemberGoingPage();
    memberGoingPage->show();
    memberGoingPage->raise();
    memberGoingPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberGoingPage);
}

void MainWindow::showMemberProfilePage()
{
    qDebug() << "Showing Member Profile Page";
    initializeMemberProfilePage();
    memberProfilePage->show();
    memberProfilePage->raise();
    memberProfilePage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberProfilePage);
}

void MainWindow::showMemberBoardPage()
{
    qDebug() << "Showing Member Board Page";
    initializeMemberBoardPage();
    memberBoardPage->show();
    memberBoardPage->raise();
    memberBoardPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberBoardPage);
}

void MainWindow::showMemberEventPage(int clubId)
{
    qDebug() << "Showing Member Event Page";
    initializeMemberEventPage(clubId);
    memberEventPage->show();
    memberEventPage->raise();
    memberEventPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberEventPage);
}

void MainWindow::showMemberChatPage(int clubId)
{
    qDebug() << "Showing Member Chat Page";
    initializeMemberChatPage(clubId);
    memberChatPage->show();
    memberChatPage->raise();
    memberChatPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(memberChatPage);
}

// Authentication handlers
void MainWindow::handleRegisterSuccess(int userId)
{
    qDebug() << "Registration successful for user ID:" << userId;
    currentUserId = userId;

    // Special admin ID check
    if (userId == 67001922) {
        qDebug() << "User is admin, navigating to admin home page";
        showAdminHomePage();
    }
    else if (isUserLeader(userId)) {
        qDebug() << "User is a leader, navigating to leader home page";
        showLeaderHomePage();
    } else {
        qDebug() << "User is a regular member, navigating to member home page";
        showMemberHomePage();
    }
}

void MainWindow::handleLoginSuccess(int userId)
{
    qDebug() << "Login successful for user ID:" << userId;
    currentUserId = userId;

    // Special admin ID check
    if (userId == 67001922) {
        qDebug() << "User is admin, navigating to admin home page";
        showAdminHomePage();
    }
    else if (isUserLeader(userId)) {
        qDebug() << "User is a leader, navigating to leader home page";
        showLeaderHomePage();
    } else {
        qDebug() << "User is a regular member, navigating to member home page";
        showMemberHomePage();
    }
}

// Check if user is a leader
bool MainWindow::isUserLeader(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM clubleaders_list WHERE leader_id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    qDebug() << "Error checking leader status:" << query.lastError().text();
    return false;
}


void MainWindow::showSearchClubsPage()
{
    qDebug() << "Showing Search Clubs Page";
    initializeSearchClubsPage();
    searchClubsPage->show();
    searchClubsPage->raise();
    searchClubsPage->activateWindow();
    // Clean up unused pages
    cleanupUnusedPages(searchClubsPage);
}

// Function to destroy unused pages to free memory
void MainWindow::cleanupUnusedPages(QWidget* currentPage)
{
    // Don't destroy the current page
    // Original pages
    if (homePage && homePage != currentPage) {
        delete homePage;
        homePage = nullptr;
        qDebug() << "Home page destroyed";
    }

    if (registerPage && registerPage != currentPage) {
        delete registerPage;
        registerPage = nullptr;
        qDebug() << "Register page destroyed";
    }

    if (loginPage && loginPage != currentPage) {
        delete loginPage;
        loginPage = nullptr;
        qDebug() << "Login page destroyed";
    }

    if (adminHome && adminHome != currentPage) {
        delete adminHome;
        adminHome = nullptr;
        qDebug() << "Admin home page destroyed";
    }

    if (adminMember && adminMember != currentPage) {
        delete adminMember;
        adminMember = nullptr;
        qDebug() << "Admin member page destroyed";
    }

    if (adminClub && adminClub != currentPage) {
        delete adminClub;
        adminClub = nullptr;
        qDebug() << "Admin club page destroyed";
    }


    if (leaderHomePage && leaderHomePage != currentPage) {
        delete leaderHomePage;
        leaderHomePage = nullptr;
        qDebug() << "Leader home page destroyed";
    }

    if (leaderGroupChat && leaderGroupChat != currentPage) {
        delete leaderGroupChat;
        leaderGroupChat = nullptr;
        qDebug() << "Leader group chat destroyed";
    }

    if (leaderNotifications && leaderNotifications != currentPage) {
        delete leaderNotifications;
        leaderNotifications = nullptr;
        qDebug() << "Leader notifications destroyed";
    }

    if (leaderboardPage && leaderboardPage != currentPage) {
        delete leaderboardPage;
        leaderboardPage = nullptr;
        qDebug() << "Leaderboard page destroyed";
    }

    // Member pages
    if (memberHomePage && memberHomePage != currentPage) {
        delete memberHomePage;
        memberHomePage = nullptr;
        qDebug() << "Member home page destroyed";
    }

    if (memberClubPage && memberClubPage != currentPage) {
        delete memberClubPage;
        memberClubPage = nullptr;
        qDebug() << "Member club page destroyed";
    }

    if (memberGoingPage && memberGoingPage != currentPage) {
        delete memberGoingPage;
        memberGoingPage = nullptr;
        qDebug() << "Member going page destroyed";
    }

    if (memberProfilePage && memberProfilePage != currentPage) {
        delete memberProfilePage;
        memberProfilePage = nullptr;
        qDebug() << "Member profile page destroyed";
    }

    if (memberBoardPage && memberBoardPage != currentPage) {
        delete memberBoardPage;
        memberBoardPage = nullptr;
        qDebug() << "Member board page destroyed";
    }

    if (memberEventPage && memberEventPage != currentPage) {
        delete memberEventPage;
        memberEventPage = nullptr;
        qDebug() << "Member event page destroyed";
    }

    if (memberChatPage && memberChatPage != currentPage) {
        delete memberChatPage;
        memberChatPage = nullptr;
        qDebug() << "Member chat page destroyed";
    }

    if (searchClubsPage && searchClubsPage != currentPage) {
        delete searchClubsPage;
        searchClubsPage = nullptr;
        qDebug() << "Search clubs page destroyed";
    }

}
