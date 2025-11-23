#include <queue>
#include <algorithm>
#include <unordered_map>

#include "game_field_widget.hpp"



const int16_t GameFieldWidget::max_r, GameFieldWidget::max_c;



namespace std
{
    template<> struct hash<QPoint>
    {
        std::size_t operator()(const QPoint& p) const noexcept
        {
            return qHash(p)                                 ;
        }
    }                                                       ;
}



GameFieldWidget::GameFieldWidget(QWidget *p) : QWidget(p)
{
    cells.assign(max_r, std::vector<Cell>(max_c))           ;
    setMouseTracking(true)                                  ;
    set_size(r, c)                                          ;
    flag_pix.load(":/icon/source/icon/flag.png")            ;
    sel_pix.load(":/icon/source/icon/shape.png")            ;
    cs  = new QMediaPlayer(this)                            ;
    csa = new QAudioOutput(this)                            ;
    cs->setAudioOutput(csa)                                 ;
    cs->setSource(QUrl("qrc:/sound/source/sound/click.wav"));
    ms  = new QMediaPlayer(this)                            ;
    msa = new QAudioOutput(this)                            ;
    ms->setAudioOutput(msa)                                 ;
    ms->setSource(QUrl("qrc:/sound/source/sound/move.wav")) ;
    csa->setVolume(1.0)                                     ;
    msa->setVolume(1.0)                                     ;
}

