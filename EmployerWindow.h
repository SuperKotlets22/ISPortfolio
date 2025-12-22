#ifndef EMPLOYERWINDOW_H
#define EMPLOYERWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QMessageBox>
#include <QPrinter>
#include <QFileDialog>
#include <QFileInfo>
#include "Database.h"

class EmployerWindow : public QMainWindow {
    Q_OBJECT
    QListWidget *userList;
    QTextBrowser *resumeView;

public:
    explicit EmployerWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Кабинет Работодателя");
        QWidget *central = new QWidget;
        setCentralWidget(central);
        QVBoxLayout *main = new QVBoxLayout(central);

        // Шапка
        QHBoxLayout *top = new QHBoxLayout();
        QLabel *logo = new QLabel("Поиск Кандидатов");
        logo->setStyleSheet("font-size: 24px; font-weight: bold; color: #bb86fc;");
        QPushButton *btnExit = new QPushButton("Выйти");
        btnExit->setStyleSheet("background-color: #cf6679; color: black; font-size: 14px;");
        top->addWidget(logo); top->addStretch(); top->addWidget(btnExit);
        main->addLayout(top);

        QSplitter *split = new QSplitter();
        split->setHandleWidth(2);
        split->setStyleSheet("QSplitter::handle { background-color: #444; }");
        main->addWidget(split);

        // Левая панель (Список)
        QWidget *left = new QWidget;
        QVBoxLayout *ll = new QVBoxLayout(left);
        ll->setContentsMargins(0,0,10,0);
        
        QLabel *lblList = new QLabel("Список специалистов:");
        lblList->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff;");
        ll->addWidget(lblList);
        
        userList = new QListWidget();
        userList->setStyleSheet("font-size: 14px; padding: 5px;");
        
        QPushButton *btnRef = new QPushButton("Обновить список");
        btnRef->setStyleSheet("font-size: 14px; padding: 10px;");
        ll->addWidget(userList); ll->addWidget(btnRef);
        split->addWidget(left);

        // Правая панель (Резюме)
        QWidget *right = new QWidget;
        QVBoxLayout *rl = new QVBoxLayout(right);
        rl->setContentsMargins(10,0,0,0);
        
        resumeView = new QTextBrowser();
        resumeView->setOpenExternalLinks(false);
        // Устанавливаем базовый стиль для виджета
        resumeView->setStyleSheet("background-color: #1e1e1e; border: 1px solid #333; padding: 10px;");
        // ! ВАЖНО: Принудительное увеличение масштаба !
        resumeView->zoomIn(2); 
        
        QPushButton *btnContact = new QPushButton("Показать зашифрованные контакты");
        btnContact->setStyleSheet("background-color: #03dac6; color: black; font-weight: bold; padding: 12px; font-size: 15px;");
        
        QPushButton *btnPdf = new QPushButton("Сохранить в PDF");
        btnPdf->setStyleSheet("background-color: #ff9800; color: black; font-weight: bold; padding: 12px; margin-top: 5px; font-size: 15px;");
        
        rl->addWidget(resumeView);
        rl->addWidget(btnContact);
        rl->addWidget(btnPdf);
        split->addWidget(right);
        
        // Резюме занимает 75% экрана
        split->setStretchFactor(0, 1);
        split->setStretchFactor(1, 3);

        loadList();

        connect(btnExit, &QPushButton::clicked, [this](){ emit logoutRequested(); this->close(); });
        connect(btnRef, &QPushButton::clicked, this, &EmployerWindow::loadList);
        connect(userList, &QListWidget::itemClicked, this, &EmployerWindow::loadResume);
        connect(btnContact, &QPushButton::clicked, this, &EmployerWindow::showContacts);
        connect(btnPdf, &QPushButton::clicked, this, &EmployerWindow::saveToPdf);
    }

signals:
    void logoutRequested();

