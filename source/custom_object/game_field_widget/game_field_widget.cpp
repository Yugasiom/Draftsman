#include "game_field_widget.hpp"



const int16_t GameFieldWidget::max_r;
const int16_t GameFieldWidget::max_c;



GameFieldWidget::GameFieldWidget(QWidget *p) : QWidget(p)
{
    setMouseTracking(true);
    set_size(r, c);
    flag_pix.load(":/icon/source/icon/flag.png");
    sel_pix.load(":/icon/source/icon/shape.png");
}

void GameFieldWidget::set_size(int16_t nr, int16_t nc)
{
    int16_t y, x, cs = std::min(width()    / c, height() / r)       ;
    float hp = 0.75;
    r = qBound(int16_t(1), nr, max_r);
    c = qBound(int16_t(1), nc, max_c);
    if(r == 1 && c == 1) {
        r = 1;
        c = 2;
    }

    for(    y = 0; y < max_r; ++y) {
        for(x = 0; x < max_c; ++x) {
            cells[y][x].ti     = 0;
            cells[y][x].c      = COLOR_EMPTY;
        }
    }

    fcp       = false;
    h         = QPoint(-1,-1);
    lc        = QPoint(-1,-1);

    sca_flag  = flag_pix.scaled(cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    sca_sel   = sel_pix.scaled( cs * hp, cs * hp, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}

void GameFieldWidget::set_current_tool(int32_t ti)
{
    ct = ti;
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
    for(yy = 0; yy < r; ++yy) {
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
    int16_t y, x , cs = std::min(width()    / c, height() / r),
                   ox = (width()  - cs * c) / 2,
                   oy = (height() - cs * r) / 2;
    uint16_t  rad, bt = 4;
    QRect          ou(ox, oy, cs * c, cs * r),
                   ad = ou.adjusted(bt / 2, bt / 2, -bt / 2, -bt / 2);
    p.setClipRect(ad);
    for(y = 0; y < r; ++y) {
        for(x = 0; x < c; ++x) {
            QRect re(ox + x * cs, oy + y * cs, cs, cs);
            const Cell &ce = cells[y][x];
            QColor f = ce.c.isValid() ? ce.c : COLOR_FIELD, b;
            if(ce.ti == 3) {
                f = COLOR_FIELD;
            }

            if(ce.ti == TYPE_FLAG   && ce.c == COLOR_FIELD) {
                p.fillRect(re, ce.c);
                QPoint cent = re.center() - QPoint(sca_flag.width()  / 2,
                                                   sca_flag.height() / 2);
                QRect tarr(cent, sca_flag.size());
                p.drawPixmap(tarr, sca_flag);
            } else if(ce.ti == 2) {
                p.fillRect(re, COLOR_FIELD);
                QPoint cent = re.center() - QPoint(sca_sel.width()   / 2,
                                                   sca_sel.height()  / 2);
                QRect tarr(cent, sca_sel.size());
                p.drawPixmap(tarr, sca_sel);
            } else {
                p.fillRect(re, f);
            }

            if(ce.c == COLOR_EMPTY) {
                bool nei = false;
                if((y >   0   && cells[y - 1][  x  ].c != COLOR_EMPTY)  ||
                   (y < r - 1 && cells[y + 1][  x  ].c != COLOR_EMPTY)  ||
                   (x >   0   && cells[  y  ][x - 1].c != COLOR_EMPTY)  ||
                   (x < c - 1 && cells[  y  ][x + 1].c != COLOR_EMPTY)) {
                    nei = true;
                }

                if(!fcp || nei) {
                    p.setBrush(COLOR_FIELD.darker(130));
                    p.setPen(Qt::NoPen);
                    p.drawRoundedRect(re, 18, 18);
                }
            }

            if(ce.ti == TYPE_PLAYER || (ce.ti == TYPE_ENEMY && lc == QPoint(x, y))) {
                QRectF in = re.adjusted(cs / 4, cs / 4, -cs / 4, -cs / 4);


                rad = cs / 5;


                p.save();

                b = COLOR_PLAYER.darker(130);
                p.setPen(QPen(b, 5));
                p.setBrush(COLOR_PLAYER);
                p.drawRoundedRect(in, rad, rad);

                p.restore();
            }

            if((ct == TOOL_FIELD || ct == TOOL_FLAG || ct == TOOL_PLAYER) && h == QPoint(x, y) && !md) {
                bool high = false;
                if(ct == TOOL_FIELD) {
                    if(!fcp) {
                        high = (ce.c == COLOR_EMPTY);
                    } else {
                        if(ce.c == COLOR_EMPTY) {
                            if((y >   0    && cells[y - 1][  x  ].c != COLOR_EMPTY)  ||
                               (y < r - 1  && cells[y + 1][  x  ].c != COLOR_EMPTY)  ||
                               (x >   0    && cells[  y  ][x - 1].c != COLOR_EMPTY)  ||
                               (x < c - 1  && cells[  y  ][x + 1].c != COLOR_EMPTY)) {
                                high = true;
                            }
                        } else {
                            high = true;
                        }
                    }
                }
                else {
                    high = is_interactive_cell(ct, ce);
                }

                if(high) {
                    p.setBrush(COLOR_HOVER);
                    p.setPen(Qt::NoPen);
                    p.drawRoundedRect(re, 9, 9);
                }
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
    QPoint pos = e->pos();
    int16_t cs  = std::min(width()    / c, height() / r), x, y,
            ox  = (width()  - cs * c) / 2,
            oy  = (height() - cs * r) / 2;
    QRect fieldr(ox, oy, cs * c, cs * r);
    if(  !fieldr.contains(pos)) {
        return;
    }

    x           = (pos.x() - ox) / cs;
    y           = (pos.y() - oy) / cs;
    if(x >= 0 && x < c && y >= 0 && y < r) {
        md = 1;
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
    Cell &cell = cells[y][x];
    int16_t xx, yy;
    if(         ct == TOOL_FIELD) {
        if(cell.ti == TYPE_ENEMY && lc == QPoint(x, y)) {
            cell.ti = TYPE_PLAYER;
            lc = {-1, -1};


            return;
        }

        if(cell.ti == TYPE_ENEMY) {
            cell.ti = TYPE_NONE;
            cell.c  = COLOR_EMPTY;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }

            if(!has_active_cells()) {
                fcp = false;
            }


            return;
        }

        if(cell.ti == TYPE_PLAYER) {
            cell.ti = TYPE_ENEMY;
            lc = QPoint(x, y);


            return;
        }

        if(cell.ti == TYPE_FLAG) {
            cell.ti = TYPE_FIELD;


            return;
        }

        if( cell.c == COLOR_EMPTY) {
            if(!fcp) {
                cell.c  = COLOR_FIELD;
                cell.ti = TYPE_FIELD;
                    fcp = true;


                return;
            }

            bool hn = false;
            if((y >   0   && cells[y - 1][  x  ].c != COLOR_EMPTY)  ||
               (y < r - 1 && cells[y + 1][  x  ].c != COLOR_EMPTY)  ||
               (x >   0   && cells[  y  ][x - 1].c != COLOR_EMPTY)  ||
               (x < c - 1 && cells[  y  ][x + 1].c != COLOR_EMPTY)) {
                hn = true;
            }

            if(hn) {
                cell.c  = COLOR_FIELD;
                cell.ti = TYPE_FIELD;
            }


            return;
        }

        if((r == 1 && c == 2) || (r == 2 && c == 1)) {
            if(cell.ti == TYPE_FIELD) {
                cell.ti = TYPE_NONE;
                cell.c  = COLOR_EMPTY;
                if(!has_active_cells()) {
                    fcp = false;
                }
            }


            return;
        }

        if(cell.ti == TYPE_FIELD) {
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
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }


            return;
        }

        if((r == 1 && c == 2) || (r == 2 && c == 1)) {
            for(yy = 0; yy < r; ++yy) {
                for(xx = 0; xx < c; ++xx) {
                    if(cells[yy][xx].ti == TYPE_FLAG) {
                        cells[yy][xx].ti = TYPE_FIELD;
                    }
                }
            }
        }

        if(cell.ti == TYPE_ENEMY && lc == QPoint(x, y)) {
            lc = {-1, -1};
            cell.ti = TYPE_FLAG;


            return;
        }

        if(cell.ti == TYPE_PLAYER) {
            cell.ti = TYPE_FLAG;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }


            return;
        }

        if(cell.ti == TYPE_ENEMY) {
            cell.ti = TYPE_FLAG;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }


            return;
        }

        if(cell.ti == TYPE_FIELD) {
            cell.ti = TYPE_FLAG;
            if(lc == QPoint(x, y)) {
                lc = {-1, -1};
            }


            return;
        }


        return;
    }

    if(ct == TOOL_PLAYER) {
        if(cell.c == COLOR_EMPTY) {
            return;
        }

        if((lc == QPoint(x, y)) && cell.ti == TYPE_ENEMY) {
            lc = {-1, -1};


            return;
        }

        if(cell.ti == TYPE_PLAYER) {
            cell.ti = TYPE_FIELD;
            lc = {-1, -1};


            return;
        }

        for(yy = 0; yy < r; ++yy) {
            for(xx = 0; xx < c; ++xx) {
                if(cells[yy][xx].ti == TYPE_PLAYER) {
                    cells[yy][xx].ti = TYPE_FIELD;
                }
            }
        }

        if(cell.ti == TYPE_FLAG) {
            cell.ti = TYPE_PLAYER;
            lc = {-1, -1};


            return;
        }

        if(cell.ti == TYPE_ENEMY) {
            lc = QPoint(x, y);


            return;
        }

        if(cell.ti == TYPE_FIELD) {
            cell.ti = TYPE_PLAYER;
            lc = {-1, -1};


            return;
        }


        return;
    }
}
