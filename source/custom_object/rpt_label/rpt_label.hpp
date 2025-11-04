#ifndef RPT_LABEL_HPP

#define RPT_LABEL_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QLabel>
#include <QResizeEvent>
#include <QFontDatabase>


class RPTLabel : public QLabel
{
    Q_OBJECT


    private:
            void     apply_font_format  (                 )         ;
            uint16_t calculate_font_size(                 ) const   ;

    protected:
            void     resizeEvent        (QResizeEvent*    ) override;
            void     focusInEvent       (QFocusEvent*     ) override;

    public:
        explicit     RPTLabel           (QWidget* =nullptr)         ;
};


#endif      // RPT_LABEL_HPP
