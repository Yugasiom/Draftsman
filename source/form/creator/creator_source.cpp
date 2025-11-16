#include "creator_header.hpp"
#include "ui_creator_window.h"
#include "./source/custom_object/game_field_widget/game_field_widget.hpp"



bool Creator::validate_before_run() const
{
    int32_t act = 0, play = 0, flags = 0,
            y      , x                  ;
    for(y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) {
                act++;
            }

            if(cell.ti == TYPE_PLAYER) {
                play++;
            }

            if(cell.ti == TYPE_FLAG) {
                flags++;
            }
        }
    }


    return (act >= 2 && play == 1 && flags >= 1);
}


int32_t Creator::default_steps_by_case(const QString &w) const
{
        bool  hu  = false, hl = false;
    for(QChar ch  : w) {
              hu |= ch.isUpper();
              hl |= ch.isLower();
    }


    return (hu && !hl) ? 9 : 1;
}


int32_t Creator::parse_steps(const QString &line, const QString &key) const
{
    QRegularExpression re(R"(\b)" + QRegularExpression::escape(key) + R"(\b\s*(\d+)?)",
                          QRegularExpression::CaseInsensitiveOption);
    auto m = re.match(line);
    if(!m.hasMatch()) {
        return 1;
    }

    auto num = m.captured(1);
    if(num.isEmpty()) {
        return default_steps_by_case(m.captured(0));
    }

    int32_t s = num.toInt();


    return qBound(1, s, 9);
}


Command Creator::parse_line(const QString &line) const
{
    QString s = line.trimmed();
    if(s.isEmpty()) {
        return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
    }

    if(s.startsWith("(") && s.endsWith(")")) {
        return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
    }

    QString S = s.toUpper();
    int32_t steps;
    if(S.startsWith("ВПРАВО")) {
        steps = parse_steps(s, "вправо");
        return {CmdType::MOVE, line, +1, 0, steps, 0, QString()};
    }

    if(S.startsWith("ВЛЕВО")) {
        steps = parse_steps(s, "влево");
        return {CmdType::MOVE, line, -1, 0, steps, 0, QString()};
    }

    if(S.startsWith("ВВЕРХ")) {
        steps = parse_steps(s, "вверх");
        return {CmdType::MOVE, line, 0, -1, steps, 0, QString()};
    }

    if(S.startsWith("ВНИЗ")) {
        steps = parse_steps(s, "вниз");
        return {CmdType::MOVE, line, 0, +1, steps, 0, QString()};
    }

    if(S.startsWith("КРАСИТЬ")) {
        return {CmdType::PAINT, line, 0, 0, 0, 0, QString()};
    }

    if(S.startsWith("ЦИКЛ")) {
        QRegularExpression re(R"(цикл\s+(\d+))", QRegularExpression::CaseInsensitiveOption);
        auto m = re.match(s);
        int32_t k = m.hasMatch() ? qBound(1, m.captured(1).toInt(), 9) : 1;
        return {CmdType::BLOCK_LOOP, line, 0, 0, 0, k, QString()};
    }

    if(S.startsWith("ЕСЛИ")) {
        int32_t pos = S.indexOf("ТО");
        QString cond = (pos > 0 ? s.left(pos).mid(QString("если").size()).trimmed() : QString());
        return {CmdType::BLOCK_IF, line, 0, 0, 0, 0, cond};
    }

    if(S == "ТОЧКА") {
        return {CmdType::BLOCK_END, line, 0, 0, 0, 0, QString()};
    }

        return {CmdType::UNKNOWN, line, 0, 0, 0, 0, QString()};
}


QVector<Command> Creator::build_plan(const QStringList &lines) const
{
    QVector<Command> out, stack, buff;
    auto fbto = [&](const QVector<Command> &buf) {
        for(const auto &cmd : buf) {
            out.push_back(cmd);
        }
    };

    for(const auto &line : lines) {
        Command cmd = parse_line(line);
        if(cmd.type == CmdType::BLOCK_LOOP) {
            stack.push_back(cmd);
            buff.clear();


            continue;
        }

        if(cmd.type == CmdType::BLOCK_IF) {
            stack.push_back(cmd);
            buff.clear();


            continue;
        }

        if(cmd.type == CmdType::BLOCK_END) {
            if(stack.isEmpty()) {
                continue;
            }

            auto blk = stack.takeLast();
            if(blk.type == CmdType::BLOCK_LOOP) {
                int32_t i;
                for(i = 0; i < blk.loop_count; ++i) {
                    fbto(buff);
                }
            } else if(blk.type == CmdType::BLOCK_IF) {
                if(eval_condition(blk.condition)) {
                    fbto(buff);
                }
            }

            buff.clear();


            continue;
        }

        if(stack.isEmpty()) {
            out.push_back(cmd);
        } else {
            buff.push_back(cmd);
        }
    }


    return out;
}


