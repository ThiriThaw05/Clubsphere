// Event.h
#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDateTime>

class Event {
public:
    Event();
    Event(int id, int clubId, const QString& content, const QByteArray& photo = QByteArray(),
          const QString& code = QString(), int goingCount = 0,
          const QDateTime& timestamp = QDateTime::currentDateTime());

    // Getters
    int getId() const { return eventId; }
    int getClubId() const { return clubId; }
    QString getContent() const { return eventContent; }
    QByteArray getPhoto() const { return eventPhoto; }
    QString getCode() const { return eventCode; }
    int getGoingCount() const { return eventGoingCount; }
    QDateTime getCreatedTimestamp() const { return createdTimestamp; }

    // Setters
    void setId(int id) { eventId = id; }
    void setClubId(int id) { clubId = id; }
    void setContent(const QString& content) { eventContent = content; }
    void setPhoto(const QByteArray& photo) { eventPhoto = photo; }
    void setCode(const QString& code) { eventCode = code; }
    void setGoingCount(int count) { eventGoingCount = count; }
    void setCreatedTimestamp(const QDateTime& timestamp) { createdTimestamp = timestamp; }

    // Attendance methods
    void incrementGoingCount() { eventGoingCount++; }
    void decrementGoingCount() { if (eventGoingCount > 0) eventGoingCount--; }

    // Save to database
    bool saveToDatabase() const;

    // Create from database
    static Event loadFromDatabase(int eventId);
    static QVector<Event> loadAllEvents();
    static QVector<Event> loadClubEvents(int clubId);

    // Generate unique event ID
    static int generateUniqueEventId();

private:
    int eventId;
    int clubId;
    QString eventContent;
    QByteArray eventPhoto;
    QString eventCode;
    int eventGoingCount;
    QDateTime createdTimestamp;
};

#endif // EVENT_H
