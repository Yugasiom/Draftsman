#ifndef GAME_FIELD_WIDGET_HPP


#define GAME_FIELD_WIDGET_HPP



#include <cstdint>


#include <vector>


#include <QPoint>
#include <QColor>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>



#define WHITE         QColor(200, 220, 220)
#define GRAY          QColor(130, 130, 130)
#define DARK_GRAY     QColor(060, 060, 060)
#define ORANGE        QColor(255, 140, 000)
#define PURPLE        QColor(160, 032, 240)


#define TOOL_FIELD           001
#define TOOL_FLAG            002
#define TOOL_PLAYER          003


#define TYPE_NONE            000
#define TYPE_FIELD           001
#define TYPE_ENEMY           002
#define TYPE_PLAYER          003
#define TYPE_FLAG            004


#define COLOR_FIELD   WHITE
#define COLOR_EMPTY   GRAY

#define COLOR_BORDER  DARK_GRAY
#define COLOR_HOVER   ORANGE
#define COLOR_PLAYER  PURPLE



class GameFieldWidget : public QWidget
{
    Q_OBJECT


    private:
        static const int16_t        max_r  = 9, max_c = 9                           ;



                     struct Cell
                     {
                         int32_t    ti     = 0                                      ;
                         QColor     c                                               ;
                     }                                                              ;



                     QPixmap        flag_pix  , sel_pix, sca_flag, sca_sel          ;


                     bool           fcp    = false                                  ;


    std::vector<std::vector<Cell>>  cells     , init_cells                          ;
                     int16_t        r      = 5, c = 5, ct = 1                       ;
                     QPoint         h      {-1, -1}  , lc    {-1, -1}               ;


                     uint16_t       md     = 0                                      ;

                     void           handle_click       (int16_t , int16_t    )      ;
                     bool           has_active_cells   (                     ) const;
                     bool           is_interactive_cell(uint16_t, const Cell&) const;
                     bool           is_connected_after_removal(int16_t, int16_t)    ;
                std::vector<QPoint> find_shortest_path        (QPoint , QPoint )    ;

    protected:
                     void           leaveEvent         (QEvent*           ) override;
                     void           resizeEvent        (QResizeEvent*     ) override;
                     void           paintEvent         (QPaintEvent*      ) override;
                     void           mouseMoveEvent     (QMouseEvent*      ) override;
                     void           mousePressEvent    (QMouseEvent*      ) override;
                     void           mouseReleaseEvent  (QMouseEvent*      ) override;

    public:
        explicit                    GameFieldWidget    (QWidget* =nullptr )         ;


                     void save_initial_state()
                     {
                         init_cells = cells;
                     }

                     void reset_state()
                     {
                         if(!init_cells.empty()) {
                             cells = init_cells;
                             this->update();
                         }
                     }


                     void           set_size           (int16_t, int16_t  )         ;
                     void           set_current_tool   (int32_t)                    ;
                     void           move_player        (int32_t, int32_t  )         ;
                     bool           move_player_step   (int32_t, int32_t  )         ;
                     void           paint_under_player (                  )         ;
                     QPoint         find_player        (                  )    const;
                     uint16_t       count_enemies      (                  )    const;


                     int32_t        rows               (                  )    const
                     {
                         return r;
                     }

                     int32_t        cols               (                  )    const
                     {
                         return c;
                     }

        const std::vector<std::vector<Cell>>& get_cells(                  )    const
                     {
                         return cells;
                     }

};



#endif  // GAME_FIELD_WIDGET_HPP
