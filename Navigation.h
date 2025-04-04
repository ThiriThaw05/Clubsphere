#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class NavigationBar : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationBar(QWidget *parent = nullptr);

    // Add accessor methods for the buttons
    QPushButton* getHomeButton() const { return homeButton; }
    QPushButton* getProfileButton() const { return profileButton; }
    QPushButton* getGroupsButton() const { return groupsButton; }
    QPushButton* getNotificationsButton() const { return notificationsButton; }

signals:
    void navigateToHome();
    void navigateToProfile();
    void navigateToGroups();
    void navigateToNotifications();

private slots:
    void onHomeClicked();
    void onProfileClicked();
    void onGroupsClicked();
    void onNotificationsClicked();

private:
    QPushButton *homeButton;
    QPushButton *profileButton;
    QPushButton *groupsButton;
    QPushButton *notificationsButton;
};

#endif
