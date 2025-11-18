#include "creator_header.hpp"
#include "ui_creator_window.h"
#include "./source/custom_object/game_field_widget/game_field_widget.hpp"



void Creator::on_Play_clicked()
{
    field_save();
    QString m = check_missing_elements();
    if(!validate_before_run()) {
        show_missing_window(m);


        return;
    }

    QStringList lin = ui->Code->toPlainText().split('\n', Qt::SkipEmptyParts);
    if(!validate_program(lin)) {
        return;
    }

    ui->Field->setEnabled(  false);
    ui->Flag->setEnabled(   false);
    ui->Player->setEnabled( false);
    ui->Secured->setEnabled(false);
    ui->Height->setEnabled( false);
    ui->Width->setEnabled(  false);
    f->set_tools_enabled(   false);
    plan          = build_plan(lin);
    plan_index    = 0;
    step_made     = 0;
    game_ended    = false;
    reach_flag    = false;
    ene_init      = f->count_enemies();
    if(!com_timer) {
        com_timer = new     QTimer(this);
        connect(com_timer, &QTimer::timeout, this, &Creator::executeNextCommand);
    }

    com_timer->start(275);
}


bool Creator::validate_before_run() const
{
    int32_t act = 0, flags = 0, y, x;
    QPoint pp;
    for(    y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) {
                act++;
            }

            if(cell.ti == TYPE_FLAG) {
                flags++;
            }
        }
    }

    pp = f->find_player();


    return (act >= 2 && (pp.x() >= 0 && pp.y() >= 0) && flags >= 1);
}


int32_t Creator::default_steps_by_case(const QString &w) const
{
    bool      hu = false, hl = false;
    for(QChar ch :  w) {
              hu |= ch.isUpper();
              hl |= ch.isLower();
    }


    return   (hu && !hl) ? 8 : 1;
}


int32_t Creator::parse_steps(const QString &li, const QString &key)
{
    bool hp = li.contains("(") && li.contains(")"), au = true, hl = false;
    for(QChar ch : li) {
        if(   ch.isLower()) {
              hl = true;
        }

        if(   ch.isLetter() && ch.isLower()) {
              au = false;
        }
    }

    if(au && !hl && hp) {
        show_missing_window("Ошибка: для операторов в\nверхнемрегистре скобки запрещены");


        return 0;
    }

    QRegularExpression re(QRegularExpression::escape(key) + R"(\s*\((\d+)\))",
                          QRegularExpression::CaseInsensitiveOption);
    auto m = re.match(li);
    if(!m.hasMatch()) {
        if(hp) {
            show_missing_window("Ошибка: внутри скобок должно\nбыть число от 1 до 8");


            return 0;
        }


        return default_steps_by_case(li);
    }

    int32_t s = m.captured(1).toInt();
    if(s < 1 || s > 8) {
        show_missing_window("Ошибка: число в скобках должно\nбыть от 1 до 8");


        return 0;
    }


    return s;
}


