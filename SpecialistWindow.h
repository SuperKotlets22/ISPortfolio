#ifndef SPECIALISTWINDOW_H
#define SPECIALISTWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include "Database.h"

class SpecialistWindow : public QMainWindow {
    Q_OBJECT
    int userId;

    // Поля формы (Профиль)
    QLineEdit *editSurname, *editName, *editPatronymic;
    QLineEdit *editCity, *editCitizenship;
    QComboBox *comboGender;
    QDateEdit *dateBirth;
    
    QLineEdit *editJobTitle;
    QSpinBox *spinSalary;
    QComboBox *comboSchedule;
    QCheckBox *checkMove;

    // Таблицы
    QTableWidget *tableEdu;
    QTableWidget *tableExp;

public:
    explicit SpecialistWindow(int uid, QWidget *parent = nullptr) : QMainWindow(parent), userId(uid) {
        setWindowTitle("Конструктор Резюме");
        QWidget *central = new QWidget;
        setCentralWidget(central);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);

        // --- Верхняя панель ---
        QHBoxLayout *topPanel = new QHBoxLayout();
        QLabel *title = new QLabel("Мое Резюме");
        title->setStyleSheet("font-size: 24px; font-weight: bold; color: #61dafb;");
        QPushButton *btnExit = new QPushButton("Выйти");
        btnExit->setStyleSheet("background-color: #d32f2f;");
        topPanel->addWidget(title); topPanel->addStretch(); topPanel->addWidget(btnExit);
        mainLayout->addLayout(topPanel);

        // --- ОБЛАСТЬ ПРОКРУТКИ (Т.к. форма длинная) ---
        QScrollArea *scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        
        QWidget *scrollContent = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
        scrollLayout->setSpacing(20);

        // 1. Блок "Основная информация"
        QGroupBox *groupBasic = new QGroupBox("Основная и Личная информация");
        groupBasic->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #555; border-radius: 5px; margin-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
        QFormLayout *formBasic = new QFormLayout(groupBasic);
        formBasic->setLabelAlignment(Qt::AlignRight);
        
        editSurname = new QLineEdit; editName = new QLineEdit; editPatronymic = new QLineEdit;
        formBasic->addRow("Фамилия:", editSurname);
        formBasic->addRow("Имя:", editName);
        formBasic->addRow("Отчество:", editPatronymic);
        
        QHBoxLayout *row2 = new QHBoxLayout();
        editCity = new QLineEdit; editCity->setPlaceholderText("Например: Москва");
        editCitizenship = new QLineEdit; editCitizenship->setPlaceholderText("Например: РФ");
        row2->addWidget(new QLabel("Город:")); row2->addWidget(editCity);
        row2->addWidget(new QLabel("Гражданство:")); row2->addWidget(editCitizenship);
        formBasic->addRow(row2);

        QHBoxLayout *row3 = new QHBoxLayout();
        comboGender = new QComboBox(); comboGender->addItems({"Мужской", "Женский"});
        dateBirth = new QDateEdit(); dateBirth->setCalendarPopup(true); dateBirth->setDisplayFormat("dd.MM.yyyy"); dateBirth->setDate(QDate(2000, 1, 1));
        row3->addWidget(new QLabel("Пол:")); row3->addWidget(comboGender);
        row3->addWidget(new QLabel("Дата рождения:")); row3->addWidget(dateBirth);
        formBasic->addRow(row3);
        scrollLayout->addWidget(groupBasic);

        // 2. Блок "Параметры работы"
        QGroupBox *groupJob = new QGroupBox("Желаемая должность и условия");
        groupJob->setStyleSheet(groupBasic->styleSheet());
        QFormLayout *formJob = new QFormLayout(groupJob);
        
        editJobTitle = new QLineEdit; editJobTitle->setPlaceholderText("Например: Специалист по ИБ");
        formJob->addRow("Должность:", editJobTitle);
        
        QHBoxLayout *rowSal = new QHBoxLayout();
        spinSalary = new QSpinBox(); spinSalary->setRange(0, 1000000); spinSalary->setSingleStep(5000); spinSalary->setSuffix(" руб.");
        comboSchedule = new QComboBox(); comboSchedule->addItems({"Полный день", "Удаленная работа", "Гибкий график", "Сменный график"});
        rowSal->addWidget(new QLabel("Зарплата:")); rowSal->addWidget(spinSalary);
        rowSal->addWidget(new QLabel("График:")); rowSal->addWidget(comboSchedule);
        formJob->addRow(rowSal);
        