bool Creator::eval_condition(const QString &cond) const
{
    QString c = cond.trimmed().toLower();
    if(c.isEmpty()) {
        return false;
    }

    if(c.contains("enemy_exists")) {
        return f->count_enemies() > 0;
    }

    if(c.contains("not_enemy_exists")) {
        return f->count_enemies() == 0;
    }

    if(c.contains("on_flag")) {
        QPoint p = f->find_player();
        if(p.y() < 0) {
            return false;
        }


        return f->get_cells()[p.y()][p.x()].ti == TYPE_FLAG;
    }
    if(c.contains("under_painted")) {
        QPoint p = f->find_player();
        if(p.y() < 0) {
            return false;
        }


        return f->get_cells()[p.y()][p.x()].c == COLOR_FIELD;
    }


    return false;
}


void Creator::on_Play_clicked()
{
    QString m = check_missing_elements();
    if(!validate_before_run()) {
        show_missing_window(m);


        return;
    }

    QStringList lines = ui->Code->toPlainText().split('\n', Qt::SkipEmptyParts),
          clean_lines                                                          ;
    QString s, S                                                               ;
    int32_t i                                                                  ;
    for(i = 0; i < lines.size(); ++i) {
        s = lines.at(i).trimmed();
        if(s.isEmpty()) {
            continue;
        }

        if(s.startsWith("(") && s.endsWith(")")) {
            clean_lines.push_back(s);


            continue;
        }

        S = s.toUpper();
        if(S.startsWith("ВПРАВО")   ||
            S.startsWith("ВЛЕВО")   ||
            S.startsWith("ВВЕРХ")   ||
            S.startsWith("ВНИЗ")    ||
            S.startsWith("КРАСИТЬ") ||
            S.startsWith("ЕСЛИ")    ||
            S.startsWith("ЦИКЛ")    ||
            S == "ТО"               ||
            S == "ТОЧКА") {
            clean_lines.push_back(s);
        }
    }

    ui->Code->setText(clean_lines.join("\n"));
    plan          = build_plan(clean_lines);
    plan_index    = 0;
    step_made     = 0;
    game_ended    = false;
    reach_flag    = false;
    ene_init      = f->count_enemies();
    if(!com_timer) {
        com_timer = new QTimer(this);
        connect(com_timer, &QTimer::timeout, this, &Creator::executeNextCommand);
    }

    com_timer->start(250);
}


void Creator::executeNextCommand()
{
    if(game_ended || plan_index >= plan.size()) {
        com_timer->stop();
        conclude_if_needed();


        return;
    }

    const Command &cmd = plan[plan_index];
    int32_t i, nx, ny;
    if(cmd.type == CmdType::MOVE) {
        for(i = 0; i < cmd.steps; ++i) {
            QPoint p = f->find_player();
            nx = p.x() + cmd.dx;
            ny = p.y() + cmd.dy;
            if(nx < 0 || nx >= f->cols() || ny < 0 || ny >= f->rows()) {
                break;
            }

            bool hf = f->move_player_step(cmd.dx, cmd.dy);
            step_made++;
            if(hf) {
                reach_flag = true;
                game_ended = true;
                com_timer->stop();
                conclude_if_needed();


                return;
            }
        }
    } else if(cmd.type == CmdType::PAINT) {
        f->paint_under_player();
    }

    ++plan_index;
}


void Creator::conclude_if_needed()
{
    if(!reach_flag) {
        show_result_window(false, false, true);


        return;
    }

    if(ene_init == 0) {
        show_result_window(true, false, false);
    } else {
        bool ap = (f->count_enemies() == 0);
        show_result_window(false, ap, !ap);
    }
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
        m = "Победа: основное задание не выполнено...";
    }

    show_missing_window(m);
}


QString Creator::check_missing_elements() const
{
    int32_t y, x, act = 0, play = 0, flags = 0;
    for(    y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) {
                act++;
            }

            if(cell.ti == TYPE_PLAYER) {
                play++;
            }

            if(cell.ti == TYPE_FLAG) {
                flags++;
            }
        }
    }

    QStringList m;
    if(act < 2) {
        m << "На поле нет флага и игрока";
    } else if(play < 1) {
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
    QDialogButtonBox b(&d);
    QPushButton *bt = b.addButton("Окей", QDialogButtonBox::AcceptRole);
    l.addWidget(&b);
    connect(bt, &QPushButton::clicked, &d, &QDialog::accept);
    d.exec();
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
