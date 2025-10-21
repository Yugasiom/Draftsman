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

    public:
        explicit Creator(QWidget* =nullptr);
        ~Creator();

    private:
        Ui::Draftsman *ui;
};


#endif  // CREATOR_HEADER_HPP
