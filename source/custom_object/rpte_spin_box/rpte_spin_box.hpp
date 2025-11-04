#ifndef RPTE_SPIN_BOX_HPP

#define RPTE_SPIN_BOX_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QSpinBox>
#include <QResizeEvent>
#include <QFontDatabase>


class RPTESpinBox : public QSpinBox
{
    Q_OBJECT


    private:
            void     apply_font_format  (                 )         ;
            uint16_t calculate_font_size(                 ) const   ;

    protected:
            void     resizeEvent        (QResizeEvent*    ) override;
            void     focusInEvent       (QFocusEvent*     ) override;

    public:
        explicit     RPTESpinBox        (QWidget* =nullptr)         ;
};


#endif      // RPTE_SPIN_BOX_HPP
