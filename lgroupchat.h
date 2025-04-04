// lgroupchat.h
#ifndef LGROUPCHAT_H
#define LGROUPCHAT_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QDateTime>

// Message bubble for chat
class BubbleMessage : public QWidget
{
    Q_OBJECT

public:
    enum MessageType { Incoming, Outgoing };

    BubbleMessage(const QString &text, const QString &sender, const QDateTime &time, MessageType type, QWidget *parent = nullptr);
    QPixmap createAvatar(const QString &letter, const QString &color);

private:
    QLabel *messageLabel;
    QLabel *timeLabel;
    QLabel *senderLabel;
    MessageType messageType;
};

// Main chat window class
class LGroupChat : public QWidget
{
    Q_OBJECT

public:
    explicit LGroupChat(int clubId, int currentUserId, QWidget *parent = nullptr);
    ~LGroupChat();
    void prepareForDestruction();

signals:
    void navigateToHomePage();

private slots:
    void sendMessage();
    void goBack();

private:
    void setupUI();
    void setupHeader();
    void setupMessagesArea();
    void setupInputArea();
    void loadMessagesFromDatabase();
    void addMessage(const QString &text, const QString &sender, bool isOutgoing, const QDateTime &timestamp = QDateTime::currentDateTime());
    QPixmap createAvatar(const QString &name, const QString &color);

    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *messagesLayout;

    QWidget *headerWidget;
    QLabel *groupAvatarLabel;
    QLabel *groupNameLabel;
    QLabel *statusLabel;
    QPushButton *backButton;

    QWidget *inputWidget;
    QLineEdit *messageInput;
    QPushButton *sendButton;

    QStringList participants;
    QMap<QString, QString> avatarColors;

    // New variables for database integration
    int m_clubId;
    int m_currentUserId;
    QString m_clubName;
};

#endif // LGROUPCHAT_H
