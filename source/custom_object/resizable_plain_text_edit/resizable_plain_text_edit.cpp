#include "resizable_plain_text_edit.hpp"



ResizablePlainTextEdit::ResizablePlainTextEdit(QWidget *parent) :
                                               QTextEdit(parent)
{
    apply_font_format();
}


void ResizablePlainTextEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);

    QTimer::singleShot(0, this, [this]() {
        uint16_t f_s = calculate_font_size();

        QFont f = font();
        f.setPointSize(f_s);
        setFont(f);

        update_all_text_format(f_s);
    });
}

void ResizablePlainTextEdit::focusInEvent(QFocusEvent *event)
{
    QTextEdit::focusInEvent(event);
    apply_font_format();
}

void ResizablePlainTextEdit::keyPressEvent(QKeyEvent *event)
{
    apply_font_format();
    QTextEdit::keyPressEvent(event);
}

void ResizablePlainTextEdit::apply_font_format()
{
    uint16_t f_s = calculate_font_size();

    QTextCharFormat fmt;
    fmt.setFontPointSize(f_s);

    QTextCursor c = textCursor();
    c.setCharFormat(fmt);
    setTextCursor(c);

    setCurrentCharFormat(fmt);
}

void ResizablePlainTextEdit::handle_text_changed()
{
    if(toPlainText().isEmpty()) {
        apply_font_format();
    }
}

void ResizablePlainTextEdit::insertFromMimeData(const QMimeData *source)
{
    apply_font_format();
    QTextEdit::insertFromMimeData(source);
}

uint16_t ResizablePlainTextEdit::calculate_font_size() const
{
    uint16_t h     = height();
    uint16_t min_h = 475;
    uint16_t max_h = 1140;
    uint16_t min_f = 14;
    uint16_t max_f = 32;

    h = qBound(min_h, h, max_h);

    double r     = static_cast<double>(h - min_h) / (max_h - min_h);
    uint16_t f_s = min_f + static_cast<uint16_t>(r * (max_f - min_f));

    return f_s;
}

void ResizablePlainTextEdit::update_all_text_format(uint16_t nfs)
{
    QTextCursor d_c(document());
    d_c.beginEditBlock();

    QTextBlockFormat b_fmt;
    b_fmt.setLineHeight(125, QTextBlockFormat::ProportionalHeight);

    QTextBlock b = document()->firstBlock();
    QTextCharFormat fmt;
    while(b.isValid()) {
        QTextCursor b_c(b);
        fmt = b_c.charFormat();
        fmt.setFontPointSize(nfs);

        b_c.select(QTextCursor::BlockUnderCursor);
        b_c.setCharFormat(fmt);
        b_c.setBlockFormat(b_fmt);
        b = b.next();
    }

    d_c.endEditBlock();
}
