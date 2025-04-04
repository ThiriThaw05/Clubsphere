#ifndef ADMINMEMBER_H
#define ADMINMEMBER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QFrame>

class AdminMember : public QWidget
{
    Q_OBJECT

public:
    explicit AdminMember(QWidget *parent = nullptr);
    ~AdminMember();

signals:
    void navigateToHome();
    void navigateToClubs();

private slots:
    void onHomeButtonClicked();
    void onProfileButtonClicked();
    void onGroupsButtonClicked();
    void searchMembers(const QString &searchText);

private:
    // UI components
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QFrame *searchFrame;
    QLineEdit *searchEdit;
    QLabel *membersLabel;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *membersLayout;
    QFrame *navigationFrame;
    QPushButton *homeButton;
    QPushButton *profileButton;
    QPushButton *groupsButton;

    // Methods
    void setupUI();
    void setupMemberList();
    void setupNavigation();
    void setupSearchFunctionality();
    QFrame* createRoundedFrame();
    void createMemberCard(const QString &name, const QString &id, int points, const QByteArray &profileImageData = QByteArray());
};

#endif // ADMINMEMBER_H
