#ifndef GAME_FIELD_WIDGET_HPP


#define GAME_FIELD_WIDGET_HPP



#include <cstdint>


#include <QPoint>
#include <QColor>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>



#define WHITE         QColor(255, 255, 255)
#define GRAY          QColor(130, 130, 130)
#define DARK_GRAY     QColor(060, 060, 060)
#define ORANGE        QColor(255, 140, 000)
#define PURPLE        QColor(160, 032, 240)


#define TOOL_FIELD   1
#define TOOL_FLAG    2
#define TOOL_PLAYER  3


#define TYPE_NONE    0
#define TYPE_FIELD   1
#define TYPE_ENEMY   2
#define TYPE_PLAYER  3
#define TYPE_FLAG    4


#define COLOR_FIELD   WHITE
#define COLOR_EMPTY   GRAY

#define COLOR_BORDER  DARK_GRAY
#define COLOR_HOVER   ORANGE
#define COLOR_PLAYER  PURPLE



class GameFieldWidget : public QWidget
{
    Q_OBJECT


    private:
        static const int16_t      max_r = 9, max_c = 9                       ;



                     struct Cell
                     {
                         int32_t    ti     = 0                               ;
                         QColor     c                                        ;
                     }                                                       ;



                     QPixmap        flag_pix  , sel_pix, sca_flag, sca_sel   ;


                     Cell           cells[max_r][max_c]                      ;
                     int16_t        r      = 5, c = 5, ct = 1                ;
                     QPoint         h      {-1, -1}  , lc    {-1, -1}        ;


                     uint16_t       md     = 0                               ;

                     void           handle_click     (int16_t, int16_t)      ;

    protected:
                     void           leaveEvent       (QEvent*     )  override;
                     void           resizeEvent      (QResizeEvent*) override;
                     void           paintEvent       (QPaintEvent*)  override;
                     void           mouseMoveEvent   (QMouseEvent*)  override;
                     void           mousePressEvent  (QMouseEvent*)  override;
                     void           mouseReleaseEvent(QMouseEvent*)  override;

    public:
        explicit                    GameFieldWidget  (QWidget* =nullptr)     ;

                     void           set_size         (int16_t, int16_t)      ;
                     void           set_current_tool (int32_t)               ;
};



#endif  // GAME_FIELD_WIDGET_HPP
