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

#define YELLOW        QColor(255, 255, 000)
#define DARK_WHITE    QColor(180, 198, 198)

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
#define COLOR_PAINT   DARK_WHITE

#define COLOR_BORDER  DARK_GRAY
#define COLOR_HOVER   ORANGE
#define COLOR_PLAYER  PURPLE



class GameFieldWidget : public QWidget
{
    Q_OBJECT



    public:
        struct Cell
        {
                     int32_t        ti         = 0                                  ;
                     QColor         c                                               ;
        }                                                                           ;

    private:
        static const int16_t        max_r  = 9, max_c = 9                           ;



                     QPixmap        flag_pix  , sel_pix, sca_flag, sca_sel          ;


                     bool           fcp    = false,   t_e = true                    ;


    std::vector<std::vector<Cell>>  cells     , init_cells                          ;
                     int16_t        r      = 5, c = 5, ct = 1, penalty      =    0  ;
                     QPoint         h{-1, -1} , lc{-1, -1}   , p_p = QPoint(-1, -1) ;


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
                         init_cells = cells                                         ;
                     }

                     void reset_state()
                     {
                         if(!init_cells.empty()) {
                             cells = init_cells                                     ;
                             this->update()                                         ;
                         }
                     }


                     void           set_size           (int16_t, int16_t, bool=true);
                     void           set_current_tool   (int32_t)                    ;
                     void           move_player        (int32_t, int32_t  )         ;
                     bool           move_player_step   (int32_t, int32_t  )         ;
                     void           paint_under_player (                  )         ;
                     void           remove_enemy_under_player  (          )         ;
                     QPoint         find_player        (                  )    const;
                     uint16_t       count_enemies      (                  )    const;


                     int32_t        rows               (                  )    const
                     {
                         return r                                                   ;
                     }

                     int32_t        cols               (                  )    const
                     {
                         return c                                                   ;
                     }

        const std::vector<std::vector<Cell>>& get_cells(                  )    const
                     {
                         return     cells                                           ;
                     }

        std::vector<std::vector<Cell>>& access_cells   (                  )
                     {
                         return     cells                                           ;
                     }

                     void           set_cell  (int32_t y, int32_t x, const Cell &cel)
                     {
                         if(y >= 0 && y < r && x >= 0 && x < c) {
                                    cells[y][x] = cel                               ;
                                    update()                                        ;
                         }
                     }

                     QPoint get_player_pos() const
                     {
                         return     p_p                                             ;
                     }

                     void set_player_pos(QPoint p)
                     {
                                    p_p = p                                         ;
                                    update()                                        ;
                     }

                     void set_tools_enabled(bool en)
                     {
                                    t_e = en                                        ;
                     }

                     bool tools_enabled() const
                     {
                         return     t_e                                             ;
                     }

                 void set_theme_colors(const QColor&, const QColor&, const QColor& );
}                                                                                   ;



#endif  // GAME_FIELD_WIDGET_HPP
