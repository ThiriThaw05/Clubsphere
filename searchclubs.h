#ifndef SEARCHCLUBS_H
#define SEARCHCLUBS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>
#include <QIcon>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QVector>

class ClubCard : public QFrame
{
    Q_OBJECT

public:
    ClubCard(int clubId, const QString& clubName, int memberCount, const QString& leaderName,
             const QByteArray& photoData, bool isJoined, bool isPending, QWidget* parent = nullptr);

    // Getters for filtering
    QString getClubName() const { return m_clubName; }
    int getClubId() const { return m_clubId; }

    // Update button state
    void updateButtonState(bool isJoined, bool isPending);

signals:
    void joinClicked(int clubId, bool isPending);

private slots:
    void onJoinButtonClicked();

private:
    QLabel* m_clubImageLabel;
    QLabel* m_clubNameLabel;
    QLabel* m_memberCountLabel;
    QLabel* m_rankingLabel;
    QLabel* m_leaderLabel;
    QPushButton* m_joinButton;

    int m_clubId;
    QString m_clubName;
    bool m_isPending;
    bool m_isJoined;
};

class SearchClubs : public QWidget
{
    Q_OBJECT

public:
    explicit SearchClubs(int currentUserId, QWidget *parent = nullptr);
    ~SearchClubs();

signals:
    void navigateToHome();

private slots:
    void filterClubs(const QString& text);
    void handleJoinButtonClicked(int clubId, bool isPending);
    void refreshClubCards();

private:
    void setupUI();
    void loadClubsFromDatabase();
    void addClubCard(int clubId, const QString& clubName, int memberCount,
                     const QString& leaderName, const QByteArray& photoData,
                     bool isJoined, bool isPending);
    QVector<int> getUserJoinedClubs();
    QVector<int> getUserPendingClubs();
    bool updateUserPendingClubs(const QVector<int>& pendingClubs);

    QVBoxLayout* m_mainLayout;
    QLineEdit* m_searchBar;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_clubsLayout;

    int m_currentUserId;
    QVector<int> m_joinedClubs;
    QVector<int> m_pendingClubs;
};

#endif // SEARCHCLUBS_H
