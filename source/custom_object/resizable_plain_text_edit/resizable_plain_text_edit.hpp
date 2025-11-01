#ifndef RESIZABLE_PLAIN_TEXT_EDIT_HPP

#define RESIZABLE_PLAIN_TEXT_EDIT_HPP


#include <cstdint>

#include <QFont>
#include <QTimer>
#include <QPainter>
#include <QPainter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QScrollBar>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextCursor>
#include <QResizeEvent>
#include <QTextDocument>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
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

class DraftsmanHighlighter : public QSyntaxHighlighter
{
    public:
        DraftsmanHighlighter(QTextDocument *p) : QSyntaxHighlighter(p)
        {
            QTextCharFormat f;

            f.setForeground(QColor(030, 215, 000));
            rs.append({ QRegularExpression(R"(вправо|влево|вверх|вниз|красить)",
                                           QRegularExpression::CaseInsensitiveOption), f });

            f.setForeground(QColor(255, 000, 100));
            rs.append({ QRegularExpression(R"(цикл|если|то)",
                                           QRegularExpression::CaseInsensitiveOption), f });

            f.setForeground(QColor(000, 130, 255));
            rs.append({ QRegularExpression(R"(\b\d+\b)",
                                           QRegularExpression::CaseInsensitiveOption), f });

            f.setForeground(QColor(150, 150, 150));
            rs.append({ QRegularExpression(R"(:|точка)",
                                          QRegularExpression::CaseInsensitiveOption), f });

            f.setForeground(QColor(235, 220, 160));
            rs.append({ QRegularExpression(R"(\|.*\|)",
                                           QRegularExpression::CaseInsensitiveOption), f });

            defaultFormat.setForeground(Qt::white);
    }

    protected:
        void highlightBlock(const QString &text) override
        {
            setFormat(0, text.length(), defaultFormat);

            uint32_t i, s, l;
            QRegularExpressionMatchIterator it;
            QRegularExpressionMatch m;
            Rule r;

            for(i = 0; i < rs.size(); ++i) {
                r = rs.at(i);
                it = r.p.globalMatch(text);
                while(it.hasNext()) {
                    m = it.next();
                    s = m.capturedStart();
                    l = m.capturedLength();
                    setFormat(s, l, r.f);
                }
            }
        }

    private:
        struct Rule
        {
            QRegularExpression p;
            QTextCharFormat    f;
        };

        QVector<Rule> rs;
        QTextCharFormat defaultFormat;
};


#endif  // RESIZABLE_PLAIN_TEXT_EDIT_HPP
