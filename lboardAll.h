#ifndef LEADERBOARDALL_H
#define LEADERBOARDALL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QSqlQuery>
#include <QPixmap>
#include <QPainter>

class LeaderboardAll : public QWidget
{
    Q_OBJECT

public:
    enum TabType { ClubMembers, Members, Clubs };

    explicit LeaderboardAll(int club_id, QWidget *parent = nullptr);
    ~LeaderboardAll();

signals:
    void navigateBack();

private slots:
    void onClubMembersTabClicked();
    void onMembersTabClicked();
    void onClubsTabClicked();

private:
    void setupUI();
    void setupTopLeaders();
    void setupListItems();
    void clearLeaderboard();
    QWidget* createLeaderItem(const QString& name, int idOrPoints, bool isTopThree, int rank);
    void showClubMembersLeaderboard();
    void showMembersLeaderboard();
    void showClubsLeaderboard();

    int m_clubId;
    TabType m_currentTab;

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QButtonGroup* m_tabGroup;
    QRadioButton* m_clubMembersTab;
    QRadioButton* m_membersTab;
    QRadioButton* m_clubsTab;
    QWidget* m_topThreeWidget;
    QScrollArea* m_scrollArea;
    QVBoxLayout* m_leaderListLayout;
};

#endif // LEADERBOARDALL_H
