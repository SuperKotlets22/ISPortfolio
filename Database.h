#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connectToDb();
    void initDb();

    // Пользователи
    bool registerUser(const QString &login, const QString &password, const QString &email, const QString &phone);
    bool loginUser(const QString &login, const QString &password, int &userId);
    bool getUserProfile(int userId, QString &email, QString &phone);

    // Портфолио (Проекты)
    bool addProject(int userId, const QString &title, const QString &role, const QString &date, const QString &desc);
    QSqlQuery getProjects(int userId);

    // Опыт работы (Резюме)
    bool addExperience(int userId, const QString &company, const QString &position, const QString &years, const QString &achievements);
    QSqlQuery getExperience(int userId);

private:
    DatabaseManager() {}
    QSqlDatabase db;
};

#endif // DATABASE_H
