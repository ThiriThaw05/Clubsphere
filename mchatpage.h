#ifndef MCHATPAGE_H
#define MCHATPAGE_H

#include <QWidget>
#include <QDateTime>
#include <QMap>

class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QLabel;
class QScrollArea;
class QResizeEvent;

// MBubbleMessage class for chat bubbles
class MBubbleMessage : public QWidget
{
    Q_OBJECT

public:
    enum MessageType { Incoming, Outgoing };

    MBubbleMessage(const QString &text, const QString &sender, const QDateTime &time, MessageType type, QWidget *parent = nullptr);
    QPixmap createAvatar(const QString &letter, const QString &color);

private:
    QLabel *messageLabel;
    QLabel *timeLabel;
    QLabel *senderLabel;
    MessageType messageType;
};

class MChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit MChatPage(int clubId, int currentUserId,QWidget *parent = nullptr);
    ~MChatPage();

signals:
    void navigateToClub();


private:
    void setupUI();
    void setupHeader();
    void setupMessagesArea();
    void setupInputArea();
    void loadInitialMessages();
    QPixmap createGroupAvatar(const QString &name, const QString &color);
    void prepareForDestruction();

private slots:
    void sendMessage();
    void addMessage(const QString &text, const QString &sender, bool isOutgoing, const QDateTime &timestamp = QDateTime::currentDateTime());

private:
    QVBoxLayout *mainLayout;
    QWidget *headerWidget;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *messagesLayout;
    QWidget *inputWidget;

    QPushButton *backButton;
    QLabel *groupAvatarLabel;
    QLabel *groupNameLabel;
    QLabel *statusLabel;

    QLineEdit *messageInput;
    QPushButton *sendButton;

    QStringList participants;
    QMap<QString, QString> avatarColors;

    int m_clubId;
    int m_currentUserId;
    QString m_clubName;
};

#endif // MCHATPAGE_H
