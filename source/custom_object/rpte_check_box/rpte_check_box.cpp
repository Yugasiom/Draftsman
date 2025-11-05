#include "rpte_check_box.hpp"



RPTECheckBox::RPTECheckBox(QWidget  *p) :
                           QCheckBox(p)
{
    apply_font_format();
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
    uint16_t f_s = calculate_font_size();

    QFont f;
    f.setPointSize(f_s);

    setFont(f);
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
    QCheckBox::showEvent(e)           ;
               setChecked(false)      ;
               setChecked(isChecked());
               apply_font_format()    ;
}
