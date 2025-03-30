#include "MarkdownEditor.h"

#include <QMenu>

MarkdownEditor::MarkdownEditor(QWidget* parent)
    : QTextEdit(parent)
{
    highlighter = new MarkdownHighlighter(document());
    setContextMenuPolicy(Qt::CustomContextMenu);
}

MarkdownEditor::~MarkdownEditor()
{
    delete highlighter;
}

void MarkdownEditor::setupContextMenu(QMenu* menu)
{
    contextMenu = menu;
    connect(this, &MarkdownEditor::customContextMenuRequested,
            [this](const QPoint& pos)
            {
                if (contextMenu) contextMenu->exec(mapToGlobal(pos));
            });
}
