#include "game_field_widget.hpp"



const int16_t GameFieldWidget::max_r;
const int16_t GameFieldWidget::max_c;



GameFieldWidget::GameFieldWidget(QWidget *p) : QWidget(p)
{
    setMouseTracking(true);
    set_size(r, c);
}

void GameFieldWidget::set_size(int16_t nr, int16_t nc)
{
    int16_t y, x;
    r = qBound(int16_t(1), nr, max_r);
    c = qBound(int16_t(1), nc, max_c);
    if(r == 1 && c == 1) {
        r = 1;
        c = 2;
    }

    for(y = 0; y < max_r; ++y) {
        for(x = 0; x < max_c; ++x) {
            cells[y][x].ti     = 0;
            cells[y][x].c      = COLOR_EMPTY;
        }
    }

    update();
}

void GameFieldWidget::set_current_tool(int32_t ti)
{
    ct = ti;
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

            p.fillRect(re, f);
            if(ce.ti == 3) {
                QRectF in = re.adjusted(cs / 4, cs / 4, -cs / 4, -cs / 4);


                rad = cs / 5;


                p.save();

                b = COLOR_PLAYER.darker(130);
                p.setPen(QPen(b, 5));
                p.setBrush(COLOR_PLAYER);
                p.drawRoundedRect(in, rad, rad);

                p.restore();
            }

            if((ct == 1 || ct == 2 || ct == 3) && h == QPoint(x, y) && !md) {
                if(ct == 1 || ce.c == COLOR_FIELD || ce.c == COLOR_FLAG ||
                    ce.c == COLOR_ENEMY || ce.c == COLOR_PLAYER) {
                    p.fillRect(re, COLOR_HOVER);
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
    int16_t cs = std::min(width() / c, height() / r),
            ox = (        width() - cs  * c   ) / 2 ,
            oy = (       height() - cs  * r   ) / 2 ,
             x = (        pos.x() - ox) / cs,
             y = (        pos.y() - oy) / cs;
    if(x >= 0 && x < c && y >= 0 && y < r) {
        QRect cellRect(ox + x * cs, oy + y * cs, cs, cs);
        if(cellRect.contains(pos)) {
            h = QPoint(x, y);
            if(md) {
                handle_click(x, y);
            }

            update();


            return;
        }
    }

    h = QPoint(-1, -1);
    update();
}

void GameFieldWidget::mousePressEvent(QMouseEvent *e)
{
    int16_t cs  = std::min(width()    / c, height() / r),
            ox  = (width()  - cs * c) / 2,
            oy  = (height() - cs * r) / 2,
            x   = (e->pos().x() - ox) / cs,
            y   = (e->pos().y() - oy) / cs;
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
    md =   0;
    lc = {-1, -1};
}

void GameFieldWidget::handle_click(int16_t x, int16_t y)
{
    if(lc == QPoint(x, y)) {
        return;
    }

    lc = QPoint(x, y);
    Cell &cell = cells[y][x];
    int16_t yy, xx;
    if(ct != 1 && cell.c == COLOR_EMPTY) {
        return;
    }

    if(ct == 1) {
        if(cell.c == COLOR_EMPTY) {
            cell.c = COLOR_FIELD;
            cell.ti = 1;
        } else if(cell.c == COLOR_FIELD) {
            if((r == 1 && c == 2) || (r == 2 && c == 1)) {
                return;
            }

            cell.c = COLOR_ENEMY;
            cell.ti = 2;
        } else if(cell.c == COLOR_FLAG || cell.c == COLOR_PLAYER) {
            cell.c = COLOR_FIELD;
            cell.ti = 0;
        } else if(cell.c == COLOR_ENEMY) {
            cell.c = COLOR_EMPTY;
            cell.ti = 0;
        }


        return;
    }

    if(ct == 3) {
        if(cell.c == COLOR_EMPTY || cell.ti == 3) {
            return;
        }

        for(yy = 0; yy < r; ++yy) {
            for(xx = 0; xx < c; ++xx) {
                if(cells[yy][xx].ti == 3) {
                    cells[yy][xx].ti = 1;
                    cells[yy][xx].c = COLOR_FIELD;
                }
            }
        }

        cell.ti = 3;
        cell.c = COLOR_PLAYER;

        return;
    }

    if(ct == 2) {
        if(cell.c == COLOR_EMPTY || (cell.ti == 2 && cell.c == COLOR_FLAG)) {
            return;
        }

        if((r == 1 && c == 2) || (r == 2 && c == 1)) {
            for(yy = 0; yy < r; ++yy) {
                for(xx = 0; xx < c; ++xx) {
                    if(cells[yy][xx].c == COLOR_FLAG) {
                        cells[yy][xx].c = COLOR_FIELD;
                        cells[yy][xx].ti = 0;
                    }
                }
            }
        }

        cell.ti = 2;
        cell.c = COLOR_FLAG;


        return;
    }
}
