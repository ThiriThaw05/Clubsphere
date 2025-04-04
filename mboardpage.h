#ifndef MBOARDPAGE_H
#define MBOARDPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollArea>
#include<QRadioButton>

class MBoardPage : public QWidget
{
    Q_OBJECT

public:
    explicit MBoardPage(QWidget *parent = nullptr);
    ~MBoardPage();

signals:
    void navigateToHome();
    void navigateToClub();
    void navigateToGoing();
    void navigateToProfile();
    void navigateToBoard();

private slots:
    void onMembersTabClicked();
    void onClubsTabClicked();
    void homeClicked();
    void clubClicked();
    void eventClicked();
    void profileClicked();
    void boardClicked();

private:
    void setupUI();
    void setupTopLeaders();
    void setupListItems();
    QWidget* createLeaderItem(const QString& name, int points, bool isTopThree = false, int rank = 0);
    void clearLeaderboard();
    void showMembersLeaderboard();
    void showClubsLeaderboard();

    // UI components
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QButtonGroup* m_tabGroup;
    QRadioButton* m_membersTab;
    QRadioButton* m_clubsTab;
    QWidget* m_topThreeWidget;
    QScrollArea* m_scrollArea;
    QVBoxLayout* m_leaderListLayout;

    // State tracking
    bool m_showingMembers;
};

#endif // MBOARDPAGE_H
