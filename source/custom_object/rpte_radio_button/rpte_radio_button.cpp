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
    QSize     ws = window() ? window()->size() : QSize(1024, 640);
    int32_t   ww = ws.width(), ph = size().height();
    if(!text().isEmpty()) {
        ot = text();
    }

    uint16_t f_s = calculate_font_size();
    bool      sh = (ph < 60 || (ph == 60 && ww < 905));
    if(sh && f_s > 8) {
             f_s = qMax<uint16_t>(8, f_s - 4);
        if(!text().isEmpty()) {
            setText("");
        }
    } else if(!sh && text().isEmpty() && !ot.isEmpty()) {
        setText(ot);
    }

    QFont      f;
    f.setPointSize(f_s);
    setFont(f);
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
