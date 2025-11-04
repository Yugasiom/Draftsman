#include "rpte_check_box.hpp"



RPTECheckBox::RPTECheckBox(QWidget  *p) :
                           QCheckBox(p)
{
    connect(this, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState) {
        apply_font_format();
    });
}


void RPTECheckBox::resizeEvent(QResizeEvent *e)
{
    QCheckBox::resizeEvent(e);
    QTimer::singleShot(0, this, [this]() {
            apply_font_format();
    });
}

void RPTECheckBox::focusInEvent(QFocusEvent *e)
{
    QCheckBox::focusInEvent(e);
               apply_font_format();
}

void RPTECheckBox::apply_font_format()
{
    QSize              ws = window() ? window()->size() : QSize(1024, 640);
    int32_t            ww = ws.width()  , wh = ws.height()                ;
    bool                c = (ww == 800 && wh == 500)                      ;
    ls = ws;
    int32_t            is = 32                                            ;
    if(!icon().isNull()) {
        setIconSize(QSize(is, is));
    }

    if(c) {
        if(!text().isEmpty()) {
            ot = text();
        }

        setText("");


        return;
    } else {
        if(text().isEmpty() && !ot.isEmpty()) {
            setText(ot);
        }
    }

    uint16_t          f_s = calculate_font_size()                         ;
    QFont               f                                                 ;
    f.setPointSize(f_s)                                                   ;
    QFontMetrics       fm(f)                                              ;
    int32_t            tw = fm.horizontalAdvance(text()),
                        a = width() - iconSize().width() - 8              ;

    while(tw > a && f_s > 8) {
        --f_s                                                             ;
        f.setPointSize(f_s)                                               ;
        setFont(f)                                                        ;
        fm = QFontMetrics(f)                                              ;
        tw = fm.horizontalAdvance(text())                                 ;
    }

    if(tw > a) {
        setText("")                                                       ;
    } else {
        setFont(f)                                                        ;
    }
}

uint16_t RPTECheckBox::calculate_font_size() const
{
    const uint16_t min_h = 21,
                   max_h = 60,
                   min_f =  8,
                   max_f = 20;
    uint16_t       h     = qBound(min_h, static_cast<uint16_t>(height()), max_h);
    double         r     = static_cast<double>(h - min_h) / (max_h - min_h);
    uint16_t       f_s   = min_f + static_cast<uint16_t>(r * (max_f - min_f));


    return         f_s;
}

void RPTECheckBox::showEvent(QShowEvent *e)
{
    QCheckBox::showEvent(e);

    bool was_checked = isChecked();
    setChecked(false);
    setChecked(was_checked);

    apply_font_format();
}
