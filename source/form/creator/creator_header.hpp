#ifndef CREATOR_HEADER_HPP


#define CREATOR_HEADER_HPP



#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QDialog>
#include <QMessageBox>
#include <QStringList>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QDialogButtonBox>



QT_BEGIN_NAMESPACE


namespace Ui
{
    class Draftsman                                                                         ;
}


QT_END_NAMESPACE



class GameFieldWidget                                                                       ;



enum class CmdType
{
    UNKNOWN,  PAINT     , MOVE                                                              ,
    BLOCK_IF, BLOCK_LOOP, BLOCK_END                                                        //
};



struct Command
{
    CmdType  type;
    QString  raw;
    int32_t  dx  = 0, dy = 0                                                                ,
             steps       = 1                                                                ,
             loop_count  = 0                                                                ;
    QString  condition                                                                      ;
};



class Creator : public QMainWindow
{
    Q_OBJECT


    private:
        Ui::Draftsman   *ui                                                                 ;

        QTimer          *com_timer                = nullptr                                 ;
        QStringList      com                                                                ;
        int32_t          com_ci                   = 0                                       ,
                         plan_index               = 0                                       ,
                         ene_init                 = 0                                       ,
                         step_made                = 0                                       ;
        GameFieldWidget *f                        = nullptr                                 ;

        QVector<Command> plan;
        bool             game_ended               = false                                   ;
        bool             reach_flag               = false                                   ;


        Command          parse_line               (const QString&                    ) const;
        QVector<Command> build_plan               (const QStringList&                ) const;
        bool             validate_before_run      (                                  ) const;
        bool             eval_condition           (const QString&                    ) const;
        void             conclude_if_needed       (                                        );
        void             show_result_window       (bool =false   , bool =false, bool =false);
        void             show_missing_window      (const QString&                          );
        int32_t          default_steps_by_case    (const QString&                    ) const;
        int32_t          parse_steps              (const QString&    , const QString&) const;
        QString          check_missing_elements   (                                  ) const;


    private slots:
            void         on_Play_clicked          (                                        );
            void         executeNextCommand       (                                        );


    public:
        explicit         Creator                  (QWidget* =nullptr                       );
                        ~Creator                  (                                        );
}                                                                                           ;



#endif  // CREATOR_HEADER_HPP
