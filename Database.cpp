#include "Database.h"
#include "Security.h"

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connectToDb() {
    if (db.isOpen()) return true;
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("PortfolioDB");
    db.setUserName("postgres");
    db.setPassword("securepassword");
    return db.open();
}

void DatabaseManager::initDb() {
    QSqlQuery query;
    // Пользователи
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id SERIAL PRIMARY KEY, "
               "login TEXT UNIQUE, "
               "password_hash TEXT, "
               "enc_email TEXT, "
               "enc_phone TEXT)");
    
    // Проекты
    query.exec("CREATE TABLE IF NOT EXISTS projects ("
               "id SERIAL PRIMARY KEY, "
               "user_id INTEGER, "
               "title TEXT, "
               "role TEXT, "
               "p_date TEXT, "
               "description TEXT)");

    // Опыт работы (Новая таблица)
    query.exec("CREATE TABLE IF NOT EXISTS experience ("
               "id SERIAL PRIMARY KEY, "
               "user_id INTEGER, "
               "company TEXT, "
               "position TEXT, "
               "years TEXT, "
               "achievements TEXT)");
}

bool DatabaseManager::registerUser(const QString &login, const QString &password, const QString &email, const QString &phone) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (login, password_hash, enc_email, enc_phone) VALUES (?, ?, ?, ?)");
    query.addBindValue(login);
    query.addBindValue(SecurityManager::hashPassword(password));
    query.addBindValue(SecurityManager::encrypt(email));
    query.addBindValue(SecurityManager::encrypt(phone));
    return query.exec();
}

bool DatabaseManager::loginUser(const QString &login, const QString &password, int &userId) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE login = ? AND password_hash = ?");
    query.addBindValue(login);
    query.addBindValue(SecurityManager::hashPassword(password));
    if (query.exec() && query.next()) {
        userId = query.value(0).toInt();
        return true;
    }
    return false;
}

bool DatabaseManager::getUserProfile(int userId, QString &email, QString &phone) {
    QSqlQuery query;
    query.prepare("SELECT enc_email, enc_phone FROM users WHERE id = ?");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        email = SecurityManager::decrypt(query.value(0).toString());
        phone = SecurityManager::decrypt(query.value(1).toString());
        return true;
    }
    return false;
}

// === Проекты ===
bool DatabaseManager::addProject(int userId, const QString &title, const QString &role, const QString &date, const QString &desc) {
    QSqlQuery query;
    query.prepare("INSERT INTO projects (user_id, title, role, p_date, description) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(userId); query.addBindValue(title); query.addBindValue(role); 
    query.addBindValue(date); query.addBindValue(desc);
    return query.exec();
}

QSqlQuery DatabaseManager::getProjects(int userId) {
    QSqlQuery query;
    query.prepare("SELECT title, role, p_date, description FROM projects WHERE user_id = ?");
    query.addBindValue(userId);
    query.exec();
    return query;
}

// === Опыт работы ===
bool DatabaseManager::addExperience(int userId, const QString &company, const QString &position, const QString &years, const QString &achievements) {
    QSqlQuery query;
    query.prepare("INSERT INTO experience (user_id, company, position, years, achievements) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(userId); query.addBindValue(company); query.addBindValue(position); 
    query.addBindValue(years); query.addBindValue(achievements);
    return query.exec();
}

QSqlQuery DatabaseManager::getExperience(int userId) {
    QSqlQuery query;
    query.prepare("SELECT company, position, years, achievements FROM experience WHERE user_id = ?");
    query.addBindValue(userId);
    query.exec();
    return query;
}