void GameFieldWidget::set_size(int16_t nr, int16_t nc, bool reset)
{
    r = qBound(int16_t(1), nr, max_r);
    c = qBound(int16_t(1), nc, max_c);
    Q_ASSERT(r <= max_r && c <= max_c);
    if(r == 1 && c == 1) {
        r = 1;
        c = 2;
    }

    if(reset) {
        cells.assign(max_r, std::vector<Cell>(max_c));
        int32_t y, x;
        for(    y = 0; y < max_r; ++y) {
            for(x = 0; x < max_c; ++x) {
                cells[y][x].ti  = TYPE_NONE;
                cells[y][x].c   = COLOR_EMPTY;
            }
        }

        fcp = false;
        h   = QPoint(-1,-1);
        lc  = QPoint(-1,-1);
        p_p = QPoint(-1,-1);
    }

    int16_t cs = std::min(width() / c, height() / r);
    float hp = 0.75;
    sca_flag = flag_pix.scaled(cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    sca_sel  = sel_pix.scaled(cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}

void GameFieldWidget::set_current_tool(int32_t ti)
{
    ct = ti;
    update();
}

void GameFieldWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    int16_t cs = std::min(width() / c, height()     / r);
    float hp = 0.75;
    sca_flag = flag_pix.scaled(cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    sca_sel  = sel_pix.scaled( cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

bool GameFieldWidget::has_active_cells() const
{
    int16_t yy, xx;
    for(    yy = 0; yy < r; ++yy) {
        for(xx = 0; xx < c; ++xx) {
            if(cells[yy][xx].c != COLOR_EMPTY) {
                return true;
            }
        }
    }


    return false;
}

bool GameFieldWidget::is_interactive_cell(uint16_t tool, const Cell &cell) const
{
    switch(tool) {
        case TOOL_FIELD:
            return (cell.c == COLOR_EMPTY || cell.ti == TYPE_FIELD);
        case TOOL_FLAG:
            return (cell.c != COLOR_EMPTY);
        case TOOL_PLAYER:
            return (cell.ti == TYPE_FIELD || cell.ti == TYPE_FLAG || cell.ti == TYPE_ENEMY);
        default:
            return false;
    }
}

void GameFieldWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    int16_t    y , x,
              cs = std::min(width() / c, height() / r),
              ox = (width()  - cs * c) / 2,
              oy = (height() - cs * r) / 2;
    uint16_t  rad, bt = 4;
    QRect     ou(ox, oy, cs * c, cs * r),
              ad = ou.adjusted(bt / 2, bt / 2, -bt / 2, -bt / 2);
    bool allo    = true;
    p.setClipRect(ad);
    for(    y = 0; y < r; ++y) {
        for(x = 0; x < c; ++x) {
            QRect re(ox + x * cs, oy + y * cs, cs, cs);
            const Cell &ce = cells[y][x];
            QColor f = ce.c.isValid() ? ce.c : COLOR_FIELD;
            if(ce.ti == TYPE_FLAG && ce.c == COLOR_FIELD) {
                p.fillRect(re, ce.c);
                if(t_e && ce.c != COLOR_EMPTY) {
                    if(ct == TOOL_PLAYER) {
                        allo = can_place_player(x, y);
                    } else if(ct == TOOL_FLAG) {
                        bool reach = is_connected_after_addition(x,y,TYPE_FLAG);
                        allo = true;
                        if(!reach) {
                            p.setBrush(COLOR_FIELD.darker(110));
                        }
                    } else if(ct == TOOL_FIELD && ce.ti == TYPE_FIELD) {
                        allo = is_connected_after_addition(x,y,TYPE_ENEMY);
                    } else {
                        allo = true;
                    }

                    if(allo) {
                        p.save();
                        p.setBrush(COLOR_FIELD.darker(110));
                        p.setPen(Qt::NoPen);
                        p.drawRoundedRect(re, 18, 18);
                        p.restore();
                    }
                }


                QPoint cent = re.center() - QPoint(sca_flag.width() / 2, sca_flag.height() / 2);
                QRect tarr(cent, sca_flag.size());
                p.drawPixmap(tarr, sca_flag);
            } else if(ce.ti == TYPE_ENEMY) {
                p.fillRect(re, COLOR_FIELD);
                if(t_e && ce.c != COLOR_EMPTY) {
                    if(ct == TOOL_PLAYER) {
                        allo = true;
                    } else {
                        allo = true;
                        if(ct == TOOL_FLAG) {
                            allo = is_connected_after_addition(x, y, TYPE_FLAG);
                        } else if(ct == TOOL_FIELD && ce.ti == TYPE_FIELD) {
                            allo = is_connected_after_addition(x, y, TYPE_ENEMY);
                        }
                    }

                    if(allo) {
                        p.save();
                        p.setBrush(COLOR_FIELD.darker(110));
                        p.setPen(Qt::NoPen);
                        p.drawRoundedRect(re, 18, 18);
                        p.restore();
                    }
                }


                QPoint cent = re.center() - QPoint(sca_sel.width() / 2, sca_sel.height() / 2);
                QRect tarr(cent, sca_sel.size());
                p.drawPixmap(tarr, sca_sel);
            } else {
                p.fillRect(re, f);
                if(t_e && ce.c != COLOR_EMPTY) {
                    if(ct == TOOL_PLAYER) {
                        allo = can_place_player(x, y);
                    } else {
                        allo = true;
                        if(ct == TOOL_FLAG) {
                            allo = is_connected_after_addition(x, y, TYPE_FLAG);
                        } else if(ct == TOOL_FIELD && ce.ti == TYPE_FIELD) {
                            allo = is_connected_after_addition(x, y, TYPE_ENEMY);
                        }
                    }

                    if(allo) {
                        p.save();
                        p.setBrush(COLOR_FIELD.darker(110));
                        p.setPen(Qt::NoPen);
                        p.drawRoundedRect(re, 18, 18);
                        p.restore();
                    }
                }
            }

            if(t_e) {
                if(ct == TOOL_FIELD && ce.c == COLOR_EMPTY) {
                    bool nei = false;
                    if((  y   >   0   && cells[y - 1][  x  ].c != COLOR_EMPTY)  ||
                       (  y   < r - 1 && cells[y + 1][  x  ].c != COLOR_EMPTY)  ||
                       (  x   >   0   && cells[  y  ][x - 1].c != COLOR_EMPTY)  ||
                       (  x   < c - 1 && cells[  y  ][x + 1].c != COLOR_EMPTY)) {
                         nei = true;
                    }

                    if(!fcp || nei) {
                        p.setBrush(COLOR_FIELD.darker(130));
                        p.setPen(Qt::NoPen);
                        p.drawRoundedRect(re, 18, 18);
                    }
                }

                if(ce.ti == TYPE_ENEMY && lc == QPoint(x, y) && QPoint(x, y) != p_p) {
                    QRectF in = re.adjusted(cs / 4, cs / 4, -cs / 4, -cs / 4);
                    rad = cs / 5;
                    p.save();
                    QColor b = COLOR_PLAYER.darker(130);
                    p.setPen(QPen(b, 5));
                    p.setBrush(COLOR_PLAYER);
                    p.drawRoundedRect(in, rad, rad);
                    p.restore();
                }
            }
        }
    }

    if(p_p.y() >= 0 && p_p.x() >= 0) {
        QRect re(ox + p_p.x() * cs, oy + p_p.y() * cs, cs, cs);
        QRectF in = re.adjusted(cs / 4, cs / 4, -cs / 4, -cs / 4);
        rad = cs / 5;
        p.save();
        QColor b = COLOR_PLAYER.darker(130);
        p.setPen(QPen(b, 5));
        p.setBrush(COLOR_PLAYER);
        p.drawRoundedRect(in, rad, rad);
        p.restore();
    }

    if(t_e) {
        if(h.y() >= 0 && h.x() >= 0 && !md) {
            const Cell &ce = cells[h.y()][h.x()];
            QRect       re(ox + h.x() * cs, oy + h.y() * cs, cs, cs);
            bool        high = false;
            if(ct == TOOL_FLAG) {
                if(ce.c != COLOR_EMPTY) {
                        high = true;
                }
            } else if(ct == TOOL_FIELD) {
                if(!fcp) {
                         high = (ce.c == COLOR_EMPTY);
                } else if(ce.c == COLOR_EMPTY) {
                    bool nei = false;
                    if( (h.y() >   0   && cells[h.y() - 1][  h.x()  ].c != COLOR_EMPTY)  ||
                        (h.y() < r - 1 && cells[h.y() + 1][  h.x()  ].c != COLOR_EMPTY)  ||
                        (h.x() >   0   && cells[  h.y()  ][h.x() - 1].c != COLOR_EMPTY)  ||
                        (h.x() < c - 1 && cells[  h.y()  ][h.x() + 1].c != COLOR_EMPTY)) {
                         nei = true;
                    }

                         high = nei;
                } else if(ce.ti == TYPE_FIELD) {
                         high = is_connected_after_addition(h.x(), h.y(), TYPE_ENEMY);
                } else if(ce.ti == TYPE_FLAG) {
                         high = true;
                } else if(ce.ti == TYPE_ENEMY) {
                         high = true;
                } else {
                         high = false;
                }
            } else if(ct == TOOL_PLAYER) {
                         high = (ce.c != COLOR_EMPTY) && can_place_player(h.x(), h.y());
            }

            if(high) {
                         p.setBrush(COLOR_HOVER);
                         p.setPen(Qt::NoPen);
                         p.drawRoundedRect(re, 9, 9);
            }
        }
    }

    p.setClipping(false);
    p.setPen(QPen(COLOR_BORDER, bt));
    p.setBrush(Qt::NoBrush);
    rad = 5;
    p.drawRoundedRect(ad, rad, rad);
}

void GameFieldWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint pos = e->pos();
    int16_t cs = std::min(width() /  c, height() / r), x, y,
            ox = (        width() - cs  * c   ) / 2 ,
            oy = (       height() - cs  * r   ) / 2 ;
    QRect fieldr(ox, oy,       cs *  c, cs * r);
    if(  !fieldr.contains(pos)) {
        return;
    }

    x          = (pos.x() - ox)   / cs;
    y          = (pos.y() - oy)   / cs;
    if(x >= 0 && x < c && y >= 0 && y < r) {
        QRect cellRect(ox + x * cs, oy + y * cs, cs, cs);
        if(cellRect.contains(pos)) {
            if(h != QPoint(x, y)) {
                h = QPoint(x, y);
                if(md) {
                    handle_click(x, y);
                }

                update();
            }


            return;
        }
    }

    h = QPoint(-1, -1);
    update();
}

void GameFieldWidget::mousePressEvent(QMouseEvent *e)
{
    QPoint pos  = e->pos();
    int16_t cs  = std::min(width()    / c, height() / r), x, y,
            ox  = (width()  - cs * c) / 2,
            oy  = (height() - cs * r) / 2;
    QRect fieldr(ox, oy, cs * c, cs * r);
    if(  !fieldr.contains(pos)) {
        return;
    }

    x           = (pos.x()  - ox)     / cs;
    y           = (pos.y()  - oy)     / cs;
    if(x >= 0 && x < c && y >= 0 && y < r) {
            md  = 1;
        handle_click(x, y);
        update();
    }
}

void GameFieldWidget::leaveEvent(QEvent*)
{
    if(h != QPoint(-1, -1)) {
        h = QPoint(-1, -1);
        update();
    }
}

void GameFieldWidget::mouseReleaseEvent(QMouseEvent *)
{
    md = 0;
    bool ko =
         lc.y() >= 0 && lc.x() >= 0 &&
         lc.y() <  r && lc.x() <  c &&
         cells[lc.y()][lc.x()].ti   == TYPE_ENEMY;
    if(!ko) {
        lc = {-1, -1};
    }
}

void GameFieldWidget::handle_click(int16_t x, int16_t y)
{
    if(!t_e) {
        return;
    }

    play_click();
    Cell &cell = cells[y][x];
    if(ct == TOOL_FIELD) {
        if(p_p == QPoint(x, y)) {
            if(cell.ti == TYPE_ENEMY) {
                cell.ti = TYPE_FIELD;
                cell.c  = COLOR_FIELD;
                lc = QPoint(-1, -1);
                update();
            } else if(cell.ti == TYPE_FIELD) {
                if(!is_connected_after_addition(x,y,TYPE_ENEMY)) {
                    return;
                }

                cell.ti = TYPE_ENEMY;
                lc = QPoint(-1, -1);
                update();
            }


            return;
        }

        if(cell.ti == TYPE_ENEMY) {
            if(!is_connected_after_removal(x, y)) {
                cell.ti = TYPE_FIELD;
                cell.c  = COLOR_FIELD;
                if(lc == QPoint(x, y)) {
                    lc = {-1, -1};
                }

                update();


                return;
            }

            cell.ti = TYPE_NONE;
            cell.c  = COLOR_EMPTY;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }

            if(!has_active_cells()) {
                fcp = false;
            }

            update();


            return;
        }

        if(cell.ti == TYPE_FLAG) {
            cell.ti = TYPE_FIELD;


            return;
        }

        if(cell.c == COLOR_EMPTY) {
            if(!fcp) {
                if(!is_connected_after_addition(x,y,TYPE_FIELD)) {
                    return;
                }

                cell.c  = COLOR_FIELD;
                cell.ti = TYPE_FIELD;
                fcp = true;


                return;
            }

            bool hn = false;
            if(  (y > 0   && cells[y-1][x].c != COLOR_EMPTY) ||
                 (y < r-1 && cells[y+1][x].c != COLOR_EMPTY) ||
                 (x > 0   && cells[y][x-1].c != COLOR_EMPTY) ||
                 (x < c-1 && cells[y][x+1].c != COLOR_EMPTY)) {
                  hn = true;
            }

            if(hn) {
                if(!is_connected_after_addition(x,y,TYPE_FIELD)) {
                    return;
                }

                cell.c  = COLOR_FIELD;
                cell.ti = TYPE_FIELD;
            }


            return;
        }

        if(cell.ti == TYPE_FIELD) {
            if(!is_connected_after_addition(x,y,TYPE_ENEMY)) {
                return;
            }

            cell.ti = TYPE_ENEMY;


            return;
        }


        return;
    }

    if(ct == TOOL_FLAG) {
        if(cell.c == COLOR_EMPTY) {
            return;
        }

        if(cell.ti == TYPE_FLAG) {
            cell.ti = TYPE_FIELD;
            cell.c  = COLOR_FIELD;
            if(lc == QPoint(x, y)) {
                lc = QPoint(-1, -1);
            }

            update();


            return;
        }

        if(p_p == QPoint(x, y)) {
            p_p = QPoint(-1, -1);
            cell.ti = TYPE_FLAG;
            lc = {-1, -1};
            update();


            return;
        }

        if(cell.ti == TYPE_ENEMY && lc == QPoint(x, y)) {
            lc = {-1, -1};
            cell.ti = TYPE_FLAG;


            return;
        }

        if(cell.ti == TYPE_ENEMY || cell.ti == TYPE_FIELD) {
            if(!is_connected_after_addition(x,y,TYPE_FLAG)) {
                return;
            }

            cell.ti = TYPE_FLAG;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};


                return;
            }
        }


        return;
    }

    if(ct == TOOL_PLAYER) {
        if(!t_e) {
            return;
        }

        if(cell.c == COLOR_EMPTY) {
            return;
        }

        if(p_p == QPoint(x, y)) {
            p_p = QPoint(-1, -1);
            update();


            return;
        }

        if(cell.ti == TYPE_ENEMY || cell.ti == TYPE_FIELD) {
            if(!can_place_player(x, y)) {
                return;
            }

            p_p = QPoint(x, y);
            lc  = QPoint(-1, -1);
            update();


            return;
        }

        if(cell.ti == TYPE_FLAG) {
            if(!can_place_player(x, y)) {
                return;
            }

            cell.ti = TYPE_FIELD;
            p_p = QPoint(x, y);
            lc  = QPoint(-1, -1);
            update();


            return;
        }


        return;
    }
}

