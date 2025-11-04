#ifndef RPTE_PUSH_BUTTON_HPP

#define RPTE_PUSH_BUTTON_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QPushButton>
#include <QResizeEvent>
#include <QFontDatabase>


class RPTEPushButton : public QPushButton
{
    Q_OBJECT


    private:
            void     apply_font_format  (                 )         ;
            uint16_t calculate_font_size(                 ) const   ;

    protected:
            void     resizeEvent        (QResizeEvent*    ) override;
            void     focusInEvent       (QFocusEvent*     ) override;

    public:
        explicit     RPTEPushButton     (QWidget* =nullptr)         ;
};


#endif      // RPTE_PUSH_BUTTON_HPP
