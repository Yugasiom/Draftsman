#include "creator_header.hpp"
#include "ui_creator_window.h"
#include "./source/custom_object/game_field_widget/game_field_widget.hpp"



void Creator::on_Play_clicked()
{
    // сохраняем поле всегда
    field_save();

    // проверка поля
    QString m = check_missing_elements();
    if(!validate_before_run()) {
        show_missing_window(m);
        return; // не запускаем
    }

    // проверка кода
    QStringList lin = ui->Code->toPlainText().split('\n', Qt::SkipEmptyParts);
    if(!validate_program(lin)) {
        return; // не запускаем
    }

    // запуск только если всё корректно
    plan       = build_plan(lin);
    plan_index = 0;
    step_made  = 0;
    game_ended = false;
    reach_flag = false;
    ene_init   = f->count_enemies();

    if(!com_timer) {
        com_timer = new QTimer(this);
        connect(com_timer, &QTimer::timeout, this, &Creator::executeNextCommand);
    }

    com_timer->start(275);
}


bool Creator::validate_before_run() const
{
    int32_t act = 0, flags = 0, y, x;
    for(y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) act++;
            if(cell.ti == TYPE_FLAG) flags++;
        }
    }
    QPoint pp = f->find_player();
    bool hasPlayer = (pp.x() >= 0 && pp.y() >= 0);
    return (act >= 2 && hasPlayer && flags >= 1);
}


int32_t Creator::default_steps_by_case(const QString &w) const
{
    bool hu = false, hl = false;
    for(QChar ch : w) {
        hu |= ch.isUpper();
        hl |= ch.isLower();
    }
    return (hu && !hl) ? 8 : 1;
}


int32_t Creator::parse_steps(const QString &line, const QString &key)
{
    // проверяем, есть ли скобки
    bool hasParens = line.contains("(") && line.contains(")");

    // проверяем регистр
    bool allUpper = true, hasLower = false;
    for(QChar ch : line) {
        if(ch.isLower()) hasLower = true;
        if(ch.isLetter() && ch.isLower()) allUpper = false;
    }

    // если оператор в верхнем регистре и есть скобки → ошибка
    if(allUpper && !hasLower && hasParens) {
        show_missing_window("Ошибка: для операторов в верхнем регистре скобки запрещены");
        return 0;
    }

    // обычная обработка для нижнего регистра
    QRegularExpression re(QRegularExpression::escape(key) + R"(\s*\((\d+)\))",
                          QRegularExpression::CaseInsensitiveOption);
    auto m = re.match(line);
    if(!m.hasMatch()) {
        if(hasParens) {
            show_missing_window("Ошибка: внутри скобок должно быть число от 1 до 8");
            return 0;
        }
        return default_steps_by_case(line);
    }

    int32_t s = m.captured(1).toInt();
    if(s < 1 || s > 8) {
        show_missing_window("Ошибка: число в скобках должно быть от 1 до 8");
        return 0;
    }

    return s;
}


