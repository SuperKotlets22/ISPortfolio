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
    query.exec("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, login TEXT UNIQUE, password_hash TEXT, enc_email TEXT, enc_phone TEXT, role TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS profiles (user_id INTEGER PRIMARY KEY, surname TEXT, name TEXT, patronymic TEXT, city TEXT, citizenship TEXT, gender TEXT, birth_date DATE, job_title TEXT, salary INTEGER, currency TEXT, schedule TEXT, ready_to_move BOOLEAN)");
    query.exec("CREATE TABLE IF NOT EXISTS education (id SERIAL PRIMARY KEY, user_id INTEGER, institution TEXT, faculty TEXT, specialty TEXT, year_end INTEGER)");
    query.exec("CREATE TABLE IF NOT EXISTS experience (id SERIAL PRIMARY KEY, user_id INTEGER, org_name TEXT, position TEXT, date_start DATE, date_end DATE, duties TEXT)");
}

// --- AUTH ---
bool DatabaseManager::registerUser(const QString &l, const QString &p, const QString &e, const QString &ph, const QString &r) {
    QSqlQuery q; q.prepare("INSERT INTO users (login, password_hash, enc_email, enc_phone, role) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(l); q.addBindValue(SecurityManager::hashPassword(p));
    q.addBindValue(SecurityManager::encrypt(e)); q.addBindValue(SecurityManager::encrypt(ph)); q.addBindValue(r);
    return q.exec();
}

bool DatabaseManager::loginUser(const QString &l, const QString &p, int &uid, QString &r) {
    QSqlQuery q; q.prepare("SELECT id, role FROM users WHERE login = ? AND password_hash = ?");
    q.addBindValue(l); q.addBindValue(SecurityManager::hashPassword(p));
    if(q.exec() && q.next()) { uid = q.value(0).toInt(); r = q.value(1).toString(); return true; }
    return false;
}

bool DatabaseManager::getUserContacts(int uid, QString &e, QString &ph) {
    QSqlQuery q; q.prepare("SELECT enc_email, enc_phone FROM users WHERE id = ?");
    q.addBindValue(uid);
    if(q.exec() && q.next()) {
        e = SecurityManager::decrypt(q.value(0).toString());
        ph = SecurityManager::decrypt(q.value(1).toString());
        return true;
    }
    return false;
}

QList<QPair<int, QString>> DatabaseManager::getAllSpecialists() {
    QList<QPair<int, QString>> list;
    QSqlQuery q;
    q.exec("SELECT u.id, p.surname, p.name, p.job_title FROM users u LEFT JOIN profiles p ON u.id = p.user_id WHERE u.role = 'specialist'");
    while(q.next()) {
        QString label = q.value(1).toString() + " " + q.value(2).toString();
        QString job = q.value(3).toString();
        if(label.trimmed().isEmpty()) label = "Без имени (ID: " + q.value(0).toString() + ")";
        if(!job.isEmpty()) label += " - " + job;
        list.append({q.value(0).toInt(), label});
    }
    return list;
}

// --- PROFILE ---
bool DatabaseManager::saveProfile(int uid, const UserProfile &p) {
    QSqlQuery q;
    q.prepare("INSERT INTO profiles (user_id, surname, name, patronymic, city, citizenship, gender, birth_date, job_title, salary, currency, schedule, ready_to_move) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ON CONFLICT (user_id) DO UPDATE SET surname=EXCLUDED.surname, name=EXCLUDED.name, patronymic=EXCLUDED.patronymic, city=EXCLUDED.city, citizenship=EXCLUDED.citizenship, gender=EXCLUDED.gender, birth_date=EXCLUDED.birth_date, job_title=EXCLUDED.job_title, salary=EXCLUDED.salary, currency=EXCLUDED.currency, schedule=EXCLUDED.schedule, ready_to_move=EXCLUDED.ready_to_move");
    q.addBindValue(uid); q.addBindValue(p.surname); q.addBindValue(p.name); q.addBindValue(p.patronymic);
    q.addBindValue(p.city); q.addBindValue(p.citizenship); q.addBindValue(p.gender); q.addBindValue(p.birthDate);
    q.addBindValue(p.jobTitle); q.addBindValue(p.salary); q.addBindValue(p.currency);
    q.addBindValue(p.schedule); q.addBindValue(p.readyToMove);
    return q.exec();
}

bool DatabaseManager::getProfile(int uid, UserProfile &p) {
    QSqlQuery q; q.prepare("SELECT surname, name, patronymic, city, citizenship, gender, birth_date, job_title, salary, currency, schedule, ready_to_move FROM profiles WHERE user_id = ?");
    q.addBindValue(uid);
    if(q.exec() && q.next()) {
        p.surname = q.value(0).toString(); p.name = q.value(1).toString(); p.patronymic = q.value(2).toString();
        p.city = q.value(3).toString(); p.citizenship = q.value(4).toString(); p.gender = q.value(5).toString();
        p.birthDate = q.value(6).toDate(); p.jobTitle = q.value(7).toString(); p.salary = q.value(8).toInt(); p.currency = q.value(9).toString();
        p.schedule = q.value(10).toString(); p.readyToMove = q.value(11).toBool();
        return true;
    }
    return false;
}

// --- EDUCATION (ORM) ---
bool DatabaseManager::addEducation(const EducationData &d) {
    QSqlQuery q; q.prepare("INSERT INTO education (user_id, institution, faculty, specialty, year_end) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(d.userId); q.addBindValue(d.institution); q.addBindValue(d.faculty); q.addBindValue(d.specialty); q.addBindValue(d.year);
    return q.exec();
}

QList<EducationData> DatabaseManager::getEducationList(int uid) {
    QList<EducationData> list;
    QSqlQuery q; q.prepare("SELECT id, user_id, institution, faculty, specialty, year_end FROM education WHERE user_id = ?");
    q.addBindValue(uid); q.exec();
    while(q.next()) list.append({q.value(0).toInt(), q.value(1).toInt(), q.value(2).toString(), q.value(3).toString(), q.value(4).toString(), q.value(5).toInt()});
    return list;
}

bool DatabaseManager::deleteEducation(int id) {
    QSqlQuery q; q.prepare("DELETE FROM education WHERE id = ?"); q.addBindValue(id); return q.exec();
}

// --- EXPERIENCE (ORM) ---
bool DatabaseManager::addExperience(const ExperienceData &d) {
    QSqlQuery q; q.prepare("INSERT INTO experience (user_id, org_name, position, date_start, date_end, duties) VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(d.userId); q.addBindValue(d.orgName); q.addBindValue(d.position); q.addBindValue(d.startDate); q.addBindValue(d.endDate); q.addBindValue(d.duties);
    return q.exec();
}

QList<ExperienceData> DatabaseManager::getExperienceList(int uid) {
    QList<ExperienceData> list;
    QSqlQuery q; q.prepare("SELECT id, user_id, org_name, position, date_start, date_end, duties FROM experience WHERE user_id = ?");
    q.addBindValue(uid); q.exec();
    while(q.next()) list.append({q.value(0).toInt(), q.value(1).toInt(), q.value(2).toString(), q.value(3).toString(), q.value(4).toDate(), q.value(5).toDate(), q.value(6).toString()});
    return list;
}

bool DatabaseManager::deleteExperience(int id) {
    QSqlQuery q; q.prepare("DELETE FROM experience WHERE id = ?"); q.addBindValue(id); return q.exec();
}