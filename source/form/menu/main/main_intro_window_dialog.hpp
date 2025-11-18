#ifndef MAIN_INTRO_WINDOW_DIALOG_HPP


#define MAIN_INTRO_WINDOW_DIALOG_HPP



#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QSizePolicy>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <QApplication>
#include <QRadioButton>
#include <QProgressBar>
#include <QStackedWidget>
#include <QRegularExpression>



class QLineEdit                                     ;
class QPushButton                                   ;
class QProgressBar                                  ;
class QRadioButton                                  ;
class QStackedWidget                                ;



struct LevelTheme
{
             QColor          color_field            ;
             QColor          color_empty            ;
             QColor          color_paint            ;
};



class MIWD : public QDialog
{
    Q_OBJECT


    private:
             QLineEdit      *nedit                  ;
             QPushButton    *nbtn                   ;
             QProgressBar   *progress               ;
             QRadioButton   *pbtn     ,        *fbtn;
             QStackedWidget *stack                  ;
             LevelTheme      THEME                  ;
             QString         sp       ,           op;
             bool            oe            =   false;


    private slots:
             void goNext();


    public:
        explicit             MIWD(QWidget* =nullptr);
             QString         level_name()      const;
        enum LocationType   {PROTOTYPE, FOREST     };
             LocationType    location()        const;

             LevelTheme theme()                const
             {
                 return      THEME                  ;
             }

             QString selected_path() const
             {
                 return oe ? op : sp                ;
             }

             bool is_open_existing() const
             {
                 return oe;
             }
}                                                   ;



#endif  // MAIN_INTRO_WINDOW_DIALOG_HPP
