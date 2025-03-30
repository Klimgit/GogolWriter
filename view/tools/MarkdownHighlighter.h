#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <qtextedit.h>

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit MarkdownHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;
    static void setEditorStyle(QTextEdit* editor);

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupRules();
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat linkFormat;
    QTextCharFormat imageFormat;
    QTextCharFormat htmlTagFormat;
    QTextCharFormat inlineColorFormat;
    QTextCharFormat boldFormat;
    QTextCharFormat italicFormat;
    QVector<QTextCharFormat> headerFormats;
};
