#ifndef RESIZABLE_PLAIN_TEXT_EDIT_HPP

#define RESIZABLE_PLAIN_TEXT_EDIT_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QPainter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QScrollBar>
#include <QTextCursor>
#include <QResizeEvent>
#include <QTextCharFormat>
#include <QAbstractTextDocumentLayout>


class ResizablePlainTextEdit : public QTextEdit
{
    Q_OBJECT


    public:
        explicit ResizablePlainTextEdit(      QWidget*       = nullptr);

    protected:
        void     resizeEvent           (      QResizeEvent *)  override;
        void     focusInEvent          (      QFocusEvent  *)  override;
        void     keyPressEvent         (      QKeyEvent    *)  override;
        void     insertFromMimeData    (const QMimeData    *)  override;


    private slots:
        void     handle_text_changed   ();


    private:
        QWidget *lna                   = nullptr;



        uint16_t line_number_area_width()                      const;
        void     paint_line_number_area(QPaintEvent* );
        bool     eventFilter           (QObject *, QEvent *)   override;


        void     apply_font_format     ();
        void     update_all_text_format(uint16_t);
        uint16_t calculate_font_size   ()                      const;
};


#endif  // RESIZABLE_PLAIN_TEXT_EDIT_HPP
