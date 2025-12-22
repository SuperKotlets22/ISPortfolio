#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QDate>

// --- ORM СТРУКТУРЫ ---
struct UserProfile {
    QString surname, name, patronymic;
    QString city, citizenship, gender;
    QDate birthDate;
    QString jobTitle, currency, schedule;
    int salary;
    bool readyToMove;
};

struct EducationData {
    int id;
    int userId;
    QString institution;
    QString faculty;
    QString specialty;
    int year;
};

struct ExperienceData {
    int id;
    int userId;
    QString orgName;
    QString position;
    QDate startDate;
    QDate endDate;
    QString duties;
};

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connectToDb();
    void initDb();

    // Auth
    bool registerUser(const QString &login, const QString &password, const QString &email, const QString &phone, const QString &role);
    bool loginUser(const QString &login, const QString &password, int &userId, QString &role);
    bool getUserContacts(int userId, QString &email, QString &phone);
    QList<QPair<int, QString>> getAllSpecialists();

    // Profile
    bool saveProfile(int userId, const UserProfile &p);
    bool getProfile(int userId, UserProfile &p);

    // Education (ORM)
    bool addEducation(const EducationData &data);
    QList<EducationData> getEducationList(int userId);
    bool deleteEducation(int id);

    // Experience (ORM)
    bool addExperience(const ExperienceData &data);
    QList<ExperienceData> getExperienceList(int userId);
    bool deleteExperience(int id);

private:
    DatabaseManager() {}
    QSqlDatabase db;
};

#endif // DATABASE_H