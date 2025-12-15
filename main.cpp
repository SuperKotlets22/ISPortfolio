#include <QApplication>
#include "LoginWindow.h"
#include "Database.h"
#include <QStyleFactory>

void setGlobalStyle(QApplication &app) {
    app.setStyle(QStyleFactory::create("Fusion"));

    // ГЛОБАЛЬНАЯ ТЕМНАЯ ТЕМА (ПРИМЕНЯЕТСЯ КО ВСЕМУ)
    app.setStyleSheet(R"(
        /* --- ОСНОВНОЙ ФОН --- */
        QMainWindow, QWidget {
            background-color: #121212;
            color: #e0e0e0;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
        }

        /* --- ПОЛЯ ВВОДА --- */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDateEdit, QComboBox {
            background-color: #1e1e1e;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 8px;
            color: #fff;
            selection-background-color: #bb86fc;
        }
        QLineEdit:focus, QTextEdit:focus {
            border: 1px solid #bb86fc;
            background-color: #252525;
        }

        /* --- КНОПКИ --- */
        QPushButton {
            background-color: #333;
            border: 1px solid #444;
            border-radius: 6px;
            padding: 8px 16px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #444; border-color: #bb86fc; }
        QPushButton:pressed { background-color: #bb86fc; color: black; }

        /* Кнопки-ссылки */
        QPushButton#TextBtn { background: transparent; border: none; color: #888; }
        QPushButton#TextBtn:hover { color: #bb86fc; text-decoration: underline; }

        /* Кнопки действий (Красные/Зеленые/Фиолетовые переопределяются локально) */

        /* --- ТАБЛИЦЫ И СПИСКИ --- */
        QTableWidget, QListWidget {
            background-color: #1e1e1e;
            gridline-color: #333;
            border: 1px solid #333;
            border-radius: 6px;
            alternate-background-color: #252525;
        }
        QHeaderView::section {
            background-color: #2c2c2c;
            color: #bb86fc;
            padding: 6px;
            border: none;
            font-weight: bold;
        }
        QTableWidget::item:selected, QListWidget::item:selected {
            background-color: #3700b3;
            color: white;
        }

        /* --- ГРУППЫ (РАМКИ) --- */
        QGroupBox {
            border: 1px solid #444;
            border-radius: 8px;
            margin-top: 20px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #bb86fc;
        }

        /* --- СКРОЛЛБАРЫ --- */
        QScrollBar:vertical { background: #121212; width: 10px; margin: 0; }
        QScrollBar::handle:vertical { background: #444; min-height: 20px; border-radius: 5px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        
        /* --- ВКЛАДКИ --- */
        QTabWidget::pane { border: 1px solid #444; }
        QTabBar::tab {
            background: #1e1e1e; color: #888; padding: 10px 20px;
            border-top-left-radius: 6px; border-top-right-radius: 6px;
        }
        QTabBar::tab:selected { background: #333; color: #bb86fc; border-bottom: 2px solid #bb86fc; }
    )");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    setGlobalStyle(app);

    if (!DatabaseManager::instance().connectToDb()) return -1;
    DatabaseManager::instance().initDb();

    LoginWindow loginWindow;
    loginWindow.showMaximized(); // Запуск на весь экран

    return app.exec();
}