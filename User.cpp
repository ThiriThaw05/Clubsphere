#include "User.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

User::User() : userId(0), userName(""), points(0) {
}

User::User(int id, const QString& name, int pts, const QByteArray& photo)
    : userId(id), userName(name), points(pts), profilePhoto(photo) {
    // If no photo is provided, use default
    if (profilePhoto.isEmpty()) {
        profilePhoto = Database::loadDefaultProfilePhoto();
    }
}

void User::joinClub(int clubId) {
    if (!isClubMember(clubId)) {
        joinedClubs.append(clubId);
        // Also remove from pending if it was there
        removeClubRequest(clubId);
    }
}

void User::leaveClub(int clubId) {
    joinedClubs.removeAll(clubId);
}

void User::requestClub(int clubId) {
    if (!hasRequestedClub(clubId) && !isClubMember(clubId)) {
        pendingClubs.append(clubId);
    }
}

void User::removeClubRequest(int clubId) {
    pendingClubs.removeAll(clubId);
}

bool User::isClubMember(int clubId) const {
    return joinedClubs.contains(clubId);
}

bool User::hasRequestedClub(int clubId) const {
    return pendingClubs.contains(clubId);
}

void User::goToEvent(int eventId) {
    if (!isGoingToEvent(eventId)) {
        goingEvents.append(eventId);
    }
}

void User::cancelEvent(int eventId) {
    goingEvents.removeAll(eventId);
}

bool User::isGoingToEvent(int eventId) const {
    return goingEvents.contains(eventId);
}

bool User::saveToDatabase() const {
    QSqlQuery query;
    // Check if user already exists
    bool isUpdate = false;
    query.prepare("SELECT COUNT(*) FROM users_list WHERE user_id = :id");
    query.bindValue(":id", userId);
    if (query.exec() && query.next()) {
        isUpdate = query.value(0).toInt() > 0;
    } else {
        qDebug() << "Error checking if user exists:" << query.lastError().text();
        return false;
    }

    // Start a transaction
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qDebug() << "Error starting transaction:" << db.lastError().text();
        return false;
    }

    // Serialize vectors
    QString serializedJoinedClubs = Database::serializeUserIds(joinedClubs);
    QString serializedPendingClubs = Database::serializeUserIds(pendingClubs);
    QString serializedGoingEvents = Database::serializeUserIds(goingEvents);

    // Update or insert user record
    if (isUpdate) {
        // Update existing user
        query.prepare("UPDATE users_list SET name = :name, points = :points, "
                      "profile_photo = :photo, "
                      "joined_clubs = :joinedClubs, pending_clubs = :pendingClubs, "
                      "going_events = :goingEvents "
                      "WHERE user_id = :id");
    } else {
        // Insert new user
        query.prepare("INSERT INTO users_list (user_id, name, points, profile_photo, "
                      "joined_clubs, pending_clubs, going_events) "
                      "VALUES (:id, :name, :points, :photo, "
                      ":joinedClubs, :pendingClubs, :goingEvents)");
    }

    query.bindValue(":id", userId);
    query.bindValue(":name", userName);
    query.bindValue(":points", points);
    query.bindValue(":photo", profilePhoto);
    query.bindValue(":joinedClubs", serializedJoinedClubs);
    query.bindValue(":pendingClubs", serializedPendingClubs);
    query.bindValue(":goingEvents", serializedGoingEvents);

    if (!query.exec()) {
        qDebug() << "Error saving user to database:" << query.lastError().text();
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

User User::loadFromDatabase(int userId) {
    User user;
    QSqlQuery query;
    query.prepare("SELECT user_id, name, points, profile_photo, "
                  "joined_clubs, pending_clubs, going_events "
                  "FROM users_list WHERE user_id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        user.setId(query.value(0).toInt());
        user.setName(query.value(1).toString());
        user.setPoints(query.value(2).toInt());
        user.setPhoto(query.value(3).toByteArray());
        user.setJoinedClubs(Database::deserializeUserIds(query.value(4).toString()));
        user.setPendingClubs(Database::deserializeUserIds(query.value(5).toString()));
        user.setGoingEvents(Database::deserializeUserIds(query.value(6).toString()));
    } else {
        qDebug() << "Error loading user from database:" << query.lastError().text();
    }

    return user;
}

QVector<User> User::loadAllUsers() {
    QVector<User> users;
    QSqlQuery query("SELECT user_id, name, points, profile_photo, "
                    "joined_clubs, pending_clubs, going_events FROM users_list");

    while (query.next()) {
        User user;
        user.setId(query.value(0).toInt());
        user.setName(query.value(1).toString());
        user.setPoints(query.value(2).toInt());
        user.setPhoto(query.value(3).toByteArray());
        user.setJoinedClubs(Database::deserializeUserIds(query.value(4).toString()));
        user.setPendingClubs(Database::deserializeUserIds(query.value(5).toString()));
        user.setGoingEvents(Database::deserializeUserIds(query.value(6).toString()));
        users.append(user);
    }

    return users;
}