bool GameFieldWidget::can_place_player(int16_t px, int16_t py)
{
    if(px < 0 || px >= c || py < 0 || py >= r) {
        return false;
    }

    if(cells[py][px].c == COLOR_EMPTY) {
        return false;
    }

    auto passa = [&](int x, int y)->bool {
        const Cell &cc = cells[y][x];
        if(cc.c == COLOR_EMPTY) {
            return false;
        }

        if(cc.ti == TYPE_FLAG)  {
            return false;
        }


        return true;
    };

    std::vector<std::vector<bool>> visit(r, std::vector<bool>(c, false));
    std::queue<QPoint> q;
    QPoint sta(px, py), cur;
    q.push(sta);
    visit[py][px] = true;
    static const int32_t dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    int32_t i, nx, ny, y, x;
    while(!q.empty()) {
        cur = q.front(); q.pop();
        for(            i = 0; i < 4; ++i) {
                        nx = cur.x() + dx[i];
                        ny = cur.y() + dy[i];
            if(         nx >= 0 && nx < c && ny >= 0 && ny < r) {
                if(!visit[ny][nx] && passa(nx, ny)) {
                    visit[ny][nx] = true;
                    q.push(QPoint(nx, ny));
                }
            }
        }
    }

    for(                y = 0; y < r; ++y) {
        for(            x = 0; x < c; ++x) {
            if(cells[y][x].ti == TYPE_ENEMY) {
                if(!visit[y][x]) {
                    return false;
                }
            }
        }
    }

    for(                y = 0; y < r; ++y) {
        for(            x = 0; x < c; ++x) {
            if(cells[y][x].ti == TYPE_FLAG) {
                bool reach = false;
                for(    i = 0; i < 4; ++i) {
                        nx = x + dx[i], ny = y + dy[i];
                    if( nx >= 0 && nx < c && ny >= 0 && ny < r) {
                        if(visit[ny][nx]) {
                            reach = true;


                            break;
                        }
                    }
                }

                if(!reach) {
                    return false;
                }
            }
        }
    }


    return true;
}

