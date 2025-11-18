#include "main_intro_window_dialog.hpp"


MIWD::MIWD(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Draftsman");
    setFixedSize(800, 500);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Добро пожаловать в обучающую программу «Draftsman»", this);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);

    // Кнопка "Создать"
    QPushButton *createBtn = new QPushButton("Создать", this);
    createBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(createBtn, 0, Qt::AlignCenter);

    // Кнопка "Открыть"
    QPushButton *openBtn = new QPushButton("Открыть", this);
    openBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(openBtn, 0, Qt::AlignCenter);

    // Обработчики
    connect(createBtn, &QPushButton::clicked, this, [this]() {
        accept(); // закрываем окно и запускаем редактор
    });

    connect(openBtn, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "Открыть уровень",
            QCoreApplication::applicationDirPath(),
            "Уровни (*.dat)"
            );
        if(fileName.isEmpty()) {
            return;
        }

        QFile f(fileName);
        if(!f.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл уровня.");
            return;
        }

        // если всё ок — закрываем окно и запускаем редактор с этим уровнем
        accept();
        // здесь можно передать путь в конструктор Creator или через setLevelPath()
    });
}
