#ifndef MCLUBPAGE_H
#define MCLUBPAGE_H

#include <QWidget>
#include <QVector>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLineEdit;
class QStackedWidget;
class QLayout;

class MClubPage : public QWidget
{
    Q_OBJECT

public:
    explicit MClubPage(int userId, QWidget *parent = nullptr);
    ~MClubPage();

    // Getters for testing or accessing properties
    int userId() const;
    QStackedWidget* stackedWidget() const;
    QPushButton* joinedButton() const;
    QPushButton* pendingButton() const;
    QLineEdit* searchInput() const;

signals:
    void navigateToGoing();
    void navigateToHome();
    void navigateToClub();
    void navigateToEvent(int clubId);
    void navigateToChat(int clubId);
    void navigateToProfile();


private slots:
    void showJoinedView();
    void showPendingView();
    void filterClubs(const QString& filterText);
    void homeClicked();
    void clubClicked();
    void eventClicked();
    void profileClicked();
    void leaveClub(int clubId);
    void cancelClubRequest(int clubId);
    void calendarClicked();
    void chatClicked();

private:
    void setupUI();
    void loadUserClubs();
    void clearContentLayout(QLayout* layout);
    void showAllClubs(QLayout* layout);
    QPixmap createCircularPixmap(const QPixmap& pixmap, int size);
    void loadClubs(const QVector<int>& clubIds, bool isJoined);
    QWidget* createClubItem(int clubId, const QString& name, const QString& rank,
                                       const QString& members, bool isPending, bool isJoinedView);
    void filterClubsByText(QLayout* layout, const QString& filterText);
    QWidget* createSeparator();

    int m_userId;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_bottomNavLayout;
    QPushButton* m_joinedButton;
    QPushButton* m_pendingButton;
    QLineEdit* m_searchInput;
    QStackedWidget* m_stackedWidget;
    QWidget* m_bottomNavBar;
    QWidget* m_joinedContentWidget;
    QWidget* m_pendingContentWidget;
    QVBoxLayout* m_joinedContentLayout;
    QVBoxLayout* m_pendingContentLayout;
};

#endif // MCLUBPAGE_H
