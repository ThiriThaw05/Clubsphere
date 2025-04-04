#ifndef ADMINCLUB_H
#define ADMINCLUB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QFont>
#include "Club.h"
#include  "database.h"
#include "AddClubDialog.h"

class AdminClub : public QWidget
{
    Q_OBJECT

public:
    explicit AdminClub(QWidget *parent = nullptr);
    ~AdminClub();

signals:
    void navigateToHome();
    void navigateToMembers();

private slots:
    void onHomeButtonClicked();
    void onProfileButtonClicked();
    void onGroupsButtonClicked();
    void onAddClubClicked();
    void searchClubs(const QString &searchText);

private:
    void setupUI();
    void setupClubList();
    void setupNavigation();
    void createClubCard(const QString &name, int clubId, int members, const QString &leader);
    void refreshClubList();
    void createClubCard(const Club& club);
    void setupSearchFunctionality();
    void deleteClub(int clubId);

    QFrame* createRoundedFrame();

    // Main layout
    QVBoxLayout *mainLayout;

    // Title and search
    QLabel *titleLabel;
    QFrame *searchFrame;
    QLineEdit *searchEdit;

    // Club list
    QHBoxLayout *clubsHeaderLayout;
    QLabel *clubsLabel;
    QPushButton *addClubButton;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *clubsLayout;

    // Navigation bar
    QFrame *navigationFrame;
    QPushButton *homeButton;
    QPushButton *profileButton;
    QPushButton *groupsButton;
};

#endif // ADMINCLUB_H