bool GameFieldWidget::is_connected_after_removal(int16_t rx, int16_t ry)
{
    if(p_p == QPoint(rx, ry)) {
        return false;
    }

    QColor    old_c  = cells[ry][rx].c;
    int32_t   old_ti = cells[ry][rx].ti, i, nx, ny, y, x;
    cells[ry][rx].c  = COLOR_EMPTY;
    cells[ry][rx].ti = TYPE_NONE;
    QPoint       sta = find_player();
    if( sta == QPoint(-1, -1)) {
        cells[ry][rx].c  = old_c;
        cells[ry][rx].ti = old_ti;


        return true;
    }

    auto passa = [&](int x, int y)->bool {
        const Cell &cc = cells[y][x];
        if(cc.c == COLOR_EMPTY) {
            return false;
        }

        if(cc.ti == TYPE_FLAG)  {
            return false;
        }


        return true;
    };

    std::vector<std::vector<bool>> visit(r, std::vector<bool>(c, false));
    std::queue<QPoint> q;
    q.push(sta);
    visit[sta.y()][sta.x()] = true;
    static const int32_t dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    while(!q.empty()) {
        QPoint cur = q.front(); q.pop();
        for(           i = 0; i < 4       ; ++i) {
            nx = cur.x() + dx[i];
            ny = cur.y() + dy[i];
            if(nx >= 0 && nx < c && ny >= 0 && ny < r) {
                if(!visit[ny][nx] && passa(nx, ny)) {
                    visit[ny][nx] = true;
                    q.push(QPoint(nx, ny));
                }
            }
        }
    }

    bool ok = true;
    for(                y = 0; y < r && ok; ++y) {
        for(            x = 0; x < c      ; ++x) {
            if(cells[y][x].ti == TYPE_ENEMY) {
                if(x == rx && y == ry) {
                    continue;
                }

                if(!visit[y][x]) {
                    ok = false; break;
                }
            }
        }
    }

    if(ok) {
        for(            y = 0; y < r && ok; ++y) {
            for(        x = 0; x < c      ; ++x) {
                if(cells[y][x].ti == TYPE_FLAG)   {
                    bool reach = false;
                    for(i = 0; i < 4       ; ++i) {
                        nx = x + dx[i], ny = y + dy[i];
                        if(nx >= 0 && nx < c && ny >= 0 && ny < r) {
                            if(visit[ny][nx]) {
                                reach = true;


                                break;
                            }
                        }
                    }

                    if(!reach) {
                        ok = false;


                        break;
                    }
                }
            }
        }
    }

    cells[ry][rx].c  = old_c;
    cells[ry][rx].ti = old_ti;


    return ok;
}

