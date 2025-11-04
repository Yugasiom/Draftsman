#ifndef RPTE_CHECK_BOX_HPP

#define RPTE_CHECK_BOX_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QCheckBox>
#include <QResizeEvent>
#include <QFontDatabase>


class RPTECheckBox : public QCheckBox
{
    Q_OBJECT


    private:
            QString  ot                                              ;
            QSize    ls                                              ;

            void     apply_font_format  (                 )          ;
            uint16_t calculate_font_size(                 )     const;

    protected:
            void     showEvent          (QShowEvent*     )   override;
            void     resizeEvent        (QResizeEvent*    )  override;
            void     focusInEvent       (QFocusEvent*     )  override;

    public:
        explicit     RPTECheckBox       (QWidget* =nullptr)          ;
};


#endif      // RPTE_CHECK_BOX_HPP
