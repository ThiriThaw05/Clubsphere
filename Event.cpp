#include "Event.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRandomGenerator>

Event::Event() : eventId(0), clubId(0), eventGoingCount(0) {
    createdTimestamp = QDateTime::currentDateTime();
}

Event::Event(int id, int cid, const QString& content, const QByteArray& photo,
             const QString& code, int goingCount, const QDateTime& timestamp)
    : eventId(id), clubId(cid), eventContent(content), eventPhoto(photo),
    eventCode(code), eventGoingCount(goingCount), createdTimestamp(timestamp) {
}

bool Event::saveToDatabase() const {
    QSqlQuery query;
    // Check if event already exists
    bool isUpdate = false;
    query.prepare("SELECT COUNT(*) FROM events_list WHERE event_id = :id");
    query.bindValue(":id", eventId);
    if (query.exec() && query.next()) {
        isUpdate = query.value(0).toInt() > 0;
    } else {
        qDebug() << "Error checking if event exists:" << query.lastError().text();
        return false;
    }

    // Start a transaction
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qDebug() << "Error starting transaction:" << db.lastError().text();
        return false;
    }

    // Update or insert event record
    if (isUpdate) {
        // Update existing event
        query.prepare("UPDATE events_list SET club_id = :clubId, event_content = :content, "
                      "event_photo = :photo, event_code = :code, event_going_count = :goingCount, "
                      "created_timestamp = :timestamp "
                      "WHERE event_id = :id");
    } else {
        // Insert new event
        query.prepare("INSERT INTO events_list (event_id, club_id, event_content, event_photo, event_code, "
                      "event_going_count, created_timestamp) "
                      "VALUES (:id, :clubId, :content, :photo, :code, :goingCount, :timestamp)");
    }

    query.bindValue(":id", eventId);
    query.bindValue(":clubId", clubId);
    query.bindValue(":content", eventContent);
    query.bindValue(":photo", eventPhoto);
    query.bindValue(":code", eventCode);
    query.bindValue(":goingCount", eventGoingCount);
    query.bindValue(":timestamp", createdTimestamp.toSecsSinceEpoch());

    if (!query.exec()) {
        qDebug() << "Error saving event to database:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Commit the transaction
    if (!db.commit()) {
        qDebug() << "Error committing transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

Event Event::loadFromDatabase(int eventId) {
    Event event;
    QSqlQuery query;
    query.prepare("SELECT event_id, club_id, event_content, event_photo, event_code, "
                  "event_going_count, created_timestamp "
                  "FROM events_list WHERE event_id = :id");
    query.bindValue(":id", eventId);

    if (query.exec() && query.next()) {
        event.setId(query.value(0).toInt());
        event.setClubId(query.value(1).toInt());
        event.setContent(query.value(2).toString());
        event.setPhoto(query.value(3).toByteArray());
        event.setCode(query.value(4).toString());
        event.setGoingCount(query.value(5).toInt());

        // Convert timestamp from seconds since epoch to QDateTime
        QDateTime timestamp;
        timestamp.setSecsSinceEpoch(query.value(6).toLongLong());
        event.setCreatedTimestamp(timestamp);
    } else {
        qDebug() << "Error loading event from database:" << query.lastError().text();
    }

    return event;
}

QVector<Event> Event::loadAllEvents() {
    QVector<Event> events;
    QSqlQuery query("SELECT event_id, club_id, event_content, event_photo, event_code, "
                    "event_going_count, created_timestamp FROM events_list");

    while (query.next()) {
        Event event;
        event.setId(query.value(0).toInt());
        event.setClubId(query.value(1).toInt());
        event.setContent(query.value(2).toString());
        event.setPhoto(query.value(3).toByteArray());
        event.setCode(query.value(4).toString());
        event.setGoingCount(query.value(5).toInt());

        // Convert timestamp from seconds since epoch to QDateTime
        QDateTime timestamp;
        timestamp.setSecsSinceEpoch(query.value(6).toLongLong());
        event.setCreatedTimestamp(timestamp);

        events.append(event);
    }

    return events;
}

QVector<Event> Event::loadClubEvents(int clubId) {
    QVector<Event> events;
    QSqlQuery query;
    query.prepare("SELECT event_id, club_id, event_content, event_photo, event_code, "
                  "event_going_count, created_timestamp "
                  "FROM events_list WHERE club_id = :clubId");
    query.bindValue(":clubId", clubId);

    if (query.exec()) {
        while (query.next()) {
            Event event;
            event.setId(query.value(0).toInt());
            event.setClubId(query.value(1).toInt());
            event.setContent(query.value(2).toString());
            event.setPhoto(query.value(3).toByteArray());
            event.setCode(query.value(4).toString());
            event.setGoingCount(query.value(5).toInt());

            // Convert timestamp from seconds since epoch to QDateTime
            QDateTime timestamp;
            timestamp.setSecsSinceEpoch(query.value(6).toLongLong());
            event.setCreatedTimestamp(timestamp);

            events.append(event);
        }
    } else {
        qDebug() << "Error loading club events from database:" << query.lastError().text();
    }

    return events;
}

int Event::generateUniqueEventId() {
    QSqlQuery query;
    int newId;
    bool idExists = true;

    // Keep trying until we find an ID that doesn't exist
    while (idExists) {
        // Generate a random ID between 10000 and 99999
        newId = QRandomGenerator::global()->bounded(10000, 100000);

        // Check if this ID already exists
        query.prepare("SELECT COUNT(*) FROM events_list WHERE event_id = :id");
        query.bindValue(":id", newId);

        if (query.exec() && query.next()) {
            idExists = query.value(0).toInt() > 0;
        } else {
            qDebug() << "Error checking event ID:" << query.lastError().text();
            // If there's an error, break the loop and return a "safe" default
            return QRandomGenerator::global()->bounded(10000, 100000);
        }
    }

    return newId;
}
