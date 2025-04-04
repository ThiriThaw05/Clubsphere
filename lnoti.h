#ifndef LNOTI_H
#define LNOTI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QFrame>
#include <QDateTime>

// Updated notification struct to match database schema
class Notification {
public:
    int clubId;
    int userId;
    QString content;
    bool isRequest; // 1 = request, 0 = notification
    qint64 timestamp;
    QString username;

    Notification(int cId, int uId, const QString &cont, bool isReq, qint64 time, const QString &name)
        : clubId(cId), userId(uId), content(cont), isRequest(isReq), timestamp(time), username(name) {}
};

class LNoti : public QWidget
{
    Q_OBJECT

public:
    explicit LNoti(int clubId, QWidget *parent=nullptr);
    ~LNoti();
    void prepareForDestruction();
signals:
    void navigateBack();

private slots:
    void onBackClicked();
    void onAcceptClicked(int index);
    void onRejectClicked(int index);

private:
    QPixmap createCircularPixmap(const QPixmap &srcPixmap);
    QPixmap getUserProfilePhoto(int userId);
    void setupUI();
    void setupNotificationsArea();
    void loadNotificationsFromDatabase();
    void updateUserClubStatus(int userId, int clubId, bool accepted);
    void clearNotifications();
    QWidget* createNotificationWidget(const Notification &notification, int index);
    QWidget* createHeaderWidget();
    QString formatTimestamp(qint64 timestamp);

    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_scrollContent;
    QVBoxLayout *m_notificationsLayout;

    QVector<Notification> m_notifications;
    QVector<QWidget*> m_notificationWidgets;

    int m_clubId; // The clubId passed to this page
};

#endif // LNOTI_H