Command Creator::parse_line(const QString &line)
{
    QString s = line.trimmed();
    if(s.isEmpty()) {
        return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
    }

    // комментарии и любой текст — игнорируем
    if(s.startsWith("(") && s.endsWith(")")) {
        return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
    }

    int32_t steps;
    if(s.startsWith("вправо", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вправо");
        return {CmdType::MOVE, line, +1, 0, steps, 0, QString()};
    }
    if(s.startsWith("влево", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "влево");
        return {CmdType::MOVE, line, -1, 0, steps, 0, QString()};
    }
    if(s.startsWith("вверх", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вверх");
        return {CmdType::MOVE, line, 0, -1, steps, 0, QString()};
    }
    if(s.startsWith("вниз", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вниз");
        return {CmdType::MOVE, line, 0, +1, steps, 0, QString()};
    }
    if(s.startsWith("красить", Qt::CaseInsensitive)) {
        return {CmdType::PAINT, line, 0, 0, 0, 0, QString()};
    }

    // всё остальное игнорируем
    return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
}


QVector<Command> Creator::build_plan(const QStringList &lines)
{
    QVector<Command> out;
    auto emit_single_move = [&](const Command &mv) {
        int count = std::max<int>(1, mv.steps);
        for (int i = 0; i < count; ++i) {
            Command one = mv;
            one.steps = 1;
            out.push_back(one);
        }
    };

    for (const auto &line : lines) {
        Command cmd = parse_line(line);
        if (cmd.type == CmdType::UNKNOWN) continue;
        if (cmd.type == CmdType::MOVE) {
            emit_single_move(cmd);
        } else {
            out.push_back(cmd);
        }
    }
    return out;
}


bool Creator::validate_program(const QStringList &lines)
{
    for (const auto &raw : lines) {
        QString s = raw.trimmed();
        if(s.isEmpty()) continue;

        if (s.startsWith("вправо", Qt::CaseInsensitive) ||
            s.startsWith("влево",  Qt::CaseInsensitive) ||
            s.startsWith("вверх",  Qt::CaseInsensitive) ||
            s.startsWith("вниз",   Qt::CaseInsensitive)) {

            // 1) Запрет скобок для ВЕРХНЕГО регистра
            bool allUpper = true, hasLower = false;
            for (QChar ch : s) {
                if (ch.isLower()) { hasLower = true; allUpper = false; }
                else if (ch.isLetter() && ch.isUpper()) { /* keep */ }
            }
            bool hasParens = s.contains("(") || s.contains(")");
            if (allUpper && !hasLower && hasParens) {
                show_missing_window("Ошибка: для операторов в верхнем регистре скобки запрещены");
                return false;
            }

            // 2) Если есть скобки, но формат не совпал → ошибка
            QRegularExpression re(
                R"((?:вправо|влево|вверх|вниз)\s*\((?<num>\d+)\))",
                QRegularExpression::CaseInsensitiveOption
                );
            auto m = re.match(s);

            if (hasParens && !m.hasMatch()) {
                show_missing_window("Ошибка: внутри скобок должно быть число от 1 до 8");
                return false;
            }

            // 3) Если формат совпал, проверяем диапазон
            if (m.hasMatch()) {
                bool ok = false;
                int n = m.captured("num").toInt(&ok);
                if (!ok || n < 1 || n > 8) {
                    show_missing_window("Ошибка: число после оператора движения должно быть от 1 до 8");
                    return false;
                }
            }
        }
        else if(s.startsWith("красить", Qt::CaseInsensitive)) {
            continue; // допустимая команда
        }
        else {
            continue; // мусор игнорируем
        }
    }
    return true;
}


void Creator::executeNextCommand()
{
    if (game_ended || plan_index >= plan.size()) {
        com_timer->stop();
        conclude_if_needed();
        return;
    }

    const Command &cmd = plan[plan_index];

    if (cmd.type == CmdType::MOVE) {
        QPoint p = f->find_player();
        int nx = p.x() + cmd.dx;
        int ny = p.y() + cmd.dy;

        if (nx >= 0 && nx < f->cols() && ny >= 0 && ny < f->rows()) {
            const auto &target = f->get_cells()[ny][nx];

            f->move_player_step(cmd.dx, cmd.dy);
            step_made++;

            // Победа уже засчитывается внутри move_player_step при удалении флага
            if (target.ti == TYPE_FLAG) {
                reach_flag = true;
                game_ended = true;
                com_timer->stop();
                conclude_if_needed();
                return;
            }
        } else {
            step_made++;
        }
    } else if (cmd.type == CmdType::PAINT) {
        QPoint np = f->find_player();
        if (np.y() < 0) { ++plan_index; return; }

        auto &cell = f->access_cells()[np.y()][np.x()];

        if (cell.ti == TYPE_ENEMY) {
            cell.ti = TYPE_FIELD;
            cell.c  = COLOR_PAINTED;
            f->update();
        } else if (cell.ti == TYPE_FLAG) {
            cell.ti = TYPE_FIELD;
            f->update();
        } else if (cell.ti == TYPE_FIELD) {
            cell.c = COLOR_PAINTED;
            f->update();
        }
    }

    ++plan_index;
}


void Creator::conclude_if_needed()
{
    if(!reach_flag) {
        show_result_window(false, false, true); // проигрыш: не дошёл до флага
        field_load();
        return;
    }

    // если дошёл до флага → победа
    if(ene_init == 0) {
        // врагов не было → победа за минимальные шаги
        show_result_window(true, false, false);
    } else if(f->count_enemies() == 0) {
        // враги были, но все закрашены → победа
        show_result_window(false, true, false);
    } else {
        // враги остались, но флаг достигнут → победа (основное задание не выполнено)
        show_result_window(false, false, false);
    }

    field_load();
}


void Creator::show_result_window(bool wm, bool wpa, bool lose)
{
    QString m;
    if(lose) {
        m = "Проигрыш: робот не дошёл до финиша";
    } else if(wpa) {
        m = "Победа: вы помогли роботу добраться\nдо финиша закрасив всех врагов!!!";
    } else if(wm) {
        m = "Победа: вы помогли роботу добраться до\nфиниша за минимальное количество шагов!!!";
    } else {
        m = "Победа: основное задание не выполнено,\nно вы помогли роботу добраться до финиша...";
    }

    show_missing_window(m);
}


QString Creator::check_missing_elements() const
{
    int32_t y, x, act = 0, flags = 0;
    for(y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) act++;
            if(cell.ti == TYPE_FLAG) flags++;
        }
    }
    QPoint pp = f->find_player();
    bool hasPlayer = (pp.x() >= 0 && pp.y() >= 0);

    QStringList m;
    if(act < 2) {
        m << "На поле нет флага и игрока";
    } else if(!hasPlayer) {
        m << "На поле должен быть игрок "
             "и\nстоять на свободной активной клетке";
    } else if(flags < 1) {
        m << "На поле должен стоять хотя бы один флаг";
    }
    return m.join("\n");
}


void Creator::show_missing_window(const QString &m)
{
    QDialog d(this);
    d.setWindowTitle("Draftsman");
    d.setWindowModality(Qt::ApplicationModal);
    d.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    d.setFixedSize(450, 200);
    QVBoxLayout l(&d);
    QLabel a(m, &d);
    a.setAlignment(Qt::AlignCenter);
    l.addWidget(&a);
    QPushButton b("Хорошо", &d);
    b.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    l.addWidget(&b);
    QObject::connect(&b, &QPushButton::clicked, &d, &QDialog::accept);
    d.exec();
}


QString Creator::level_save_path() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); // подняться из build/ в корень проекта
    dir.mkpath("source/saves/lvls"); // убедиться, что папка существует
    return dir.filePath("source/saves/lvls/sample.dat");
}


