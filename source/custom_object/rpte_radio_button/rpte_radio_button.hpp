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


    private:
            QString  ot                                               ;
            QSize    ls                                               ;

            void     apply_font_format  ()                            ;
            uint16_t calculate_font_size(                   ) const   ;

    protected:
            void     resizeEvent        (      QResizeEvent*) override;
            void     focusInEvent       (      QFocusEvent*)  override;

    public:
        explicit     RPTERadioButton    (      QWidget* =nullptr)     ;
};


#endif      // RPTE_RADIO_BUTTON_HPP
