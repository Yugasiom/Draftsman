#include "rpte_radio_button.hpp"



RPTERadioButton::RPTERadioButton(QWidget *p) :
                                 QRadioButton(p)
{
    apply_font_format();
}


void RPTERadioButton::resizeEvent(QResizeEvent *e)
{
    QRadioButton::resizeEvent(e);
    QTimer::singleShot(0, this, [this]() {
            apply_font_format();
    });
}

void RPTERadioButton::focusInEvent(QFocusEvent *e)
{
    QRadioButton::focusInEvent(e);
                  apply_font_format();
}

void RPTERadioButton::apply_font_format()
{
          QWidget      *p = parentWidget()                                ;
    while(p && p->size().width() < 500) {
        p = p->parentWidget()                                             ;
    }
          QSize        ws = p ? p->size() : QSize(1024, 640)              ;
          int32_t      ww = ws.width()  , wh = ws.height()                ;
          bool          c = (ww == 800 && wh == 500)                      ;
    const uint16_t  min_i = 16, max_i = 32                                ;
          int32_t      is = (wh < 540) ? min_i :
                             min_i + ((qMin(wh, 1200) - 540) * (max_i - min_i)) / (1200 - 540);
    setIconSize(QSize(is, is));
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

          uint16_t    f_s = calculate_font_size()                  ;
          bool        nsc = (ls.width() == ww && ls.height() == wh);
    if(nsc) {
        return;
    }

    ls = ws;

          QFont         f                                   ;
    f.setPointSize(f_s)                                     ;
          QFontMetrics fm(f)                                ;
          int32_t      tw = fm.horizontalAdvance(text()),
                       a = width() - iconSize().width() - 8 ;

    while(tw > a && f_s > 8) {
        --f_s                                               ;
        f.setPointSize(f_s)                                 ;
        setFont(f)                                          ;
        fm = QFontMetrics(f)                                ;
        tw = fm.horizontalAdvance(text())                   ;
    }

    if(tw > a) {
        setText("")                                         ;
    } else {
        setFont(f)                                          ;
    }
}

uint16_t RPTERadioButton::calculate_font_size() const
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
