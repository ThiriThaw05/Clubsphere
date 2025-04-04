#ifndef CLUB_H
#define CLUB_H

#include <QString>
#include <QVector>
#include <QByteArray>

class Club {
public:
    Club();
    Club(int id, const QString& name, int leaderId, const QByteArray& photo = QByteArray());

    // Getters
    int getId() const { return clubId; }
    QString getName() const { return clubName; }
    QByteArray getPhoto() const { return clubPhoto; }
    QVector<int> getMembers() const { return members; }
    int getLeaderId() const { return leaderId; }

    // Setters
    void setId(int id) { clubId = id; }
    void setName(const QString& name) { clubName = name; }
    void setPhoto(const QByteArray& photo) { clubPhoto = photo; }
    void setMembers(const QVector<int>& membersList) { members = membersList; }
    void setLeaderId(int id) { leaderId = id; }

    // Member management
    void addMember(int userId);
    void removeMember(int userId);
    bool isMember(int userId) const;
    int getMemberCount() const { return members.size(); }

    // Save to database
    bool saveToDatabase() const;
    QVector<int> getEventIds() const { return eventIds; }
    void setEventIds(const QVector<int>& ids) { eventIds = ids; }
    void addEventId(int eventId);
    void removeEventId(int eventId);

    // Create from database
    static Club loadFromDatabase(int clubId);
    static QVector<Club> loadAllClubs();

private:
    int clubId;
    QString clubName;
    QByteArray clubPhoto;
    QVector<int> members;
    QVector<int> eventIds;
    int leaderId;
};

#endif // CLUB_H