void Creator::field_save()
{
    QString path = level_save_path();
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << path;
        return;
    }

    QDataStream out(&file);
    out << f->rows() << f->cols();

    for(int y = 0; y < f->rows(); ++y) {
        for(int x = 0; x < f->cols(); ++x) {
            const GameFieldWidget::Cell &cell = f->get_cells()[y][x];
            out << cell.ti << cell.c;
        }
    }

    // Сохраняем позицию игрока отдельно
    QPoint pp = f->get_player_pos();
    out << pp.x() << pp.y();
}


void Creator::field_load()
{
    QString path = level_save_path();
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения:" << path;
        return;
    }

    QDataStream in(&file);
    int rows, cols;
    in >> rows >> cols;

    f->set_size(rows, cols);

    for(int y = 0; y < rows; ++y) {
        for(int x = 0; x < cols; ++x) {
            GameFieldWidget::Cell cell;
            in >> cell.ti >> cell.c;
            f->set_cell(y, x, cell);
        }
    }

    // Загружаем позицию игрока
    int px, py;
    if (in.status() == QDataStream::Ok && !in.atEnd()) {
        in >> px >> py;
        f->set_player_pos(QPoint(px, py));
    }

    f->update();
}



Creator::Creator(QWidget *parent) :
                 QMainWindow(parent), ui(new Ui::Draftsman)
{
    ui->setupUi(this);




    ui->Field->setChecked(true);




    f = new GameFieldWidget(this);
    f->save_initial_state();

    auto *l = new QGridLayout(ui->GameFieldContainer);
    ui->GameFieldContainer->setLayout(l);
    l->addWidget(f);



    ui->Height->setRange(1, 9);
    ui->Width->setRange(1, 9);



    connect(ui->Secured, &QCheckBox::toggled                               , this, [=](uint16_t lo)
    {
        ui->Height->setEnabled(!lo);
        ui->Width->setEnabled(!lo);
    });

    connect(ui->Height, QOverload<int32_t>::of(&RPTESpinBox::valueChanged) , this, [=](uint16_t h)
    {
        if(h == 1 && ui->Width->value() == 1) {
            ui->Width->setValue(2);
        }

        f->set_size(h, ui->Width->value());
    });

    connect(ui->Width , QOverload<int32_t>::of(&RPTESpinBox::valueChanged) , this, [=](uint16_t w)
    {
        if(ui->Height->value() == 1 && w == 1) {
            ui->Height->setValue(2);
        }

        f->set_size(ui->Height->value(), w);
    });

    connect(ui->Field , &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(1);
        }
    });

    connect(ui->Flag  , &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(2);
        }
    });

    connect(ui->Player, &RPTERadioButton::toggled  , this, [=](uint16_t c)
    {
        if(c) {
            f->set_current_tool(3);
        }
    });
}



Creator::~Creator()
{
    delete ui;
}