bool GameFieldWidget::is_connected_after_addition(int16_t ax, int16_t ay, int32_t nt)
{
    QColor    old_c  = cells[ay][ax].c;
    int32_t   old_ti = cells[ay][ax].ti, y, x, i, nx, ny, k, sx, sy;
    cells[ay][ax].ti = nt;
    cells[ay][ax].c  = COLOR_FIELD;
    QPoint       sta = find_player(), cur;
    bool          ok = true, reach = false;
    if(sta == QPoint(-1, -1)) {
        for(    y = 0; y < r && sta == QPoint(-1,-1); ++y) {
            for(x = 0; x < c; ++x) {
                if(cells[y][x].c != COLOR_EMPTY) {
                    sta = QPoint(x,y);


                    break;
                }
            }

            if( sta == QPoint(-1,-1)) {
                cells[ay][ax].c  = old_c;
                cells[ay][ax].ti = old_ti;


                return true;
            }
        }
    }

    QVector<QPoint> enem;
    for(        y = 0; y < r; ++y)
        for(    x = 0; x < c; ++x)
            if( cells[y][x].ti == TYPE_ENEMY) {
                enem.push_back(QPoint(x,y));
            }

    if(enem.isEmpty()) {
        if(nt != TYPE_FLAG) {
            cells[ay][ax].c  = old_c;
            cells[ay][ax].ti = old_ti;


            return true;
        }
    }

    auto passa = [&](int32_t x, int32_t y)->bool {
        const Cell &cc = cells[y][x];
        if(cc.c == COLOR_EMPTY) {
            return false;
        }

        if(cc.ti == TYPE_FLAG)  {
            return false;
        }


        return true;
    };

    std::vector<std::vector<bool>> visit(r, std::vector<bool>(c,false));
    std::queue<QPoint> q;
    q.push(sta);
    visit[sta.y()][sta.x()] = true;
    while(!q.empty()) {
        cur = q.front(); q.pop();
        static const int dx[4]={1, -1, 0, 0}, dy[4]={0, 0, 1, -1};
        for(    i = 0; i < 4; ++i) {
                nx = cur.x()+dx[i];
                ny = cur.y()+dy[i];
            if( nx >= 0 && nx < c && ny >= 0 && ny < r) {
                if(!visit[ny][nx] && passa(nx, ny)) {
                    visit[ny][nx] = true;
                    q.push(QPoint(nx, ny));
                }
            }
        }
    }

    for(const QPoint &e : enem) {
        if(     !visit[e.y()][e.x()]) {
                ok = false;


            break;
        }
    }

    if(ok && nt == TYPE_FLAG) {
        static const int32_t ddx[4] = {1, -1, 0, 0}, ddy[4] = {0, 0, 1, -1};
        reach = false;
        for(    k = 0; k < 4; ++k) {
                sx = ax + ddx[k];
                sy = ay + ddy[k];
            if( sx >= 0 && sx < c && sy >= 0 && sy < r) {
                if(visit[sy][sx]) {
                    reach = true;


                    break;
                }
            }
        }

        if(!reach) {
            ok = false;
        }
    }

    cells[ay][ax].c  = old_c;
    cells[ay][ax].ti = old_ti;


    return ok;
}

