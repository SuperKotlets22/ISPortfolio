#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QDate>

// Структура для передачи данных профиля одним объектом
struct UserProfile {
    QString surname, name, patronymic;
    QString city, citizenship;
    QString gender; // Муж/Жен
    QDate birthDate;
    QString jobTitle;
    int salary;
    QString currency; // Рубли/USD
    QString schedule; // Полный день/Удаленка
    bool readyToMove;
};

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connectToDb();
    void initDb();

    // --- Auth ---
    bool registerUser(const QString &login, const QString &password, const QString &email, const QString &phone, const QString &role);
    bool loginUser(const QString &login, const QString &password, int &userId, QString &role);
    bool getUserContacts(int userId, QString &email, QString &phone);
    QList<QPair<int, QString>> getAllSpecialists(); // Возвращает ID и "Фамилия Имя (Должность)"

    // --- Profile (Личные данные) ---
    bool saveProfile(int userId, const UserProfile &p);
    bool getProfile(int userId, UserProfile &p);

    // --- Education ---
    bool addEducation(int userId, const QString &inst, const QString &faculty, const QString &spec, int year);
    QSqlQuery getEducation(int userId);
    bool deleteEducation(int id);

    // --- Experience ---
    bool addExperience(int userId, const QString &org, const QString &pos, QDate start, QDate end, const QString &duties);
    QSqlQuery getExperience(int userId);
    bool deleteExperience(int id);

private:
    DatabaseManager() {}
    QSqlDatabase db;
};

#endif // DATABASE_H
