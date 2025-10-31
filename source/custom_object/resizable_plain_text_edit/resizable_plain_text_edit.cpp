#include "resizable_plain_text_edit.hpp"



ResizablePlainTextEdit::ResizablePlainTextEdit(QWidget *p) :
                                               QTextEdit(p)
{
    apply_font_format();



    lna = new QWidget(this);
    lna->setObjectName("LNA");

    lna->installEventFilter(this);


    connect(this, &QTextEdit::textChanged, this, [this]() {
        lna->update();
    });

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        lna->update();
    });
}


void ResizablePlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    QTimer::singleShot(0, this, [this]() {
        uint16_t f_s = calculate_font_size();

        QFont f = font();
        f.setPointSize(f_s);
        setFont(f);

        update_all_text_format(f_s);
    });


    lna->setGeometry(QRect(0, 0, line_number_area_width(), height()));
    setViewportMargins(line_number_area_width() + 5, 0, 0, 0);
}

void ResizablePlainTextEdit::focusInEvent(QFocusEvent *e)
{
    QTextEdit::focusInEvent(e);
    apply_font_format();
}

void ResizablePlainTextEdit::keyPressEvent(QKeyEvent *e)
{
    apply_font_format();
    QTextEdit::keyPressEvent(e);
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

void ResizablePlainTextEdit::insertFromMimeData(const QMimeData *s)
{
    apply_font_format();
    QTextEdit::insertFromMimeData(s);
}

uint16_t ResizablePlainTextEdit::calculate_font_size() const
{
    uint16_t h     = height();
    uint16_t min_h = 475;
    uint16_t max_h = 1140;
    uint16_t min_f = 14;
    uint16_t max_f = 22;

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

uint16_t ResizablePlainTextEdit::line_number_area_width() const
{
    return 40;
}

void ResizablePlainTextEdit::paint_line_number_area(QPaintEvent *e)
{
    QPainter p(lna);
    p.fillRect(e->rect(), QColor(46, 46, 46));

    QFont nf;
    nf.setPointSize(12);
    p.setFont(nf);

    QTextBlock b = document()->firstBlock();
    int32_t bn = 1, sy = verticalScrollBar()->value();
    QRectF r;
    QPointF pos;
    QString n;

    while(b.isValid()) {
        r = document()->documentLayout()->blockBoundingRect(b);
        pos = r.topLeft() - QPointF(0, sy);

        if(b.isVisible()) {
            n = QString::number(bn);
            p.setPen(QColor(255, 100, 0));
            p.drawText(0, static_cast<int32_t>(pos.y()), line_number_area_width() - 8,
                       fontMetrics().height(), Qt::AlignRight, n);
            ++bn;
        }

        b = b.next();
    }
}

bool ResizablePlainTextEdit::eventFilter(QObject *o, QEvent *e)
{
    if(o == lna && e->type() == QEvent::Paint) {
        paint_line_number_area(static_cast<QPaintEvent *>(e));


        return true;
    }


    return QTextEdit::eventFilter(o, e);
}