std::vector<QPoint> GameFieldWidget::find_shortest_path(QPoint sta, QPoint goal)
{
    std::queue<QPoint> q;
    std::unordered_map<QPoint, QPoint> par;
    std::vector<std::vector<bool>> visit(r, std::vector<bool>(c, false));
    q.push(sta);
    visit[sta.y()][sta.x()] = true;
    static const int32_t dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    int32_t i, nx, ny;
    while(!q.empty()) {
        QPoint cur = q.front(); q.pop();
        if(cur == goal) {
            break;
        }

        for(i = 0; i < 4; ++i) {
            nx = cur.x() + dx[i];
            ny = cur.y() + dy[i];
            if(nx >= 0 && nx < c && ny >= 0 && ny < r) {
                if(!visit.at(ny).at(nx) && cells[ny][nx].c != COLOR_EMPTY) {
                    visit[ny][nx] = true;
                    QPoint next(nx, ny);
                    par[next] = cur;
                    q.push(next);
                }
            }
        }
    }

    std::vector<QPoint> path;
    if(!par.count(goal)) {
        return path;
    }

    for(QPoint cur = goal; cur != sta; cur = par[cur]) {
        path.push_back(cur);
    }

    path.push_back(sta);
    std::reverse(path.begin(), path.end());


    return path;
}

