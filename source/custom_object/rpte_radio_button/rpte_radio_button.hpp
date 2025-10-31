#ifndef RPTE_RADIO_BUTTON_HPP

#define RPTE_RADIO_BUTTON_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QResizeEvent>
#include <QRadioButton>
#include <QFontDatabase>


class RPTERadioButton : public QRadioButton
{
    Q_OBJECT


    public:
        explicit     RPTERadioButton    (QWidget* =nullptr);

    protected:
            void     resizeEvent        (QResizeEvent *) override;
            void     focusInEvent       (QFocusEvent  *) override;

    private:
            void     apply_font_format  ();
            uint16_t calculate_font_size()               const;
};


#endif      // RPTE_RADIO_BUTTON_HPP
