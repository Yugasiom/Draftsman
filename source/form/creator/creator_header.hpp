#ifndef CREATOR_HEADER_HPP


#define CREATOR_HEADER_HPP



#include <QDir>
#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QDialog>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QStringList>
#include <QMessageBox>
#include <QDataStream>
#include <QDialogButtonBox>
#include <QCoreApplication>



#include "../menu/main/main_intro_window_dialog.hpp"



QT_BEGIN_NAMESPACE


namespace Ui
{
    class Draftsman                                                                         ;
}


QT_END_NAMESPACE



class GameFieldWidget                                                                       ;



enum class CmdType{UNKNOWN, MOVE, PAINT}                                                    ;



struct Command
{
    CmdType  type;
    QString  raw   , condition     = QString()                                              ;
    int32_t  dx = 0, dy = 0, steps = 1, loop_count = 0                                      ;
};



class Creator : public QMainWindow
{
    Q_OBJECT


    private:
        Ui::Draftsman   *ui                                                                 ;
        QTimer          *com_timer                =       nullptr                           ;
        QStringList      com                                                                ;
        int32_t          com_ci                   =             0, plan_index   = 0         ,
                         ene_init                 =             0, step_made    = 0         ;
        GameFieldWidget *f                        =       nullptr                           ;
        QVector<Command> plan                                                               ;
        bool             game_ended               =         false, reach_flag   = false     ;
        QString          ln                                      , sp                       ;
        LevelTheme       THEME                                                              ;

        Command          parse_line               (const QString&                          );
        QVector<Command> build_plan               (const QStringList&                      );
        bool             validate_before_run      (                                  ) const;
        bool             eval_condition           (const QString&                    ) const;
        void             conclude_if_needed       (                                        );
        void             show_result_window       (bool =false   , bool =false, bool =false);
        void             show_missing_window      (const QString&                          );
        void             reset_after_game         (                                        );
        int32_t          default_steps_by_case    (const QString&                    ) const;
        int32_t          parse_steps              (const QString&,           const QString&);
        QString          check_missing_elements   (                                  ) const;
        bool             validate_program         (const QStringList&                      );
        QString          level_save_path          (                                  ) const;


    private slots:
        void             on_Play_clicked          (                                        );
        void             on_Reset_clicked         (                                        );
        void             executeNextCommand       (                                        );


    public:
        explicit         Creator                  (QWidget* =nullptr                       );
                        ~Creator                  (                                        );
        void             field_save               (                                        );
        void             field_load               (                                        );
}                                                                                           ;



#endif  // CREATOR_HEADER_HPP
