#include "rpte_spin_box.hpp"


RPTESpinBox::RPTESpinBox(QWidget *parent) :
                         QSpinBox(parent)
{
    apply_font_format();
}


void RPTESpinBox::resizeEvent(QResizeEvent *event)
{
    QSpinBox::resizeEvent(event);

    QTimer::singleShot(0, this, [this]() {
        apply_font_format();
    });
}

void RPTESpinBox::focusInEvent(QFocusEvent *event)
{
    QSpinBox::focusInEvent(event);
    apply_font_format();
}

void RPTESpinBox::apply_font_format()
{
    uint16_t f_s = calculate_font_size();

    QFont f;
    f.setPointSize(f_s);

    setFont(f);
}

uint16_t RPTESpinBox::calculate_font_size() const
{
    const uint16_t min_h = 21,
                   max_h = 60,
                   min_f = 12,
                   max_f = 20;

    uint16_t       h     = qBound(min_h, static_cast<uint16_t>(height()), max_h);
    double         r     = static_cast<double>(h - min_h) / (max_h - min_h);
    uint16_t       f_s   = min_f + static_cast<uint16_t>(r * (max_f - min_f));

    return         f_s;
}