        checkMove = new QCheckBox("Готов к переезду");
        formJob->addRow("", checkMove);
        scrollLayout->addWidget(groupJob);

        // 3. Блок "Образование" (Таблица)
        QGroupBox *groupEdu = new QGroupBox("Образование");
        groupEdu->setStyleSheet(groupBasic->styleSheet());
        QVBoxLayout *eduLayout = new QVBoxLayout(groupEdu);
        
        tableEdu = new QTableWidget(0, 4);
        tableEdu->setHorizontalHeaderLabels({"Учебное заведение", "Факультет", "Специальность", "Год"});
        tableEdu->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableEdu->setFixedHeight(150);
        
        QHBoxLayout *eduBtns = new QHBoxLayout();
        QPushButton *btnAddEdu = new QPushButton("Добавить ВУЗ");
        QPushButton *btnDelEdu = new QPushButton("Удалить строку");
        eduBtns->addWidget(btnAddEdu); eduBtns->addWidget(btnDelEdu); eduBtns->addStretch();
        
        eduLayout->addWidget(tableEdu); eduLayout->addLayout(eduBtns);
        scrollLayout->addWidget(groupEdu);

        // 4. Блок "Опыт работы" (Таблица)
        QGroupBox *groupExp = new QGroupBox("Опыт работы");
        groupExp->setStyleSheet(groupBasic->styleSheet());
        QVBoxLayout *expLayout = new QVBoxLayout(groupExp);
        
        tableExp = new QTableWidget(0, 4);
        tableExp->setHorizontalHeaderLabels({"Организация", "Должность", "Период", "Обязанности"});
        tableExp->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        tableExp->setFixedHeight(200);

        QHBoxLayout *expBtns = new QHBoxLayout();
        QPushButton *btnAddExp = new QPushButton("Добавить место работы");
        QPushButton *btnDelExp = new QPushButton("Удалить строку");
        expBtns->addWidget(btnAddExp); expBtns->addWidget(btnDelExp); expBtns->addStretch();

        expLayout->addWidget(tableExp); expLayout->addLayout(expBtns);
        scrollLayout->addWidget(groupExp);

        // КНОПКА СОХРАНИТЬ (Внизу скролла)
        QPushButton *btnSave = new QPushButton("СОХРАНИТЬ РЕЗЮМЕ");
        btnSave->setStyleSheet("background-color: #388e3c; font-size: 16px; padding: 15px; font-weight: bold;");
        scrollLayout->addWidget(btnSave);

        scroll->setWidget(scrollContent);
        mainLayout->addWidget(scroll);

        // --- ЛОГИКА ---
        loadAllData();

        connect(btnExit, &QPushButton::clicked, [this](){ emit logoutRequested(); this->close(); });
        connect(btnSave, &QPushButton::clicked, this, &SpecialistWindow::saveAllData);
        
        connect(btnAddEdu, &QPushButton::clicked, this, &SpecialistWindow::addEduDialog);
        connect(btnDelEdu, &QPushButton::clicked, [this](){ deleteRow(tableEdu, "education"); });
        
        connect(btnAddExp, &QPushButton::clicked, this, &SpecialistWindow::addExpDialog);
        connect(btnDelExp, &QPushButton::clicked, [this](){ deleteRow(tableExp, "experience"); });
    }

signals:
    void logoutRequested();