Command Creator::parse_line(const QString &lin)
{
    QString s = lin.trimmed();
    if(s.isEmpty()) {
        return {CmdType::UNKNOWN, lin, QString(), 0 , 0 ,   0  , 0};
    }

    if(s.startsWith("(") && s.endsWith(")")) {
        return {CmdType::UNKNOWN, lin, QString(), 0 , 0 ,   0  , 0};
    }

    int32_t steps;
    if(s.startsWith("вправо", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вправо");


        return {CmdType::MOVE   , lin, QString(), +1, 0 , steps, 0};
    }

    if(s.startsWith("влево", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "влево");


        return {CmdType::MOVE   , lin, QString(), -1, 0 , steps, 0};
    }

    if(s.startsWith("вверх", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вверх");


        return {CmdType::MOVE   , lin, QString(), 0 , -1, steps, 0};
    }

    if(s.startsWith("вниз", Qt::CaseInsensitive)) {
        steps = parse_steps(s, "вниз");


        return {CmdType::MOVE   , lin, QString(), 0 , +1, steps, 0};
    }

    if(s.startsWith("красить", Qt::CaseInsensitive)) {
        return {CmdType::PAINT  , lin, QString(), 0 , 0 ,   0  , 0};
    }


    return {CmdType::UNKNOWN    , lin, QString(), 0 , 0 ,   0  , 0};
}


QVector<Command> Creator::build_plan(const QStringList &lin)
{
    QVector<Command> out;
    auto emit_single_move = [&](const Command &mv) {
        int32_t count = std::max<int>(1, mv.steps), i;
        for(i = 0; i < count; ++i) {
            Command one = mv;
            one.steps = 1;
            out.push_back(one);
        }
    };

    for(const auto &line : lin) {
        Command cmd = parse_line(line);
        if(cmd.type == CmdType::UNKNOWN) {
            continue             ;
        }

        if(cmd.type == CmdType::MOVE   ) {
            emit_single_move(cmd);
        } else {
            out.push_back(cmd);
        }
    }


    return out;
}


bool Creator::validate_program(const QStringList &lin)
{
    bool au, hl, hp, ok;
    QString s;
    int32_t n;
    for(const auto &raw : lin) {
                      s = raw.trimmed();
        if(s.isEmpty()) {
            continue;
        }

        if(s.startsWith("вправо", Qt::CaseInsensitive) ||
            s.startsWith("влево",  Qt::CaseInsensitive) ||
            s.startsWith("вверх",  Qt::CaseInsensitive) ||
            s.startsWith("вниз",   Qt::CaseInsensitive)) {
            au = true;
            hl = false;
            for(const QChar &ch : std::as_const(s)) {
                if(ch.isLower()) {
                    hl = true; au = false;
                }
            }

            hp = s.contains("(") || s.contains(")");
            if(au && !hl && hp) {
                show_missing_window("Ошибка: для операторов в верхнем регистре скобки запрещены");


                return false;
            }

            QRegularExpression re(R"((?:вправо|влево|вверх|вниз)\s*\((?<num>\d+)\))",
                                  QRegularExpression::CaseInsensitiveOption        );
            auto m = re.match(s);
            if(hp && !m.hasMatch()) {
                show_missing_window("Ошибка: внутри скобок должно быть число от 1 до 8");


                return false;
            }

            if(m.hasMatch()) {
                ok = false;
                n = m.captured("num").toInt(&ok);
                if(!ok || n < 1 || n > 8) {
                    show_missing_window("Ошибка: число после оператора\nдвижения должно быть от 1 до 8");


                    return false;
                }
            }
        } else {
            continue;
        }
    }


    return true;
}


void Creator::executeNextCommand()
{
    if(     game_ended || plan_index >= plan.size()) {
        com_timer->stop();
        conclude_if_needed();


        return;
    }

    const Command &cmd = plan[plan_index];
    if(       cmd.type == CmdType::MOVE) {
        QPoint       p = f->find_player();
        int32_t     nx = p.x() + cmd.dx,
                    ny = p.y() + cmd.dy;
        if(nx >= 0 && nx < f->cols() && ny >= 0 && ny < f->rows()) {
            bool fr = f->move_player_step(cmd.dx, cmd.dy);
            step_made++;
            if(fr) {
                reach_flag = true;
                game_ended = true;
                com_timer->stop();
                conclude_if_needed();


                return;
            }
        } else {
            step_made++;
        }
    } else if(cmd.type == CmdType::PAINT) {
        QPoint      np = f->find_player();
        if(np.y() < 0) {
            ++plan_index;


            return;
        }

        auto     &cell = f->access_cells()[np.y()][np.x()];
        if(    cell.ti == TYPE_ENEMY) {
               cell.ti = TYPE_FIELD;
               cell.c  = COLOR_PAINT;
            f->update();
        } else if(cmd.type == CmdType::PAINT) {
            QPoint  np = f->find_player();
            if(np.y() < 0) {
                ++plan_index;


                return;
            }

            f->paint_under_player();
            auto &cell = f->access_cells()[np.y()][np.x()];
            if(    cell.ti == TYPE_FLAG) {
                cell.ti = TYPE_FIELD;
                reach_flag = true;
                game_ended = true;
                com_timer->stop();
                conclude_if_needed();


                return;
            }
        } else if( cell.ti == TYPE_FIELD) {
                   cell.c = COLOR_PAINT;
            f->update();
        }
    }

    ++plan_index;
}


void Creator::reset_after_game()
{
    ui->Field->setEnabled(  true);
    ui->Flag->setEnabled(   true);
    ui->Player->setEnabled( true);
    ui->Secured->setEnabled(true);
    ui->Height->setEnabled( true);
    ui->Width->setEnabled(  true);
    f->set_tools_enabled(   true);
    field_load();
}


void Creator::conclude_if_needed()
{
    if(!reach_flag) {
        show_result_window(  false, false, true);
        reset_after_game();


        return;
    }

    const uint16_t ms = 275;
    if(ene_init == 0) {
        QTimer::singleShot(ms     , this, [this]() {
                show_result_window(true, false, false);
                reset_after_game();
        });
    } else if(f->count_enemies() == 0) {
        QTimer::singleShot(ms     , this, [this]() {
                show_result_window(false, true, false);
                reset_after_game();
        });
    } else {
        QTimer::singleShot(ms     , this, [this]() {
                show_result_window(false, false, false);
                reset_after_game();
        });
    }
}


void Creator::show_result_window(bool wm, bool wpa, bool lose)
{
    QString m                                                                                    ;
    if(lose) {
        m = "Проигрыш: робот не дошёл до финиша"                                                 ;
    } else if(wpa) {
        m = "Победа: вы помогли роботу добраться\nдо финиша закрасив всех врагов!!!"             ;
    } else if(wm) {
        m = "Победа: вы помогли роботу добраться до\nфиниша за минимальное количество шагов!!!"  ;
    } else {
        m = "Победа: основное задание не выполнено,\nно вы помогли роботу добраться до финиша...";
    }

    show_missing_window(m)                                                                       ;
}


QString Creator::check_missing_elements() const
{
    int32_t y, x, act = 0, flags = 0;
    for(    y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const auto &cell = f->get_cells()[y][x];
            if(cell.c != COLOR_EMPTY) {
                act++;
            }

            if(cell.ti == TYPE_FLAG) {
                flags++;
            }
        }
    }

    QPoint pp = f->find_player();
    bool hp = (pp.x() >= 0 && pp.y() >= 0);
    QStringList m;
    if(act < 2) {
        m << "На поле нет флага и игрока";
    } else if(!hp) {
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
    dir.cdUp();
    dir.mkpath("source/saves/lvls");


    return dir.filePath("source/saves/lvls/sample.dat");
}


void Creator::field_save()
{
    QString pat = level_save_path();
    QFile fil(pat);
    if(!fil.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:\n" << pat;


        return;
    }

    QDataStream out(&fil);
    out << f->rows() << f->cols();
    int32_t y, x;
    for(    y = 0; y < f->rows(); ++y) {
        for(x = 0; x < f->cols(); ++x) {
            const GameFieldWidget::Cell &cell = f->get_cells()[y][x];
            out << cell.ti << cell.c;
        }
    }

    QPoint pp = f->get_player_pos();
    out << pp.x() << pp.y();
}


void Creator::field_load()
{
    QString pat = level_save_path();
    QFile   fil(pat);
    if(!fil.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения:\n" << pat;


        return;
    }

    QDataStream in(&fil);
    int32_t rows, cols, y, x, px, py;
    in >> rows >> cols;
    f->set_size(rows, cols, false);
    for(    y = 0; y < rows; ++y) {
        for(x = 0; x < cols; ++x) {
            GameFieldWidget::Cell cell;
            in >> cell.ti >> cell.c;
            f->set_cell(y, x, cell);
        }
    }

    if(in.status() == QDataStream::Ok && !in.atEnd()) {
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

        f->set_size(h, ui->Width->value(), true);
    });

    connect(ui->Width , QOverload<int32_t>::of(&RPTESpinBox::valueChanged) , this, [=](uint16_t w)
    {
        if(ui->Height->value() == 1 && w == 1) {
            ui->Height->setValue(2);
        }

        f->set_size(ui->Height->value(), w, true);
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
