#ifndef CREATOR_HEADER_HPP

#define CREATOR_HEADER_HPP


#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class Draftsman;
}

QT_END_NAMESPACE


class Creator : public QMainWindow
{
    Q_OBJECT


    private:
        Ui::Draftsman *ui;

    public:
        explicit       Creator(QWidget* =nullptr);
                      ~Creator();
};


#endif  // CREATOR_HEADER_HPP
