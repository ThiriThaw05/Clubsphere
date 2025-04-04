#include "Club.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Club::Club() : clubId(0), clubName(""), leaderId(0) {
}

Club::Club(int id, const QString& name, int leaderID, const QByteArray& photo)
    : clubId(id), clubName(name), clubPhoto(photo), leaderId(leaderID) {
    // If no photo is provided, use default
    if (clubPhoto.isEmpty()) {
        clubPhoto = Database::loadDefaultClubPhoto();
    }
}

void Club::addMember(int userId) {
    if (!isMember(userId)) {
        members.append(userId);
    }
}

void Club::removeMember(int userId) {
    members.removeAll(userId);
}

bool Club::isMember(int userId) const {
    return members.contains(userId);
}

bool Club::saveToDatabase() const {
    QSqlQuery query;
    // Check if club already exists
    bool isUpdate = false;
    query.prepare("SELECT COUNT(*) FROM clubs_list WHERE club_id = :id");
    query.bindValue(":id", clubId);
    if (query.exec() && query.next()) {
        isUpdate = query.value(0).toInt() > 0;
    } else {
        qDebug() << "Error checking if club exists:" << query.lastError().text();
        return false;
    }

    // Start a transaction
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qDebug() << "Error starting transaction:" << db.lastError().text();
        return false;
    }

    // Convert members to serialized string
    QString serializedMembers = Database::serializeUserIds(members);
    QString serializedEventIds = Database::serializeUserIds(eventIds);

    // Update or insert club record
    if (isUpdate) {
        // Update existing club
        query.prepare("UPDATE clubs_list SET club_name = :name, club_photo = :photo, "
                      "club_members = :members, leader_id = :leaderId, event_ids = :eventIds "
                      "WHERE club_id = :id");
    } else {
        // Insert new club
        query.prepare("INSERT INTO clubs_list (club_id, club_name, club_photo, club_members, leader_id, event_ids) "
                      "VALUES (:id, :name, :photo, :members, :leaderId, :eventIds)");
    }

    query.bindValue(":id", clubId);
    query.bindValue(":name", clubName);
    query.bindValue(":photo", clubPhoto);
    query.bindValue(":members", serializedMembers);
    query.bindValue(":leaderId", leaderId);
    query.bindValue(":eventIds", serializedEventIds);

    if (!query.exec()) {
        qDebug() << "Error saving club to database:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // Add entry to clubleaders_list table if leader_id is valid
    if (leaderId > 0) {
        // Insert entry into clubleaders_list
        QSqlQuery leaderQuery;
        leaderQuery.prepare("INSERT OR REPLACE INTO clubleaders_list (leader_id, assigned_club_id) VALUES (:leaderId, :clubId)");
        leaderQuery.bindValue(":leaderId", leaderId);
        leaderQuery.bindValue(":clubId", clubId);

        if (!leaderQuery.exec()) {
            qDebug() << "Error adding leader relationship:" << leaderQuery.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Commit the transaction
    if (!db.commit()) {
        qDebug() << "Error committing transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

Club Club::loadFromDatabase(int clubId) {
    Club club;
    QSqlQuery query;
    query.prepare("SELECT club_id, club_name, club_photo, club_members, leader_id, event_ids "
                  "FROM clubs_list WHERE club_id = :id");
    query.bindValue(":id", clubId);

    if (query.exec() && query.next()) {
        club.setId(query.value(0).toInt());
        club.setName(query.value(1).toString());
        club.setPhoto(query.value(2).toByteArray());
        club.setMembers(Database::deserializeUserIds(query.value(3).toString()));
        club.setLeaderId(query.value(4).toInt());
        club.setEventIds(Database::deserializeUserIds(query.value(5).toString()));
    } else {
        qDebug() << "Error loading club from database:" << query.lastError().text();
    }

    return club;
}

QVector<Club> Club::loadAllClubs() {
    QVector<Club> clubs;
    QSqlQuery query("SELECT club_id, club_name, club_photo, club_members, leader_id, event_ids FROM clubs_list");

    while (query.next()) {
        Club club;
        club.setId(query.value(0).toInt());
        club.setName(query.value(1).toString());
        club.setPhoto(query.value(2).toByteArray());
        club.setMembers(Database::deserializeUserIds(query.value(3).toString()));
        club.setLeaderId(query.value(4).toInt());
        club.setEventIds(Database::deserializeUserIds(query.value(5).toString()));
        clubs.append(club);
    }

    return clubs;
}


void Club::addEventId(int eventId) {
    if (!eventIds.contains(eventId)) {
        eventIds.append(eventId);
    }
}

void Club::removeEventId(int eventId) {
    eventIds.removeAll(eventId);
}