private:
    void loadList() {
        userList->clear();
        auto list = DatabaseManager::instance().getAllSpecialists();
        for(auto &p : list) {
            QListWidgetItem *it = new QListWidgetItem(p.second);
            it->setData(Qt::UserRole, p.first);
            userList->addItem(it);
        }
    }

    void loadResume(QListWidgetItem *item) {
        int uid = item->data(Qt::UserRole).toInt();
        UserProfile p;
        bool hasProfile = DatabaseManager::instance().getProfile(uid, p);

        QString html = "<html><body style='font-family: Arial, sans-serif; color: #e0e0e0; background-color: #1e1e1e; font-size: 14pt;'>";
        
        // Шапка
        html += "<div style='border-bottom: 3px solid #bb86fc; padding-bottom: 15px; margin-bottom: 25px;'>";
        html += "<h1 style='color: #ffffff; margin:0; font-size: 28pt;'>" + (hasProfile ? p.surname + " " + p.name + " " + p.patronymic : "Без имени") + "</h1>";
        html += "<h2 style='color: #bb86fc; margin:10px 0; font-size: 20pt;'>" + (hasProfile ? p.jobTitle : "Должность не указана") + "</h2>";
        html += "</div>";
        
        if(hasProfile) {
            html += "<table style='width: 100%; color: #cccccc; font-size: 14pt;'>";
            html += "<tr><td style='padding: 5px;'><b>Зарплата:</b> " + QString::number(p.salary) + " руб.</td><td style='padding: 5px;'><b>График:</b> " + p.schedule + "</td></tr>";
            html += "<tr><td style='padding: 5px;'><b>Город:</b> " + p.city + "</td><td style='padding: 5px;'><b>Переезд:</b> " + (p.readyToMove ? "Да" : "Нет") + "</td></tr>";
            html += "<tr><td style='padding: 5px;'><b>Дата рождения:</b> " + p.birthDate.toString("dd.MM.yyyy") + "</td><td style='padding: 5px;'><b>Пол:</b> " + p.gender + "</td></tr>";
            html += "</table><hr style='border: 1px solid #444; margin: 20px 0;'>";
        }

        // Опыт
        html += "<h3 style='color: #03dac6; font-size: 18pt; margin-bottom: 15px;'>Опыт работы</h3>";
        auto expList = DatabaseManager::instance().getExperienceList(uid);
        if(!expList.isEmpty()) {
            for(const auto &e : expList) {
                html += "<div style='background-color: #2a2a2a; padding: 15px; border-radius: 8px; margin-bottom: 15px;'>";
                html += "<div style='font-weight: bold; font-size: 16pt; color: #fff;'>" + e.position + "</div>"; 
                html += "<div style='color: #aaa; font-size: 14pt; margin-top: 5px;'>" + e.orgName + " | " + e.startDate.toString("MM.yyyy") + " - " + e.endDate.toString("MM.yyyy") + "</div>";
                html += "<div style='margin-top: 10px; color: #eee; font-size: 14pt;'>" + e.duties + "</div></div>";
            }
        } else html += "<p style='color: #777; font-size: 14pt;'>Нет данных.</p>";

        // Образование
        html += "<h3 style='color: #03dac6; font-size: 18pt; margin-bottom: 15px;'>Образование</h3>";
        auto eduList = DatabaseManager::instance().getEducationList(uid);
        if(!eduList.isEmpty()) {
            for(const auto &e : eduList) {
                html += "<div style='margin-bottom: 15px; padding-left: 10px; border-left: 3px solid #444;'>";
                html += "<div style='font-size: 16pt; color: #fff;'><b>" + QString::number(e.year) + "</b> — " + e.institution + "</div>";
                html += "<div style='color: #ccc; font-size: 14pt; margin-top: 5px;'>" + e.faculty + ", " + e.specialty + "</div></div>";
            }
        } else html += "<p style='color: #777; font-size: 14pt;'>Нет данных.</p>";

        html += "</body></html>";
        resumeView->setHtml(html);
    }

    void showContacts() {
        if(userList->selectedItems().isEmpty()) return;
        int uid = userList->selectedItems().first()->data(Qt::UserRole).toInt();
        QString e, p;
        if(DatabaseManager::instance().getUserContacts(uid, e, p)) {
            QMessageBox::information(this, "Контакты (Расшифровано)", 
                                     "Email: " + e + "\nТелефон: " + p);
        }
    }

    void saveToPdf() {
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить резюме", "resume.pdf", "PDF Files (*.pdf)");
        if (fileName.isEmpty()) return;
        if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printer.setPageSize(QPageSize::A4);

        resumeView->print(&printer);
        QMessageBox::information(this, "Успех", "Файл успешно сохранен!");
    }
};

#endif // EMPLOYERWINDOW_H