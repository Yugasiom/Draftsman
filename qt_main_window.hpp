#ifndef QT_MAIN_WINDOW_HPP

#define QT_MAIN_WINDOW_HPP


#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class Draftsman;
}

QT_END_NAMESPACE


class QtMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        QtMainWindow(QWidget *parent = nullptr);
        ~QtMainWindow();

    private:
        Ui::Draftsman *ui;
};


#endif  // QT_MAIN_WINDOW_HPP
