#pragma once

#include <QTextEdit>
#include "MarkdownHighlighter.h"

class MarkdownEditor : public QTextEdit
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();

    void setupContextMenu(QMenu *menu);

private:
    MarkdownHighlighter *highlighter;
    QMenu *contextMenu = nullptr;
};
