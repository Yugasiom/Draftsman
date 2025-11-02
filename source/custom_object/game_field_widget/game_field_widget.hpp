#ifndef GAME_FIELD_WIDGET_HPP

#define GAME_FIELD_WIDGET_HPP


#include <cstdint>

#include <QPoint>
#include <QColor>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>


#define WHITE         QColor(255, 255, 255)
#define GRAY          QColor(130, 130, 130)
#define DARK_GRAY     QColor(060, 060, 060)
#define RED           QColor(255, 000, 000)
#define GREEN         QColor(000, 255, 000)
#define BLUE          QColor(000, 000, 255)
#define YELLOW        QColor(255, 255, 000)
#define ORANGE        QColor(255, 140, 000)
#define PURPLE        QColor(160, 032, 240)

#define COLOR_EMPTY   WHITE
#define COLOR_DEFAULT WHITE
#define COLOR_FIELD   GRAY
#define COLOR_BORDER  DARK_GRAY
#define COLOR_FLAG    RED
#define COLOR_ENEMY   YELLOW
#define COLOR_PLAYER  PURPLE
#define COLOR_HOVER   ORANGE


class GameFieldWidget : public QWidget
{
    Q_OBJECT


    private:
        static const int16_t    max_r = 9, max_c = 9;



                     struct Cell
                     {
                        int32_t ti    = 0;
                        QColor  c;
                     };


                     Cell       cells[max_r][max_c];
                     int16_t    r     = 5, c = 5, ct = 1;
                     QPoint     h     {-1, -1}  , lc    {-1, -1};


                     uint16_t   md    = 0;

                     void       handle_click     (int16_t, int16_t);

    protected:
                     void       paintEvent       (QPaintEvent *) override;
                     void       mouseMoveEvent   (QMouseEvent *) override;
                     void       mousePressEvent  (QMouseEvent *) override;
                     void       mouseReleaseEvent(QMouseEvent *) override;

    public:
        explicit                GameFieldWidget  (QWidget* =nullptr);

                     void       set_size         (int16_t, int16_t);
                     void       set_current_tool (int32_t);
};


#endif  // GAME_FIELD_WIDGET_HPP
