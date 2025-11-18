#ifndef MAIN_INTRO_WINDOW_DIALOG_HPP

#define MAIN_INTRO_WINDOW_DIALOG_HPP


#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QProgressBar>
#include <QStackedWidget>


class QLineEdit                                     ;
class QPushButton                                   ;
class QProgressBar                                  ;
class QRadioButton                                  ;
class QStackedWidget                                ;


class MIWD : public QDialog
{
    Q_OBJECT

    private:
             QLineEdit      *nedit                  ;
             QPushButton    *nbtn                   ;
             QProgressBar   *progress               ;
             QRadioButton   *pbtn     ,        *fbtn;
             QStackedWidget *stack                  ;

    private slots:
             void goNext();

    public:
        explicit             MIWD(QWidget* =nullptr);
             QString         level_name()      const;
        enum LocationType   {PROTOTYPE, FOREST     };
             LocationType    location()        const;
}                                                   ;


#endif  // MAIN_INTRO_WINDOW_DIALOG_HPP