uint16_t GameFieldWidget::count_enemies() const
{
    int32_t cnt = 0, yy, xx;
    for(    yy = 0; yy < r; ++yy) {
        for(xx = 0; xx < c; ++xx) {
            if(cells[yy][xx].ti == TYPE_ENEMY) {
                ++cnt;
            }
        }
    }


    return cnt;
}

QPoint GameFieldWidget::find_player() const
{
    return p_p.y() >= 0 ? p_p : QPoint(-1, -1);
}

void GameFieldWidget::move_player(int32_t dx, int32_t dy)
{
    QPoint   p = find_player();
    if(p.y() < 0) {
        return;
    }

    int32_t nx = p.x() + dx, ny = p.y() + dy;
    if(nx < 0 || nx >= c || ny < 0 || ny >= r) {
        return;
    }

    if(cells[ny][nx].c == COLOR_EMPTY) {
        penalty++;


        return;
    }

    p_p = QPoint(nx, ny);
    play_move();
    update();
}

bool GameFieldWidget::move_player_step(int32_t dx, int32_t dy)
{
    QPoint   p = find_player();
    if(p.y() < 0) {
        return false;
    }

    int32_t nx = p.x() + dx, ny = p.y() + dy;
    if(nx < 0 || nx >= c || ny < 0 || ny >= r) {
        return false;
    }

    if(cells[ny][nx].c == COLOR_EMPTY) {
        penalty++;


        return false;
    }

    p_p = QPoint(nx, ny);
    play_move();
    if(cells[ny][nx].ti == TYPE_FLAG) {
        cells[ny][nx].ti = TYPE_FIELD;
        update();


        return true;
    }

    update();


    return false;
}

void GameFieldWidget::paint_under_player()
{
    QPoint   p = find_player();
    if(p.y() < 0) {
        return;
    }

    Cell &cell = cells[p.y()][p.x()];
    if(cell.c == COLOR_PAINT) {
        cell.c = COLOR_FIELD;
    } else {
        cell.c = COLOR_PAINT;
    }

    update();
}

void GameFieldWidget::remove_enemy_under_player()
{
    QPoint    p = find_player();
    if(p.y() < 0) {
        return;
    }

    Cell  &cell = cells[p.y()][p.x()];
    if(cell.ti == TYPE_ENEMY) {
        cell.ti = TYPE_FIELD;
        cell.c  = COLOR_PAINT;
        update();
    }
}

void GameFieldWidget::play_sound(QMediaPlayer *s)
{
    if(s->playbackState() == QMediaPlayer::PlayingState) {
        s->stop();
    }

    s->play();
}

void GameFieldWidget::play_click()
{
    play_sound(cs);
}

void GameFieldWidget::play_move()
{
    play_sound(ms);
}

QPoint GameFieldWidget::find_nearest_flag() const
{
    QPoint p = find_player();
    if(p == QPoint(-1, -1)) {
        return QPoint(-1, -1);
    }

    QPoint          n(-1, -1);
    int32_t         bd = std::numeric_limits<int>::max(), y, x, d;
    for(            y = 0; y < r; ++y) {
        for(        x = 0; x < c; ++x) {
            if(     cells[y][x].ti == TYPE_FLAG) {
                    d = std::abs(p.x() - x) + std::abs(p.y() - y);
                if( d < bd) {
                    bd = d;
                    n = QPoint(x, y);
                }
            }
        }
    }


    return n;
}
