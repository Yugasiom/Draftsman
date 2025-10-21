#ifndef RESIZABLE_PLAIN_TEXT_EDIT_HPP

#define RESIZABLE_PLAIN_TEXT_EDIT_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QResizeEvent>
#include <QTextCharFormat>


class ResizablePlainTextEdit : public QTextEdit
{
    Q_OBJECT


    public:
        explicit ResizablePlainTextEdit(QWidget* =nullptr);

    protected:
        void     resizeEvent       (      QResizeEvent *) override;
        void     focusInEvent      (      QFocusEvent  *) override;
        void     keyPressEvent     (      QKeyEvent    *) override;
        void     insertFromMimeData(const QMimeData    *) override;


    private slots:
        void     handle_text_changed();


    private:
        void     apply_font_format();
        void     update_all_text_format(uint16_t);
        uint16_t calculate_font_size()                      const;
};


#endif  // RESIZABLE_PLAIN_TEXT_EDIT_HPP
