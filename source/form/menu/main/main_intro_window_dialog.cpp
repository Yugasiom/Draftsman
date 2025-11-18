#include "main_intro_window_dialog.hpp"



MIWD::MIWD(QWidget *p) : QDialog(p)
{
    setFixedSize(800, 500);
    setWindowTitle("Draftsman");

    auto *ml = new QVBoxLayout(this);
    ml->setContentsMargins(24, 16, 24, 16);
    ml->setSpacing(0);

    progress = new QProgressBar(this);
    progress->setRange(0, 100);
    progress->setValue(0);
    progress->setTextVisible(false);
    progress->setFixedHeight(20); // увеличили высоту по вертикали
    progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    progress->setStyleSheet("QProgressBar::chunk {"
                            "  background-color: #FB8C00;"
                            "  border-radius: 5px;"
                            "}");
    ml->addWidget(progress);

    QLabel *hl = new QLabel("Добро пожаловать в обучающую программу «Draftsman»", this);
    hl->setAlignment(Qt::AlignCenter);
    hl->setWordWrap(true);
    hl->setStyleSheet("font-size: 32px; font-weight: bold;");
    ml->addSpacing(16);
    ml->addWidget(hl);
    ml->addSpacing(24);

    stack = new QStackedWidget(this);
    ml->addWidget(stack, 1);

    // --- Страница 0 ---
    auto *p0 = new QWidget(this);
    auto *p0l = new QVBoxLayout(p0);
    QPushButton *createBtn = new QPushButton("Создать", p0);
    QPushButton *openBtn   = new QPushButton("Открыть", p0);
    createBtn->setFixedWidth(220);
    openBtn->setFixedWidth(220);
    p0l->addStretch();
    p0l->addWidget(createBtn, 0, Qt::AlignCenter);
    p0l->addSpacing(12);
    p0l->addWidget(openBtn, 0, Qt::AlignCenter);
    p0l->addStretch();
    stack->addWidget(p0);

    // --- Страница 1 ---
    auto *p1 = new QWidget(this);
    auto *p1l = new QVBoxLayout(p1);
    pbtn = new QRadioButton("Прототип", p1);
    fbtn = new QRadioButton("Лес", p1);
    pbtn->setChecked(true);
    pbtn->setStyleSheet("font-size: 28px; color: rgb(200,220,220);");
    fbtn->setStyleSheet("font-size: 28px; color: rgb(144,238,144);");
    pbtn->setFixedWidth(200);
    fbtn->setFixedWidth(200);
    auto *rco = new QWidget(p1);
    auto *ral = new QVBoxLayout(rco);
    ral->setSpacing(12);
    ral->addWidget(pbtn, 0, Qt::AlignCenter);
    ral->addWidget(fbtn, 0, Qt::AlignCenter);
    p1l->addStretch();
    p1l->addWidget(rco);
    p1l->addStretch();
    auto *br1 = new QHBoxLayout();
    QPushButton *backBtn1 = new QPushButton("Назад", p1);
    QPushButton *nextBtn1 = new QPushButton("Далее", p1);
    br1->addWidget(backBtn1, 0, Qt::AlignLeft);
    br1->addStretch();
    br1->addWidget(nextBtn1, 0, Qt::AlignRight);
    p1l->addLayout(br1);
    stack->addWidget(p1);

    // --- Страница 2 ---
    auto *p2 = new QWidget(this);
    auto *p2l = new QVBoxLayout(p2);
    nedit = new QLineEdit(p2);
    nedit->setMaxLength(24);
    nedit->setPlaceholderText("Введите название (до 24 символов)");
    nedit->setAlignment(Qt::AlignCenter);
    nedit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // растянуто по горизонтали
    p2l->addStretch();
    p2l->addWidget(nedit);
    p2l->addStretch();
    auto *br2 = new QHBoxLayout();
    QPushButton *backBtn2 = new QPushButton("Назад", p2);
    nbtn = new QPushButton("Завершить", p2);
    br2->addWidget(backBtn2, 0, Qt::AlignLeft);
    br2->addStretch();
    br2->addWidget(nbtn, 0, Qt::AlignRight);
    p2l->addLayout(br2);
    stack->addWidget(p2);

    // --- Логика ---
    connect(createBtn, &QPushButton::clicked, this, [this, hl]() {
        stack->setCurrentIndex(1);
        progress->setValue(50);
        hl->setText("Выберите локацию");
    });
    connect(openBtn, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть уровень", "", "Уровни (*.dat)");
        if(!fileName.isEmpty()) accept();
    });
    connect(backBtn1, &QPushButton::clicked, this, [this, hl]() {
        stack->setCurrentIndex(0);
        progress->setValue(0);
        hl->setText("Добро пожаловать в обучающую программу «Draftsman»");
    });
    connect(nextBtn1, &QPushButton::clicked, this, [this, hl]() {
        if(fbtn->isChecked()) {
            THEME.color_field = QColor(144, 238, 144);
            THEME.color_empty = QColor(135, 206, 250);
            THEME.color_paint = QColor(34, 139, 34);
        } else {
            THEME.color_field = QColor(200, 220, 220);
            THEME.color_empty = QColor(255, 255, 255);
            THEME.color_paint = QColor(180, 198, 198);
        }
        stack->setCurrentIndex(2);
        progress->setValue(100);
        hl->setText("Назовите уровень");
        nbtn->setText("Завершить");
    });
    connect(backBtn2, &QPushButton::clicked, this, [this, hl]() {
        stack->setCurrentIndex(1);
        progress->setValue(50);
        hl->setText("Выберите локацию");
    });
    connect(nbtn, &QPushButton::clicked, this, [this]() {
        QString nm = nedit->text().trimmed();
        if(nm.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите название уровня.");
            return;
        }
        if(nm.contains(QRegularExpression("[/\\\\\\-.,]"))) {
            QMessageBox::warning(this, "Ошибка", "Название содержит запрещённые символы: / \\ - . ,");
            return;
        }
        accept();
    });

    THEME = {QColor(200, 220, 220),
             QColor(255, 255, 255),
             QColor(180, 198, 198)};
}


void MIWD::goNext()
{
    int idx = stack->currentIndex();
    if(idx == 1) {
        if(fbtn->isChecked()) {
            THEME.color_field = QColor(144, 238, 144);
            THEME.color_empty = QColor(135, 206, 250);
            THEME.color_paint = QColor(34, 139, 34);
        } else {
            THEME.color_field = QColor(200, 220, 220);
            THEME.color_empty = QColor(255, 255, 255);
            THEME.color_paint = QColor(180, 198, 198);
        }
        stack->setCurrentIndex(2);
        progress->setValue(2);
    } else if(idx == 2) {
        QString nm = nedit->text().trimmed();
        if(nm.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите название уровня.");
            return;
        }
        if(nm.contains(QRegularExpression("[/\\\\\\-.,]"))) {
            QMessageBox::warning(this, "Ошибка", "Название содержит запрещённые символы: / \\ - . ,");
            return;
        }
        accept();
    }
}


QString MIWD::level_name() const {
    return nedit->text().trimmed();
}


MIWD::LocationType MIWD::location() const {
    return fbtn->isChecked() ? FOREST : PROTOTYPE;
}
