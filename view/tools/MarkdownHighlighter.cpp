#include "MarkdownHighlighter.h"
#include <QRegularExpression>
#include <QTextEdit>

MarkdownHighlighter::MarkdownHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    if (auto* editor = qobject_cast<QTextEdit*>(parent->parent()))
    {
        setEditorStyle(editor);
    }

    boldFormat.setForeground(QColor("#88C0D0")); // Голубой
    boldFormat.setFontWeight(QFont::Bold);

    italicFormat.setForeground(QColor("#D8DEE9")); // Светло-серый
    italicFormat.setFontItalic(true);

    linkFormat.setForeground(Qt::blue);
    linkFormat.setFontUnderline(true);

    imageFormat.setForeground(Qt::darkGreen);
    imageFormat.setFontItalic(true);

    htmlTagFormat.setForeground(Qt::darkMagenta);
    htmlTagFormat.setFontFamilies({
        "Fira Code", // Предпочтительный шрифт
        "Cascadia Mono", // Альтернатива для Windows
        "JetBrains Mono", // Альтернатива для разработчиков
        "Consolas", // Стандартный моноширинный в Windows
        "Monospace", // Общий fallback
        "Courier New"
    });

    inlineColorFormat.setForeground(Qt::red); // Для текста в `!!цвет!!`

    QVector<QColor> headerColors = {
        QColor("#8FBCBB"), // h1 - бирюзовый
        QColor("#88C0D0"), // h2 - голубой
        QColor("#81A1C1"), // h3 - синий
        QColor("#5E81AC"), // h4 - темно-синий
        QColor("#4C566A"), // h5 - серо-синий
        QColor("#3B4252") // h6 - темно-серый
    };

    QVector<int> headerSizes = {22, 20, 18, 16, 14, 12};

    for (int i = 0; i < 6; ++i)
    {
        QTextCharFormat format;
        format.setForeground(headerColors[i]);
        format.setFontWeight(QFont::Bold);
        format.setFontPointSize(headerSizes[i]);
        headerFormats.append(format);
    }

    setupRules();
}


void MarkdownHighlighter::setupRules()
{
    HighlightingRule rule;

    // 1. Заголовки (H1-H6)
    for (int level = 1; level <= 6; ++level)
    {
        rule.pattern = QRegularExpression(QString("^#{%1}\\s+(.*)").arg(level));
        rule.format = headerFormats[level - 1];
        highlightingRules.append(rule);
    }

    // 2. Жирный текст (**текст** или __текст__)
    rule.pattern = QRegularExpression("(\\*\\*|__)(?=\\S)(.*?\\S)\\1");
    rule.format = boldFormat;
    highlightingRules.append(rule);

    // 3. Курсив (*текст* или _текст_)
    rule.pattern = QRegularExpression("(\\*|_)(?=\\S)(.*?\\S)\\1");
    rule.format = italicFormat;
    highlightingRules.append(rule);

    // 4. Зачеркнутый (~~текст~~)
    rule.pattern = QRegularExpression("~~(?=\\S)(.*?\\S)~~");
    QTextCharFormat strikeFormat;
    strikeFormat.setFontStrikeOut(true);
    strikeFormat.setForeground(Qt::gray);
    rule.format = strikeFormat;
    highlightingRules.append(rule);

    // 5. Встроенный код (`код`)
    rule.pattern = QRegularExpression("`([^`]+)`");
    QTextCharFormat codeFormat;
    codeFormat.setFontFamily("monospace");
    codeFormat.setForeground(Qt::darkMagenta);
    rule.format = codeFormat;
    highlightingRules.append(rule);

    // 6. Блоки кода (```код```)
    rule.pattern = QRegularExpression("```[a-zA-Z0-9]*\\n[\\s\\S]*?```");
    QTextCharFormat blockCodeFormat;
    blockCodeFormat.setBackground(QColor(240, 240, 240));
    blockCodeFormat.setFontFamily("Courier New");
    rule.format = blockCodeFormat;
    highlightingRules.append(rule);

    // 7. Цитаты (> цитата)
    rule.pattern = QRegularExpression("^> (.*)");
    QTextCharFormat quoteFormat;
    quoteFormat.setForeground(Qt::darkGreen);
    quoteFormat.setFontItalic(true);
    rule.format = quoteFormat;
    highlightingRules.append(rule);

    // 8. Списки (-, *, 1.)
    rule.pattern = QRegularExpression("^(\\-\\d+\\.)\\s+");
    QTextCharFormat listFormat;
    listFormat.setFontWeight(QFont::Bold);
    listFormat.setForeground(Qt::darkGray);
    rule.format = listFormat;
    highlightingRules.append(rule);

    // 9. Горизонтальные линии (---, ***)
    rule.pattern = QRegularExpression("^([-\\*])(?:\\s*\\1){2,}\\s*$");
    QTextCharFormat lineFormat;
    lineFormat.setForeground(Qt::darkCyan);
    lineFormat.setFontWeight(QFont::Bold);
    lineFormat.setProperty(QTextFormat::FullWidthSelection, true);
    rule.format = lineFormat;
    highlightingRules.append(rule);

    // 10. Ссылки ([текст](URL))
    rule.pattern = QRegularExpression("\\[([^\\]]+)\\]\\(([^\\)]+)\\)");
    rule.format = linkFormat;
    highlightingRules.append(rule);

    // 11. Изображения (![alt](URL))
    rule.pattern = QRegularExpression("!\\[([^\\]]+)\\]\\(([^\\)]+)\\)");
    rule.format = imageFormat;
    highlightingRules.append(rule);

    // 12. HTML-теги (<tag>)
    rule.pattern = QRegularExpression("<[^>]+>");
    rule.format = htmlTagFormat;
    highlightingRules.append(rule);

    // 13. Выделение цветом (!!красный!!)
    rule.pattern = QRegularExpression("!!([^!!]+)!!");
    rule.format = inlineColorFormat;
    highlightingRules.append(rule);
}

void MarkdownHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    QRegularExpression codeBlockStart("```[a-zA-Z0-9]*");
    QRegularExpression codeBlockEnd("```");

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(codeBlockStart);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = codeBlockEnd.match(text, startIndex);
        int endIndex = match.capturedStart();
        int codeLength = 0;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            codeLength = text.length() - startIndex;
        }
        else
        {
            codeLength = endIndex - startIndex + match.capturedLength();
        }

        QTextCharFormat format;
        format.setBackground(QColor(240, 240, 240));
        format.setFontFamilies({
            "Fira Code",
            "Cascadia Mono",
            "JetBrains Mono",
            "Consolas",
            "Monospace",
            "Courier New"
        });
        setFormat(startIndex, codeLength, format);

        startIndex = text.indexOf(codeBlockStart, startIndex + codeLength);
    }
}

void MarkdownHighlighter::setEditorStyle(QTextEdit* editor)
{
    editor->setStyleSheet(QString(
        "QWidget {"
        "   color: #E0E0E0;"
        "   selection-background-color: #3A3A3A;"
        " font-size: 18px;"
        "}"
        "QScrollBar:vertical {"
        "   background: #1D1B20;"
        "}"
    ));


    QPalette palette = editor->palette();
    palette.setColor(QPalette::Base, QColor("#1D1B20"));
    palette.setColor(QPalette::Text, QColor("#E0E0E0"));
    palette.setColor(QPalette::Highlight, QColor("#3A3A3A"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    editor->setPalette(palette);
}