private:
    void loadAllData() {
        // Профиль
        UserProfile p;
        if(DatabaseManager::instance().getProfile(userId, p)) {
            editSurname->setText(p.surname); editName->setText(p.name); editPatronymic->setText(p.patronymic);
            editCity->setText(p.city); editCitizenship->setText(p.citizenship);
            comboGender->setCurrentText(p.gender); dateBirth->setDate(p.birthDate);
            editJobTitle->setText(p.jobTitle); spinSalary->setValue(p.salary);
            comboSchedule->setCurrentText(p.schedule); checkMove->setChecked(p.readyToMove);
        }

        // Образование
        tableEdu->setRowCount(0);
        QSqlQuery qEdu = DatabaseManager::instance().getEducation(userId);
        while(qEdu.next()) {
            int r = tableEdu->rowCount(); tableEdu->insertRow(r);
            tableEdu->setItem(r, 0, new QTableWidgetItem(qEdu.value(0).toString()));
            tableEdu->setItem(r, 1, new QTableWidgetItem(qEdu.value(1).toString()));
            tableEdu->setItem(r, 2, new QTableWidgetItem(qEdu.value(2).toString()));
            tableEdu->setItem(r, 3, new QTableWidgetItem(qEdu.value(3).toString()));
            tableEdu->item(r, 0)->setData(Qt::UserRole, qEdu.value(4)); // ID записи
        }

        // Опыт
        tableExp->setRowCount(0);
        QSqlQuery qExp = DatabaseManager::instance().getExperience(userId);
        while(qExp.next()) {
            int r = tableExp->rowCount(); tableExp->insertRow(r);
            tableExp->setItem(r, 0, new QTableWidgetItem(qExp.value(0).toString()));
            tableExp->setItem(r, 1, new QTableWidgetItem(qExp.value(1).toString()));
            QString period = qExp.value(2).toDate().toString("MM.yyyy") + " - " + qExp.value(3).toDate().toString("MM.yyyy");
            tableExp->setItem(r, 2, new QTableWidgetItem(period));
            tableExp->setItem(r, 3, new QTableWidgetItem(qExp.value(4).toString()));
            tableExp->item(r, 0)->setData(Qt::UserRole, qExp.value(5)); // ID записи
        }
    }

    void saveAllData() {
        UserProfile p;
        p.surname = editSurname->text(); p.name = editName->text(); p.patronymic = editPatronymic->text();
        p.city = editCity->text(); p.citizenship = editCitizenship->text();
        p.gender = comboGender->currentText(); p.birthDate = dateBirth->date();
        p.jobTitle = editJobTitle->text(); p.salary = spinSalary->value();
        p.schedule = comboSchedule->currentText(); p.readyToMove = checkMove->isChecked();
        
        if(DatabaseManager::instance().saveProfile(userId, p)) {
            QMessageBox::information(this, "Успех", "Резюме сохранено!");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить данные");
        }
    }

    void addEduDialog() {
        QDialog d(this); d.setWindowTitle("Добавить образование");
        QFormLayout *l = new QFormLayout(&d);
        QLineEdit *i = new QLineEdit, *f = new QLineEdit, *s = new QLineEdit; 
        QSpinBox *y = new QSpinBox; y->setRange(1950, 2030); y->setValue(2025);
        l->addRow("Учебное заведение:", i); l->addRow("Факультет:", f); l->addRow("Специальность:", s); l->addRow("Год окончания:", y);
        QDialogButtonBox *b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        l->addRow(b);
        connect(b, &QDialogButtonBox::accepted, &d, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, &d, &QDialog::reject);
        if(d.exec() == QDialog::Accepted) {
            DatabaseManager::instance().addEducation(userId, i->text(), f->text(), s->text(), y->value());
            loadAllData(); // Перезагрузка таблицы
        }
    }

    void addExpDialog() {
        QDialog d(this); d.setWindowTitle("Добавить опыт работы"); d.resize(400, 300);
        QFormLayout *l = new QFormLayout(&d);
        QLineEdit *o = new QLineEdit, *p = new QLineEdit;
        QDateEdit *ds = new QDateEdit(QDate::currentDate().addYears(-1)); ds->setDisplayFormat("MM.yyyy");
        QDateEdit *de = new QDateEdit(QDate::currentDate()); de->setDisplayFormat("MM.yyyy");
        QTextEdit *du = new QTextEdit;
        l->addRow("Организация:", o); l->addRow("Должность:", p);
        l->addRow("Начало работы:", ds); l->addRow("Окончание:", de); l->addRow("Обязанности:", du);
        QDialogButtonBox *b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        l->addRow(b);
        connect(b, &QDialogButtonBox::accepted, &d, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, &d, &QDialog::reject);
        if(d.exec() == QDialog::Accepted) {
            DatabaseManager::instance().addExperience(userId, o->text(), p->text(), ds->date(), de->date(), du->toPlainText());
            loadAllData();
        }
    }

    void deleteRow(QTableWidget *t, QString table) {
        int r = t->currentRow();
        if(r < 0) return;
        int id = t->item(r, 0)->data(Qt::UserRole).toInt();
        if(table == "education") DatabaseManager::instance().deleteEducation(id);
        else DatabaseManager::instance().deleteExperience(id);
        t->removeRow(r);
    }
};

#endif // SPECIALISTWINDOW_H
