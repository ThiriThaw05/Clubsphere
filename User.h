#ifndef USER_H
#define USER_H

#include <QString>
#include <QVector>
#include <QByteArray>

class User {
public:
    User();
    User(int id, const QString& name, int points, const QByteArray& photo = QByteArray());

    // Getters
    int getId() const { return userId; }
    QString getName() const { return userName; }
    int getPoints() const { return points; }
    QByteArray getPhoto() const { return profilePhoto; }
    QVector<int> getJoinedClubs() const { return joinedClubs; }
    QVector<int> getPendingClubs() const { return pendingClubs; }
    QVector<int> getGoingEvents() const { return goingEvents; }

    // Setters
    void setId(int id) { userId = id; }
    void setName(const QString& name) { userName = name; }
    void setPoints(int pts) { points = pts; }
    void setPhoto(const QByteArray& photo) { profilePhoto = photo; }
    void setSuspended(bool status) { suspended = status; }
    void setJoinedClubs(const QVector<int>& clubs) { joinedClubs = clubs; }
    void setPendingClubs(const QVector<int>& clubs) { pendingClubs = clubs; }
    void setGoingEvents(const QVector<int>& events) { goingEvents = events; }

    // Club management
    void joinClub(int clubId);
    void leaveClub(int clubId);
    void requestClub(int clubId);
    void removeClubRequest(int clubId);
    bool isClubMember(int clubId) const;
    bool hasRequestedClub(int clubId) const;

    // Event management
    void goToEvent(int eventId);
    void cancelEvent(int eventId);
    bool isGoingToEvent(int eventId) const;

    // Save to database
    bool saveToDatabase() const;

    // Create from database
    static User loadFromDatabase(int userId);
    static QVector<User> loadAllUsers();

private:
    int userId;
    QString userName;
    int points;
    QByteArray profilePhoto;
    bool suspended;
    QVector<int> joinedClubs;
    QVector<int> pendingClubs;
    QVector<int> goingEvents;
};

#endif // USER_H
