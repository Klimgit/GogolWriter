// ProjectWindow.cpp
#include "ProjectWindow.h"
#include "../../resources/styles/ProjectStyle.h"

#include <cerrno>
#include <cstring>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QShortcut>
#include <QGraphicsDropShadowEffect>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>
#include <QTimer>
// #include <QPrinter>
#include <QPdfWriter>
#include <QTextDocument>
#include <QPageSize>
#include <QPageLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QFormLayout>
#include <QTextDocument>
#include <QPdfWriter>
#include <QPageSize>
#include <QPageLayout>
#include <QFont>
#include <QMimeData>
#include <QStandardItemModel>
#include <QDataStream>
#include <QFontComboBox>

ProjectWindow::ProjectWindow(QString name, QWidget* parent)
    : QMainWindow(parent), projectName(std::move(name))
{
    setWindowTitle(projectName);
    setBaseSize(800, 600);

    projectPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + projectName;

    centralWidget = new QWidget(this);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    /*---------Tree View---------------------------*/

    treeModel = new QStandardItemModel(this);
    treeModel->setHorizontalHeaderLabels(QStringList() << "Project Structure");

    treeView = new QTreeView(centralWidget);
    treeView->setModel(treeModel);
    treeView->setHeaderHidden(true);
    treeView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    treeView->setMaximumWidth(200);
    treeView->setStyleSheet(TreeViewStyle);


    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(true);
    treeView->setDropIndicatorShown(true);
    treeView->setDragDropMode(QAbstractItemView::InternalMove);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setDefaultDropAction(Qt::MoveAction);

    /*------------------------SidePanel-------------------------*/
    sidePanel = new QWidget(centralWidget);
    sidePanelLayout = new QVBoxLayout(sidePanel);
    sidePanelLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    sidePanelLayout->setContentsMargins(0, 0, 0, 0);
    sidePanelLayout->setSpacing(10);
    sidePanel->setFixedWidth(50);

    int buttonSize = 24;
    int iconSize = 24;

    toolButtonCollapse = new QToolButton(sidePanel);
    toolButtonCollapse->setIcon(QIcon("../resources/icons/wheelchair.png"));
    toolButtonCollapse->setStyleSheet("QToolButton { background: transparent; border: none; }");
    toolButtonCollapse->setToolTip("Collapse");
    sidePanelLayout->addWidget(toolButtonCollapse);
    treeView->setVisible(true);
    toolButtonCollapse->setIcon(QIcon("../resources/icons/roll.png"));

    toolButtonCharacters = new QToolButton(sidePanel);
    toolButtonCharacters->setIcon(QIcon("../resources/icons/characters.png"));
    toolButtonCharacters->setStyleSheet("QToolButton { background: transparent; border: none; }");
    toolButtonCharacters->setToolTip("Characters");
    sidePanelLayout->addWidget(toolButtonCharacters);

    toolButtonPlot = new QToolButton(sidePanel);
    toolButtonPlot->setIcon(QIcon("../resources/icons/plot.png"));
    toolButtonPlot->setStyleSheet("QToolButton { background: transparent; border: none; }");
    toolButtonPlot->setToolTip("Plot");
    sidePanelLayout->addWidget(toolButtonPlot);

    toolButtonCalendar = new QToolButton(sidePanel);
    toolButtonCalendar->setIcon(QIcon("../resources/icons/calendar.png"));
    toolButtonCalendar->setStyleSheet("QToolButton { background: transparent; border: none; }");
    toolButtonCalendar->setToolTip("Calendar");
    sidePanelLayout->addWidget(toolButtonCalendar);

    sidePanelLayout->addStretch();

    toolButtonSettings = new QToolButton(sidePanel);
    toolButtonSettings->setIcon(QIcon("../resources/icons/settings.png"));
    toolButtonSettings->setStyleSheet("QToolButton { background: transparent; border: none; }");
    toolButtonSettings->setToolTip("Settings");
    sidePanelLayout->addWidget(toolButtonSettings);


    for (auto button : sidePanel->findChildren<QToolButton*>())
    {
        button->setIconSize(QSize(iconSize, iconSize));
    }

    sidePanel->setStyleSheet(SidePanelStyle);

    /*------------------------------------------------------------*/
    mainLayout->addWidget(sidePanel);
    topLayout = new QVBoxLayout();

    /*---------------------ToolBar------------------------*/
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(5);

    QWidget* buttonContainer = new QWidget();
    buttonContainer->setLayout(buttonLayout);
    buttonContainer->setStyleSheet(ButtonContainerStyle);


    auto createToolButton = [&](const QString& iconPath) -> QToolButton* {
        QToolButton* btn = new QToolButton(centralWidget);
        btn->setIcon(QIcon(iconPath));
        btn->setStyleSheet(ToolButtonStyle);
        btn->setIconSize(QSize(iconSize, iconSize));
        btn->setMinimumSize(buttonSize, buttonSize);
        return btn;
    };

    toolButtonAddChapter = createToolButton("../resources/icons/add-document.png");
    toolButtonAddFolder = createToolButton("../resources/icons/add-folder.png");
    toolButtonSave = createToolButton("../resources/icons/save.png");
    toolButtonCompile = createToolButton("../resources/icons/compile.png");

    buttonLayout->addWidget(toolButtonAddChapter);
    buttonLayout->addWidget(toolButtonAddFolder);
    buttonLayout->addWidget(toolButtonSave);
    buttonLayout->addWidget(toolButtonCompile);
    /*--------------------------------------------------------*/

    topLayout->addWidget(buttonContainer);
    topLayout->addWidget(treeView);

    /*------------------------TabBar-------------------------*/
    tabWidget = new QTabWidget(centralWidget);
    QWidget* tab1 = createMarkdownEditor();
    QVBoxLayout* tab1Layout;
    tab1Layout = new QVBoxLayout(tab1);
    tabWidget->setDocumentMode(true);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);

    tabWidget->setStyleSheet(TabWidgetStyle);
    tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    /*--------------Setups-----------------*/
    QSettings settings("MyCompany", "WriterApp");
    projectPath = settings.value(projectName + "/path").toString();

    // Создаем базовую структуру
    QDir projectDir(projectPath);
    if (!projectDir.exists())
    {
        projectDir.mkpath(".");
        projectDir.mkdir("Chapters");
        projectDir.mkdir("Assets");
    }
    setupCalendarTab();
    setupCharactersTab();
    setupPlotTab();
    setupAutoSave();

    loadProjectData();
    /*---------------------------------*/

    editor = new QTextEdit(tab1);
    highlighter = new MarkdownHighlighter(editor->document());
    tab1Layout->addWidget(editor);
    setupEditorContextMenu();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(tabWidget);
    setCentralWidget(centralWidget);

    editor->setContextMenuPolicy(Qt::CustomContextMenu);
    editor->setVisible(false);

    editorContextMenu->setStyleSheet(EditorContextMenuStyle);

    QDir chaptersDir(QDir(projectPath).filePath("Chapters"));
    if (chaptersDir.exists())
    {
        loadChaptersFromDisk(chaptersDir, treeModel->invisibleRootItem(), false);
    }

    /*-------------------------Connections------------------------*/
    connect(editor, &QTextEdit::customContextMenuRequested, this, &ProjectWindow::showTextEditContextMenu);

    connect(toolButtonAddChapter, &QToolButton::clicked, this, &ProjectWindow::onAddChapterClicked);
    connect(toolButtonAddFolder, &QToolButton::clicked, this, &ProjectWindow::onAddFolderClicked);
    connect(toolButtonSave, &QToolButton::clicked, this, &ProjectWindow::onSaveClicked);
    connect(toolButtonCompile, &QToolButton::clicked, this, &ProjectWindow::exportToPdf);

    connect(toolButtonCharacters, &QToolButton::clicked, [this]() { tabWidget->setCurrentIndex(1); });
    connect(toolButtonPlot, &QToolButton::clicked, [this]() { tabWidget->setCurrentIndex(2); });
    connect(toolButtonCalendar, &QToolButton::clicked, [this]() { tabWidget->setCurrentIndex(3); });
    // connect(toolButtonSettings, &QToolButton::clicked, [this]() { tabWidget->setCurrentIndex(5); });
    connect(toolButtonCollapse, &QToolButton::clicked, this, &ProjectWindow::toggleTreePanel);

    connect(toolButtonSettings, &QToolButton::clicked, this, &ProjectWindow::settingsOpen);
    connect(toolButtonCalendar, &QToolButton::clicked, this, &ProjectWindow::calendarOpen);
    connect(toolButtonPlot, &QToolButton::clicked, this, &ProjectWindow::plotOpen);
    connect(toolButtonCharacters, &QToolButton::clicked, this, &ProjectWindow::charactersOpen);

    contextMenu = new QMenu(this);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &ProjectWindow::updateTextFormatting);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &ProjectWindow::showTreeContextMenu);
    connect(treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index)
    {
        openChapterForEditing(index);
    });
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index)
    {
        QString tabName = tabWidget->tabText(index);

        const QStringList protectedTabs = {"Calendar", "Plot", "Characters"};

        if (protectedTabs.contains(tabName))
        {
            QMessageBox::information(this, "Info",
                                     tr("The '%1' tab cannot be closed").arg(tabName));
            return;
        }

        if (chapterFilePaths.contains(tabName))
        {
            if (auto* editor = qobject_cast<QTextEdit*>(tabWidget->widget(index)))
            {
                saveChapterContent(tabName, editor->toPlainText());
            }
        }

        tabWidget->removeTab(index);
    });
}

ProjectWindow::~ProjectWindow()
{
    onSaveClicked();
    delete treeView;
    delete toolButtonAddChapter;
    delete toolButtonAddFolder;
    delete toolButtonSave;
    delete tabWidget;
    delete centralWidget;
}

QString ProjectWindow::constructChapterPath(QStandardItem* chapterItem) const
{
    QStandardItem* parent = chapterItem->parent();
    QString path = parent ? constructFolderPath(parent) : projectPath;
    return QDir(path).filePath(chapterItem->text() + ".md");
}

QString ProjectWindow::constructFolderPath(QStandardItem* folderItem) const
{
    if (!folderItem || folderItem == treeModel->invisibleRootItem()) {
        return projectPath;
    }

    QStringList pathParts;
    QStandardItem* current = folderItem;
    while (current && current != treeModel->invisibleRootItem()) {
        pathParts.prepend(current->text());
        current = current->parent();
    }

    return QDir(projectPath).filePath(pathParts.join(QDir::separator()));
}
/*----------------Tree operations---------------------------*/
void ProjectWindow::onAddChapterClicked()
{
    bool ok;
    QString chapterName = QInputDialog::getText(this, "Add Folder",
                                                "Folder name ",
                                                QLineEdit::Normal, "", &ok);
    if (ok && !chapterName.isEmpty())
    {
        addChapter(chapterName);

        QStandardItem* rootItem = treeModel->invisibleRootItem();
        QStandardItem* lastItem = rootItem->child(rootItem->rowCount() - 1);
        QModelIndex index = treeModel->indexFromItem(lastItem);

        openChapterForEditing(index);
    }
}

void ProjectWindow::onAddFolderClicked()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, "Add Folder",
                                               "Folder name ",
                                               QLineEdit::Normal, "", &ok);

    if (!ok || folderName.isEmpty()) return;

    if (!isValidName(folderName))
    {
        QMessageBox::warning(this, "Error", "Invalid folder name!");
        return;
    }

    addFolder(folderName);
}

void ProjectWindow::addFolder(QString& folderName, QStandardItem* parentItem)
{
    QStandardItem* folderItem = new QStandardItem(folderName);
    folderItem->setData(true, Qt::UserRole);
    folderItem->setIcon(QIcon("../resources/icons/folder.png"));

    if (parentItem)
    {
        parentItem->appendRow(folderItem);
    }
    else
    {
        treeModel->appendRow(folderItem);
    }

    QString dirPath = constructFolderPath(folderItem);
    QDir dir;
    if (!dir.mkpath(dirPath))
    {
        QMessageBox::warning(this, "Error", "Failed to create directory: " + dirPath);
    }
}

void ProjectWindow::addChapter(const QString& chapterName, QStandardItem* parentFolder)
{
    if (chapterName.isEmpty()) return;

    // Создаем элемент главы
    QStandardItem* chapterItem = new QStandardItem(chapterName);
    chapterItem->setData("chapter", Qt::UserRole);
    chapterItem->setIcon(QIcon("../resources/icons/document.png"));

    // Добавляем в дерево
    if (parentFolder)
    {
        parentFolder->appendRow(chapterItem);
    }
    else
    {
        treeModel->appendRow(chapterItem);
    }

    // Создаем файл
    QString filePath = constructChapterPath(chapterItem);
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write("");
        file.close();
        chapterFilePaths[chapterName] = filePath;
    }
    else
    {
        QMessageBox::warning(this, "Error",
                             tr("Failed to create chapter file:\n%1").arg(file.errorString()));
    }
}

void ProjectWindow::renameItem(QStandardItem* item)
{
    bool ok;
    QString oldName = item->text();
    QString newName = QInputDialog::getText(this, "Rename",
                                            "New name (invalid chars: \\ / : * ? \" < > |):",
                                            QLineEdit::Normal, "", &ok);
    if (!ok || newName.isEmpty() || newName == oldName) return;

    bool isFolder = item->data(Qt::UserRole).toBool();
    QString oldPath = isFolder ? constructFolderPath(item) : constructChapterPath(item);
    item->setText(newName);
    QString newPath = isFolder ? constructFolderPath(item) : constructChapterPath(item);

    bool success = false;
    if (isFolder)
    {
        QDir dir;
        success = dir.rename(oldPath, newPath);
    }
    else
    {
        success = QFile::rename(oldPath, newPath + ".md");
    }

    if (!success)
    {
        item->setText(oldName);
        QMessageBox::warning(this, "Error", "Failed to rename " + QString(isFolder ? "folder" : "chapter"));
    }
}

void ProjectWindow::deleteItem(QStandardItem* item)
{
    closeTabForChapter(item->text());
    bool isFolder = item->data(Qt::UserRole).toBool();
    QString path = isFolder ? constructFolderPath(item) : constructChapterPath(item);

    bool success = isFolder ? QDir(path).removeRecursively() : QFile::remove(path);
    if (!success)
    {
        QMessageBox::warning(this, "Error", "Failed to delete " + QString(isFolder ? "folder" : "chapter"));
        return;
    }

    QStandardItem* parent = item->parent();
    if (parent)
    {
        parent->removeRow(item->row());
    }
    else
    {
        treeModel->removeRow(item->row());
    }
}

void ProjectWindow::addChapterToFolder(QStandardItem* folderItem)
{
    bool ok;
    QString chapterName = QInputDialog::getText(this, "Add Chapter",
                                                "Enter chapter name:",
                                                QLineEdit::Normal, "", &ok);
    if (ok && !chapterName.isEmpty())
    {
        addChapter(chapterName, folderItem);
    }
}


void ProjectWindow::addChapterToFolder()
{
    QModelIndex index = treeView->currentIndex();
    if (index.isValid())
    {
        QStandardItem* item = treeModel->itemFromIndex(index);
        if (item)
        {
            bool ok;
            QString chapterName = QInputDialog::getText(this, "Add Chapter", "Enter chapter name:", QLineEdit::Normal,
                                                        "", &ok);
            if (ok && !chapterName.isEmpty())
            {
                addChapter(chapterName, item);
            }
        }
    }
}

/*-----------------------Aligning------------*/
void ProjectWindow::alignTextLeft() const
{
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(format);
}

void ProjectWindow::alignTextCenter() const
{
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(format);
}

void ProjectWindow::alignTextRight() const
{
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(format);
}

/*------------------------------------------*/

void ProjectWindow::openChapterForEditing(const QString& chapterName)
{
    QModelIndexList matches = treeModel->match(
        treeModel->index(0, 0),
        Qt::DisplayRole,
        chapterName,
        1,
        Qt::MatchExactly | Qt::MatchRecursive
    );

    if (!matches.isEmpty())
    {
        QStandardItem* item = treeModel->itemFromIndex(matches.first());
        if (item)
        {
            QString filePath = chapterFilePaths.value(chapterName);
            if (!filePath.isEmpty())
            {
                QFile file(filePath);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    editor->setPlainText(file.readAll());
                    file.close();
                }
            }
        }
    }
}

void ProjectWindow::closeTabForChapter(const QString& chapterName) const
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == chapterName)
        {
            tabWidget->removeTab(i);
            break;
        }
    }

    if (tabWidget->count() == 0)
    {
        editor->setVisible(false);
    }
}


void ProjectWindow::onCloseEditorTab(int index) const
{
    if (tabWidget->count() == 1)
    {
        QWidget* emptyTab = new QWidget();
        tabWidget->addTab(emptyTab, "No Chapter Open");
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
    }
    if (tabWidget->count() == 0)
    {
        tabWidget->setVisible(false);
    }
}


void ProjectWindow::toggleTreePanel() const
{
    bool isVisible = treeView->isVisible();
    treeView->setVisible(!isVisible);
    toolButtonAddChapter->setVisible(!isVisible);
    toolButtonAddFolder->setVisible(!isVisible);
    toolButtonSave->setVisible(!isVisible);
    toolButtonCompile->setVisible(!isVisible);

    if (isVisible)
    {
        toolButtonCollapse->setIcon(QIcon("../resources/icons/wheelchair.png"));
    }
    else
    {
        toolButtonCollapse->setIcon(QIcon("../resources/icons/roll.png"));
    }
}

/*----------------Sections opening-------------------------*/

void ProjectWindow::settingsOpen() const
{
    int tabIndex = -1;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == "Settings")
        {
            tabIndex = i;
            break;
        }
    }

    if (tabIndex == -1)
    {
        QWidget* settingsTab = new QWidget();
        QVBoxLayout* settingsLayout = new QVBoxLayout(settingsTab);
        settingsLayout->addWidget(new QLabel()); // Пустой виджет
        tabIndex = tabWidget->addTab(settingsTab, "Settings");
    }

    tabWidget->setCurrentIndex(tabIndex);
}

void ProjectWindow::calendarOpen() const
{
    int tabIndex = -1;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == "Calendar")
        {
            tabIndex = i;
            break;
        }
    }

    if (tabIndex == -1)
    {
        QWidget* calendarTab = new QWidget();
        QVBoxLayout* calendarLayout = new QVBoxLayout(calendarTab);
        calendarLayout->addWidget(new QLabel()); // Пустой виджет
        tabIndex = tabWidget->addTab(calendarTab, "Calendar");
    }

    tabWidget->setCurrentIndex(tabIndex);
}

void ProjectWindow::plotOpen() const
{
    int tabIndex = -1;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == "Plot")
        {
            tabIndex = i;
            break;
        }
    }

    if (tabIndex == -1)
    {
        QWidget* plotTab = new QWidget();
        QVBoxLayout* plotLayout = new QVBoxLayout(plotTab);
        plotLayout->addWidget(new QLabel()); // Пустой виджет
        tabIndex = tabWidget->addTab(plotTab, "Plot");
    }

    tabWidget->setCurrentIndex(tabIndex);
}

void ProjectWindow::charactersOpen() const
{
    int tabIndex = -1;
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == "Characters")
        {
            tabIndex = i;
            break;
        }
    }

    if (tabIndex == -1)
    {
        QWidget* charactersTab = new QWidget();
        QVBoxLayout* charactersLayout = new QVBoxLayout(charactersTab);
        charactersLayout->addWidget(new QLabel()); // Пустой виджет
        tabIndex = tabWidget->addTab(charactersTab, "Characters");
    }

    tabWidget->setCurrentIndex(tabIndex);
}


/*---------------Markdown editor--------------------*/

void ProjectWindow::openChapterForEditing(const QModelIndex& index)
{
    if (!index.isValid()) return;

    QStandardItem* item = treeModel->itemFromIndex(index);
    if (!item || item->data(Qt::UserRole).toString() != "chapter") return;

    QString chapterName = item->text();
    QString filePath = constructChapterPath(item);

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        if (tabWidget->tabText(i) == chapterName)
        {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    MarkdownEditor* editor = createMarkdownEditor();
    layout->addWidget(editor);

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        editor->setPlainText(file.readAll());
        file.close();
    }
    else
    {
        editor->setPlainText("");
    }

    int tabIndex = tabWidget->addTab(tab, chapterName);
    tabWidget->setCurrentIndex(tabIndex);
    chapterFilePaths[chapterName] = filePath;
}

void ProjectWindow::setupEditorContextMenu()
{
    editorContextMenu = new QMenu(this);

    QMenu* standardMenu = editor->createStandardContextMenu();
    for (QAction* action : standardMenu->actions())
    {
        editorContextMenu->addAction(action);
    }
    delete standardMenu;

    editorContextMenu->addSeparator();

    QMenu* markdownMenu = editorContextMenu->addMenu(tr("Markdown Formatting"));
    markdownMenu->setIcon(QIcon(":/icons/markdown.png"));

    auto addFormatAction = [this](const QString& title, const QString& prefix,
                                  const QString& suffix = "", const QIcon& icon = QIcon())
    {
        QAction* action = new QAction(title, this);
        if (!icon.isNull())
        {
            action->setIcon(icon);
        }
        connect(action, &QAction::triggered, [this, prefix, suffix]()
        {
            QTextEdit* currentEditor = qobject_cast<QTextEdit*>(QApplication::focusWidget());
            if (!currentEditor) return;

            QTextCursor cursor = currentEditor->textCursor();
            QString selectedText = cursor.selectedText();

            if (selectedText.isEmpty())
            {
                cursor.insertText(prefix + suffix);
                cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, suffix.length());
            }
            else
            {
                cursor.insertText(prefix + selectedText + suffix);
            }

            currentEditor->setTextCursor(cursor);
        });
        return action;
    };

    markdownMenu->addAction(addFormatAction(tr("Bold"), "**", "**", QIcon(":/icons/bold.png")));
    markdownMenu->addAction(addFormatAction(tr("Italic"), "*", "*", QIcon(":/icons/italic.png")));
    markdownMenu->addAction(addFormatAction(tr("Strikethrough"), "~~", "~~", QIcon(":/icons/strikethrough.png")));
    markdownMenu->addAction(addFormatAction(tr("Inline Code"), "`", "`", QIcon(":/icons/code.png")));
    markdownMenu->addAction(addFormatAction(tr("Link"), "[", "](url)", QIcon(":/icons/link.png")));
    markdownMenu->addAction(addFormatAction(tr("Image"), "![", "](image-url)", QIcon(":/icons/image.png")));

    markdownMenu->addSeparator();
    markdownMenu->addAction(tr("Horizontal Rule"), [this]()
    {
        if (auto editor = qobject_cast<QTextEdit*>(QApplication::focusWidget()))
        {
            editor->textCursor().insertText("\n---\n");
        }
    });

    markdownMenu->addAction(tr("Blockquote"), [this]()
    {
        if (auto editor = qobject_cast<QTextEdit*>(QApplication::focusWidget()))
        {
            QTextCursor cursor = editor->textCursor();
            cursor.insertText("> ");
            editor->setTextCursor(cursor);
        }
    });

    QMenu* alignMenu = editorContextMenu->addMenu(tr("Alignment"));
    alignMenu->setIcon(QIcon(":/icons/align.png"));

    auto addAlignmentAction = [this](const QString& title, Qt::Alignment alignment, const QIcon& icon)
    {
        QAction* action = new QAction(title, this);
        action->setIcon(icon);
        connect(action, &QAction::triggered, [this, alignment]()
        {
            if (auto editor = qobject_cast<QTextEdit*>(QApplication::focusWidget()))
            {
                QTextCursor cursor = editor->textCursor();
                QTextBlockFormat format;
                format.setAlignment(alignment);
                cursor.mergeBlockFormat(format);
                editor->setTextCursor(cursor);
            }
        });
        return action;
    };

    alignMenu->addAction(addAlignmentAction(tr("Left"), Qt::AlignLeft, QIcon(":/icons/align-left.png")));
    alignMenu->addAction(addAlignmentAction(tr("Center"), Qt::AlignCenter, QIcon(":/icons/align-center.png")));
    alignMenu->addAction(addAlignmentAction(tr("Right"), Qt::AlignRight, QIcon(":/icons/align-right.png")));

    editorContextMenu->addSeparator();
    editorContextMenu->addAction(tr("Insert Table"), [this]()
    {
        if (auto editor = qobject_cast<QTextEdit*>(QApplication::focusWidget()))
        {
            editor->textCursor().insertText(
                "\n| Header 1 | Header 2 |\n"
                "|----------|----------|\n"
                "| Cell 1   | Cell 2   |\n"
                "| Cell 3   | Cell 4   |\n"
            );
        }
    });

    editorContextMenu->addAction(tr("Insert Code Block"), [this]()
    {
        if (auto editor = qobject_cast<QTextEdit*>(QApplication::focusWidget()))
        {
            editor->textCursor().insertText("\n```\n// Your code here\n```\n");
        }
    });
}

MarkdownEditor* ProjectWindow::createMarkdownEditor() const
{
    MarkdownEditor* editor = new MarkdownEditor();
    editor->setupContextMenu(editorContextMenu);
    return editor;
}

void ProjectWindow::insertMarkdownFormatting(const QString& prefix, const QString& suffix) const
{
    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();

    if (!selectedText.isEmpty())
    {
        cursor.insertText(prefix + selectedText + suffix);
    }
    else
    {
        cursor.insertText(prefix + suffix);
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, suffix.length());
        editor->setTextCursor(cursor);
    }
}

void ProjectWindow::updateTextFormatting() const
{
    editor->blockSignals(true);

    QTextCursor cursor = editor->textCursor();
    int currentBlockNumber = cursor.blockNumber();

    QTextBlock block = cursor.block();
    highlighter->rehighlightBlock(block);

    if (block.previous().isValid())
        highlighter->rehighlightBlock(block.previous());

    if (block.next().isValid())
        highlighter->rehighlightBlock(block.next());

    editor->blockSignals(false);
}

/*-------------------Context Menu----------------------*/

void ProjectWindow::showTreeContextMenu(const QPoint& pos)
{
    QModelIndex index = treeView->indexAt(pos);
    if (!index.isValid())
        return;

    QStandardItem* item = treeModel->itemFromIndex(index);
    if (!item)
        return;

    bool isFolder = item->data(Qt::UserRole).toBool();
    QMenu menu(this);

    if (isFolder)
    {
        QAction* addChapterAction = menu.addAction("Add Chapter");
        QAction* renameAction = menu.addAction("Rename");
        QAction* deleteAction = menu.addAction("Delete");

        connect(addChapterAction, &QAction::triggered, [this, item]()
        {
            this->addChapterToFolder(item);
        });

        connect(renameAction, &QAction::triggered, [this, item]()
        {
            this->renameItem(item);
        });

        connect(deleteAction, &QAction::triggered, [this, item]()
        {
            this->deleteItem(item);
        });
    }
    else
    {
        QAction* openAction = menu.addAction("Open");
        QAction* renameAction = menu.addAction("Rename");
        QAction* deleteAction = menu.addAction("Delete");

        connect(openAction, &QAction::triggered, this, [this, index]()
        {
            QStandardItem* item = treeModel->itemFromIndex(index);
            if (item)
            {
                this->openChapterForEditing(item->text());
            }
        });

        connect(renameAction, &QAction::triggered, [this, item]()
        {
            this->renameItem(item);
        });

        connect(deleteAction, &QAction::triggered, [this, item]()
        {
            this->deleteItem(item);
        });
    }

    menu.exec(treeView->viewport()->mapToGlobal(pos));
}

void ProjectWindow::showTextEditContextMenu(const QPoint& pos)
{
    QMenu* menu = editor->createStandardContextMenu();
    menu->addSeparator();

    QMenu* markdownMenu = menu->addMenu("Markdown");

    // Жирный текст
    QAction* boldAction = markdownMenu->addAction("Bold (**text**)");
    connect(boldAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("**", "**");
    });

    // Курсив
    QAction* italicAction = markdownMenu->addAction("Italic (*text*)");
    connect(italicAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("*", "*");
    });

    // Зачеркнутый
    QAction* strikeAction = markdownMenu->addAction("Strikethrough (~~text~~)");
    connect(strikeAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("~~", "~~");
    });

    // Встроенный код
    QAction* codeAction = markdownMenu->addAction("Inline Code (`code`)");
    connect(codeAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("`", "`");
    });

    // Ссылка
    QAction* linkAction = markdownMenu->addAction("Link ([text](url))");
    connect(linkAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("[", "](url)");
    });

    // Изображение
    QAction* imageAction = markdownMenu->addAction("Image (![alt](url))");
    connect(imageAction, &QAction::triggered, [this]()
    {
        insertMarkdownFormatting("![", "](url)");
    });

    // Цитата
    QAction* quoteAction = markdownMenu->addAction("Quote (>)");
    connect(quoteAction, &QAction::triggered, [this]()
    {
        QTextCursor cursor = editor->textCursor();
        cursor.insertText("> ");
    });

    // Горизонтальная линия
    QAction* hrAction = markdownMenu->addAction("Horizontal Rule (---)");
    connect(hrAction, &QAction::triggered, [this]()
    {
        QTextCursor cursor = editor->textCursor();
        cursor.insertText("\n---\n");
    });

    // Выравнивание
    QMenu* alignMenu = menu->addMenu("Alignment");

    QAction* alignLeftAction = alignMenu->addAction("Align Left");
    connect(alignLeftAction, &QAction::triggered, this, &ProjectWindow::alignTextLeft);

    QAction* alignCenterAction = alignMenu->addAction("Align Center");
    connect(alignCenterAction, &QAction::triggered, this, &ProjectWindow::alignTextCenter);

    QAction* alignRightAction = alignMenu->addAction("Align Right");
    connect(alignRightAction, &QAction::triggered, this, &ProjectWindow::alignTextRight);

    menu->exec(editor->viewport()->mapToGlobal(pos));
    delete menu;
}

/*----------------------------Calendar--------------------------*/
void ProjectWindow::setupCalendarTab()
{
    QWidget* calendarTab = new QWidget();
    QVBoxLayout* calendarLayout = new QVBoxLayout(calendarTab);

    calendarTab->setStyleSheet(CalendarTabStyle);

    calendarWidget = new QCalendarWidget();
    calendarWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    calendarWidget->setStyleSheet(CalendarStyle);

    setupCalendarNavigation();

    calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendarWidget->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    calendarWidget->setGridVisible(true);


    deadlinesDialog = new QDialog(this);
    deadlinesDialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    deadlinesDialog->setStyleSheet(DeadlinesDialogStyle);

    QVBoxLayout* dialogLayout = new QVBoxLayout(deadlinesDialog);


    deadlinesList = new QListWidget();
    deadlinesList->setStyleSheet(DeadlineListStyle);
    deadlinesList->setSelectionMode(QAbstractItemView::SingleSelection);

    QPushButton* addDeadlineBtn = new QPushButton("+");
    addDeadlineBtn->setStyleSheet(AddDeadlineBtnStyle);


    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    removeSelectedButton = new QPushButton("Remove Selected");
    removeSelectedButton->setStyleSheet(RemoveSelectedDeadlineBtnStyle);

    closeButton = new QPushButton("Close");
    closeButton->setStyleSheet(CloseDeadlineListBtnStyle);

    buttonsLayout->addWidget(addDeadlineBtn);
    buttonsLayout->addWidget(removeSelectedButton);
    buttonsLayout->addWidget(closeButton);

    QWidget* buttonsWidget = new QWidget();
    buttonsWidget->setLayout(buttonsLayout);

    dialogLayout->addWidget(deadlinesList);
    dialogLayout->addWidget(buttonsWidget);

    deadlinesDialog->setLayout(dialogLayout);
    deadlinesDialog->setFixedSize(300, 250);

    calendarContextMenu = new QMenu(this);
    addDeadlineAction = new QAction("Add Deadline", this);
    removeDeadlineAction = new QAction("Remove Deadlines", this);

    calendarContextMenu->addAction(addDeadlineAction);
    calendarContextMenu->addAction(removeDeadlineAction);

    connect(addDeadlineBtn, &QPushButton::clicked,
            this, &ProjectWindow::showAddDeadlineDialog);

    connect(removeSelectedButton, &QPushButton::clicked,
            this, &ProjectWindow::removeSelectedDeadline);
    connect(deadlinesList, &QListWidget::itemSelectionChanged, [this]()
    {
        removeSelectedButton->setEnabled(deadlinesList->currentItem() != nullptr);
    });

    connect(calendarWidget, &QCalendarWidget::customContextMenuRequested,
            this, &ProjectWindow::showCalendarContextMenu);
    connect(addDeadlineAction, &QAction::triggered, [this]()
    {
        addDeadlineForDate(calendarWidget->selectedDate());
    });
    connect(removeDeadlineAction, &QAction::triggered, [this]()
    {
        deadlines.remove(calendarWidget->selectedDate());
        updateCalendarDates();
    });
    connect(calendarWidget, &QCalendarWidget::clicked,
            this, &ProjectWindow::showDeadlinesPopup);
    connect(deadlinesList, &QListWidget::itemClicked,
            this, &ProjectWindow::toggleDeadlineCompletion);
    connect(closeButton, &QPushButton::clicked,
            deadlinesDialog, &QDialog::hide);

    calendarLayout->addWidget(calendarWidget);
    tabWidget->addTab(calendarTab, "Calendar");
}

void ProjectWindow::showCalendarContextMenu(const QPoint& pos)
{
    QDate date = calendarWidget->selectedDate();
    bool hasDeadlines = deadlines.contains(date) && !deadlines[date].isEmpty();

    removeDeadlineAction->setEnabled(hasDeadlines);
    calendarContextMenu->exec(calendarWidget->mapToGlobal(pos));
}

void ProjectWindow::addDeadlineForDate(const QDate& date)
{
    bool ok;
    QString text = QInputDialog::getText(this, "New Deadline",
                                         "Enter deadline description:",
                                         QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        deadlines[date].append(qMakePair(text, false));
        updateCalendarDates();
        showDeadlinesPopup(date);
    }
}

void ProjectWindow::showDeadlinesPopup(const QDate& date)
{
    deadlinesList->clear();

    if (deadlines.contains(date))
    {
        for (const auto& deadline : deadlines[date])
        {
            QListWidgetItem* item = new QListWidgetItem(deadline.first);
            item->setData(Qt::UserRole, deadline.second);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(deadline.second ? Qt::Checked : Qt::Unchecked);

            if (deadline.second)
            {
                item->setForeground(Qt::gray);
                item->setIcon(QIcon(":/icons/checked.png"));
            }
            else
            {
                item->setIcon(QIcon(":/icons/unchecked.png"));
            }

            deadlinesList->addItem(item);
        }
    }

    // Позиционирование
    QTableView* tableView = calendarWidget->findChild<QTableView*>();
    if (tableView)
    {
        QModelIndexList indexes = tableView->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty())
        {
            QRect cellRect = tableView->visualRect(indexes.first());
            QPoint popupPos = tableView->viewport()->mapToGlobal(
                QPoint(cellRect.right(), cellRect.top()));
            deadlinesDialog->move(popupPos);
            deadlinesDialog->show();
        }
    }
}

void ProjectWindow::toggleDeadlineCompletion(QListWidgetItem* item)
{
    QDate date = calendarWidget->selectedDate();
    bool completed = (item->checkState() == Qt::Checked);

    for (auto& deadline : deadlines[date])
    {
        if (deadline.first == item->text())
        {
            deadline.second = completed;
            break;
        }
    }

    item->setForeground(completed ? Qt::gray : Qt::white);
    item->setIcon(QIcon(completed ? ":/icons/checked.png" : ":/icons/unchecked.png"));
    updateCalendarDates();
}

void ProjectWindow::updateCalendarDates() const
{
    QTextCharFormat defaultFormat;
    calendarWidget->setDateTextFormat(QDate(), defaultFormat);

    QTextCharFormat pendingFormat;
    pendingFormat.setFontWeight(QFont::Bold);
    pendingFormat.setForeground(Qt::white);
    pendingFormat.setBackground(QColor("#BF616A"));

    QTextCharFormat completedFormat;
    completedFormat.setFontWeight(QFont::Normal);
    completedFormat.setForeground(Qt::white);
    completedFormat.setBackground(QColor("#A3BE8C"));

    for (auto it = deadlines.constBegin(); it != deadlines.constEnd(); ++it)
    {
        bool hasPending = false;
        bool hasCompleted = false;

        for (const auto& deadline : it.value())
        {
            if (deadline.second) hasCompleted = true;
            else hasPending = true;
        }

        if (hasPending)
        {
            calendarWidget->setDateTextFormat(it.key(), pendingFormat);
        }
        else if (hasCompleted)
        {
            calendarWidget->setDateTextFormat(it.key(), completedFormat);
        }
    }
}

void ProjectWindow::removeSelectedDeadline()
{
    QDate currentDate = calendarWidget->selectedDate();
    QListWidgetItem* item = deadlinesList->currentItem();

    if (!item || !deadlines.contains(currentDate)) return;

    QString deadlineText = item->text();

    auto& dateDeadlines = deadlines[currentDate];
    dateDeadlines.erase(std::remove_if(dateDeadlines.begin(), dateDeadlines.end(),
                                       [&deadlineText](const QPair<QString, bool>& deadline)
                                       {
                                           return deadline.first == deadlineText;
                                       }),
                        dateDeadlines.end());

    if (dateDeadlines.isEmpty())
    {
        deadlines.remove(currentDate);
    }

    delete item;
    updateCalendarDates();

    if (deadlinesList->count() == 0)
    {
        deadlinesDialog->hide();
    }
}


void ProjectWindow::setupCalendarNavigation() const
{
    QToolButton* prevButton = calendarWidget->findChild<QToolButton*>("qt_calendar_prevmonth");
    QToolButton* nextButton = calendarWidget->findChild<QToolButton*>("qt_calendar_nextmonth");

    if (prevButton && nextButton)
    {
        prevButton->setArrowType(Qt::NoArrow);
        nextButton->setArrowType(Qt::NoArrow);

        prevButton->setIcon(QIcon("../resources/icons/arrow-left.png"));
        nextButton->setIcon(QIcon("../resources/icons/arrow-right.png"));

        prevButton->setStyleSheet(CalendarPrevBtnStyle);

        nextButton->setStyleSheet(prevButton->styleSheet());
    }
}

QToolButton* ProjectWindow::createNavigationButton(const QString& iconPath)
{
    QToolButton* button = new QToolButton();
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(24, 24));
    button->setFixedSize(24, 24);
    button->setStyleSheet(CalendarNavigationBtnStyle);
    return button;
}

void ProjectWindow::showAddDeadlineDialog()
{
    bool ok;
    QString text = QInputDialog::getText(
        this,
        "Add Deadline",
        "Enter deadline description:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (ok && !text.isEmpty())
    {
        QDate currentDate = calendarWidget->selectedDate();
        deadlines[currentDate].append(qMakePair(text, false));
        updateCalendarDates();
        showDeadlinesPopup(currentDate);
    }
}

/*---------------------Characters-----------------------------*/
void ProjectWindow::setupCharactersTab()
{
    QWidget* charactersTab = new QWidget();
    QVBoxLayout* charactersLayout = new QVBoxLayout(charactersTab);

    charactersTab->setStyleSheet(CharacterstabStyle);
    charactersList = new QListWidget();
    charactersList->setViewMode(QListWidget::IconMode);
    charactersList->setResizeMode(QListWidget::Adjust);
    charactersList->setMovement(QListWidget::Static);
    charactersList->setSpacing(10);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    QPushButton* addButton = new QPushButton("Add Character");
    QPushButton* removeButton = new QPushButton("Remove");

    addButton->setStyleSheet(CharactersButtonStyle);
    removeButton->setStyleSheet(CharactersButtonStyle);
    removeButton->setEnabled(false);

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addStretch();

    connect(addButton, &QPushButton::clicked, this, &ProjectWindow::showAddCharacterDialog);
    connect(removeButton, &QPushButton::clicked, this, &ProjectWindow::removeSelectedCharacter);
    connect(charactersList, &QListWidget::itemClicked, [removeButton](QListWidgetItem* item)
    {
        removeButton->setEnabled(item != nullptr);
    });
    connect(charactersList, &QListWidget::itemDoubleClicked, this, &ProjectWindow::showCharacterDetails);

    charactersLayout->addLayout(buttonsLayout);
    charactersLayout->addWidget(charactersList);

    tabWidget->addTab(charactersTab, "Characters");
}

void ProjectWindow::showAddCharacterDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Character");
    dialog.setFixedSize(400, 400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLineEdit* nameEdit = new QLineEdit();
    QTextEdit* descEdit = new QTextEdit();
    QLabel* imageLabel = new QLabel();
    imageLabel->setFixedSize(150, 150);
    imageLabel->setAlignment(Qt::AlignCenter);

    QPushButton* imageButton = new QPushButton("Select Image");
    imageButton->setStyleSheet(CharactersImageBtnStyle);

    QString imagePath;
    connect(imageButton, &QPushButton::clicked, [&]()
    {
        imagePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
        if (!imagePath.isEmpty())
        {
            QPixmap pixmap(imagePath);
            if (!pixmap.isNull())
            {
                imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    });

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(new QLabel("Name:"));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("Description:"));
    layout->addWidget(descEdit);
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);
    layout->addWidget(imageButton, 0, Qt::AlignCenter);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString name = nameEdit->text().trimmed();
        QString description = descEdit->toPlainText().trimmed();

        if (!name.isEmpty())
        {
            addCharacterCard(name, description, imagePath);
        }
    }
}

void ProjectWindow::addCharacterCard(const QString& name, const QString& description, const QString& imagePath)
{
    characters[name] = qMakePair(description, imagePath);

    QWidget* cardWidget = new QWidget();
    const int cardWidth = 200;
    QVBoxLayout* cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setContentsMargins(0, 0, 0, 10);
    cardLayout->setSpacing(8);
    cardWidget->setFixedWidth(cardWidth);
    cardWidget->setStyleSheet(
        "QWidget {"
        "   background: #79747E;"
        "   border-radius: 8px;"
        "   margin: 5px;"
        "   color: #FFFFFF;"
        "}"
    );

    QLabel* imageLabel = new QLabel();
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumHeight(180);
    imageLabel->setStyleSheet(
        "QLabel {"
        "   background: #4A4458;"
        "   border-top-left-radius: 8px;"
        "   border-top-right-radius: 8px;"
        "   border-bottom: 1px solid #5E81AC;"
        "}"
    );

    auto loadImage = [imageLabel, cardWidth](const QString& path)
    {
        QPixmap pixmap;
        if (!path.isEmpty() && QFile::exists(path))
        {
            if (pixmap.load(path))
            {
                QPixmap scaled = pixmap.scaledToWidth(cardWidth - 2, Qt::SmoothTransformation);
                imageLabel->setPixmap(scaled);
                return;
            }
        }
        QPixmap defaultPixmap(":/resources/icons/character.png");
        defaultPixmap = defaultPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(defaultPixmap);
    };
    loadImage(imagePath);

    QWidget* textContainer = new QWidget();
    QVBoxLayout* textLayout = new QVBoxLayout(textContainer);
    textLayout->setContentsMargins(10, 0, 10, 0);

    QLabel* nameLabel = new QLabel(name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   color: #FFFFFF;"
        "   padding: 5px 0;"
        "}"
    );

    QLabel* descLabel = new QLabel(description.left(500) + (description.length() > 500 ? "..." : ""));
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   color: #E0E0E0;"
        "}"
    );
    descLabel->setWordWrap(true);
    descLabel->setMaximumHeight(40);

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(descLabel);

    cardLayout->addWidget(imageLabel);
    cardLayout->addWidget(textContainer);

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(cardWidth, 250));
    charactersList->addItem(item);
    charactersList->setItemWidget(item, cardWidget);
    item->setData(Qt::UserRole, name);
}

void ProjectWindow::showCharacterDetails(QListWidgetItem* item)
{
    if (!item) return;

    QString oldName = item->data(Qt::UserRole).toString();
    if (!characters.contains(oldName)) return;

    QDialog dialog(this);
    dialog.setWindowTitle("Edit Character - " + oldName);
    dialog.setMinimumSize(500, 550);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    QLineEdit* nameEdit = new QLineEdit(oldName);
    nameEdit->setStyleSheet(
        "QLineEdit {"
        "   background: #4A4458;"
        "   color: #ECEFF4;"
        "   border: 1px solid #5E81AC;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
    );

    QLabel* imageLabel = new QLabel();
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(250, 250);
    imageLabel->setStyleSheet(
        "QLabel {"
        "   background: #4A4458;"
        "   border-radius: 4px;"
        "}"
    );

    QString currentImagePath = characters[oldName].second;
    QPixmap currentPixmap;
    if (!currentImagePath.isEmpty() && QFile::exists(currentImagePath))
    {
        currentPixmap.load(currentImagePath);
    }
    if (currentPixmap.isNull())
    {
        currentPixmap = QPixmap(":/resources/icons/character.png");
    }
    imageLabel->setPixmap(currentPixmap.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QPushButton* changeImageButton = new QPushButton("Change Image");
    changeImageButton->setStyleSheet(CharactersImageBtnStyle);

    QString newImagePath = currentImagePath;
    connect(changeImageButton, &QPushButton::clicked, [&]()
    {
        QString selectedPath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
        if (!selectedPath.isEmpty())
        {
            newImagePath = selectedPath;
            QPixmap pixmap(newImagePath);
            if (!pixmap.isNull())
            {
                imageLabel->setPixmap(pixmap.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    });

    QTextEdit* descEdit = new QTextEdit(characters[oldName].first);
    descEdit->setStyleSheet(DeskEditStyle);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->setStyleSheet(CharactersButtonStyle);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(new QLabel("Name:"));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("Image:"));
    layout->addWidget(imageLabel);
    layout->addWidget(changeImageButton, 0, Qt::AlignCenter);
    layout->addWidget(new QLabel("Description:"));
    layout->addWidget(descEdit);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString newName = nameEdit->text().trimmed();
        QString newDescription = descEdit->toPlainText().trimmed();

        if (oldName != newName)
        {
            characters[newName] = qMakePair(newDescription, newImagePath);
            characters.remove(oldName);
            item->setData(Qt::UserRole, newName);
        }
        else
        {
            characters[oldName] = qMakePair(newDescription, newImagePath);
        }

        updateCharacterCard(item, newName, newDescription, newImagePath);
    }
}

void ProjectWindow::updateCharacterCard(QListWidgetItem* item, const QString& name,
                                        const QString& description, const QString& imagePath)
{
    QWidget* cardWidget = charactersList->itemWidget(item);
    if (!cardWidget) return;

    QVBoxLayout* cardLayout = qobject_cast<QVBoxLayout*>(cardWidget->layout());
    if (!cardLayout) return;

    QLabel* imageLabel = qobject_cast<QLabel*>(cardLayout->itemAt(0)->widget());
    QWidget* textContainer = qobject_cast<QWidget*>(cardLayout->itemAt(1)->widget());

    if (!imageLabel || !textContainer) return;

    QPixmap pixmap;
    if (!imagePath.isEmpty() && QFile::exists(imagePath))
    {
        pixmap.load(imagePath);
    }
    if (pixmap.isNull())
    {
        pixmap = QPixmap(":/resources/icons/character.png");
    }

    pixmap = pixmap.scaled(imageLabel->width(), imageLabel->height(),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);

    QVBoxLayout* textLayout = qobject_cast<QVBoxLayout*>(textContainer->layout());
    if (!textLayout) return;

    QLabel* nameLabel = qobject_cast<QLabel*>(textLayout->itemAt(0)->widget());
    QLabel* descLabel = qobject_cast<QLabel*>(textLayout->itemAt(1)->widget());

    if (nameLabel) nameLabel->setText(name);
    if (descLabel) descLabel->setText(description.left(500) + (description.length() > 500 ? "..." : ""));
}

void ProjectWindow::removeSelectedCharacter()
{
    QList<QListWidgetItem*> selected = charactersList->selectedItems();
    if (selected.isEmpty()) return;

    QString name = selected.first()->data(Qt::UserRole).toString();
    characters.remove(name);
    delete selected.first();
}

/*---------------------------------------------*/

void ProjectWindow::setupPlotTab()
{
    QWidget* plotTab = new QWidget();
    QVBoxLayout* plotLayout = new QVBoxLayout(plotTab);
    plotLayout->setContentsMargins(0, 0, 0, 0);

    plotScene = new QGraphicsScene(this);
    plotView = new QGraphicsView(plotScene);
    plotView->setRenderHint(QPainter::Antialiasing);
    plotView->setDragMode(QGraphicsView::RubberBandDrag);
    plotView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    plotView->setContextMenuPolicy(Qt::CustomContextMenu);
    plotView->setStyleSheet("QGraphicsView { background: #1D1B20; border: none; }");

    plotView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    plotContextMenu = new QMenu(this);

    QAction* addNodeAction = new QAction("Add Node", this);
    QAction* connectAction = new QAction("Connect Nodes", this);
    QAction* deleteAction = new QAction("Delete", this);
    QAction* groupAction = new QAction("Group", this);
    QAction* ungroupAction = new QAction("Ungroup", this);
    QAction* changeSizeAction = new QAction("Resize Node", this);
    QAction* addImageAction = new QAction("Add Image", this);
    QAction* zoomInAction = new QAction("Zoom In (Ctrl++)", this);
    QAction* zoomOutAction = new QAction("Zoom Out (Ctrl+-)", this);
    QAction* resetZoomAction = new QAction("Reset Zoom", this);
    QAction* deleteArrowAction = new QAction("Delete Arrow", this);

    plotContextMenu->addAction(addNodeAction);
    plotContextMenu->addAction(connectAction);
    plotContextMenu->addAction(deleteAction);
    plotContextMenu->addSeparator();
    plotContextMenu->addAction(groupAction);
    plotContextMenu->addAction(ungroupAction);
    plotContextMenu->addAction(changeSizeAction);
    plotContextMenu->addSeparator();
    plotContextMenu->addAction(addImageAction);
    plotContextMenu->addAction(deleteArrowAction);
    plotContextMenu->addSeparator();
    plotContextMenu->addAction(zoomInAction);
    plotContextMenu->addAction(zoomOutAction);
    plotContextMenu->addAction(resetZoomAction);
    plotContextMenu->addSeparator();

    plotContextMenu->setStyleSheet(PlotContextMenuStyle);

    connect(addNodeAction, &QAction::triggered, [this]()
    {
        addPlotNode("New Node", "", plotView->mapToScene(plotView->viewport()->rect().center()));
    });

    connect(connectAction, &QAction::triggered, this, &ProjectWindow::connectSelectedItems);
    connect(deleteAction, &QAction::triggered, this, &ProjectWindow::removeSelectedPlotItem);
    connect(groupAction, &QAction::triggered, this, &ProjectWindow::groupSelectedItems);
    connect(ungroupAction, &QAction::triggered, this, &ProjectWindow::ungroupSelectedItems);
    connect(changeSizeAction, &QAction::triggered, this, &ProjectWindow::changeNodeSize);
    connect(addImageAction, &QAction::triggered, this, &ProjectWindow::addImageToScene);
    connect(deleteArrowAction, &QAction::triggered, this, &ProjectWindow::deleteSelectedArrow);
    connect(zoomInAction, &QAction::triggered, [this]() { plotView->scale(1.2, 1.2); });
    connect(zoomOutAction, &QAction::triggered, [this]() { plotView->scale(1 / 1.2, 1 / 1.2); });
    connect(resetZoomAction, &QAction::triggered, [this]() { plotView->resetTransform(); });
    connect(plotView, &QGraphicsView::customContextMenuRequested, this, &ProjectWindow::showPlotContextMenu);

    QShortcut* zoomInShortcut = new QShortcut(QKeySequence("Ctrl++"), plotView);
    QShortcut* zoomOutShortcut = new QShortcut(QKeySequence("Ctrl+-"), plotView);
    connect(zoomInShortcut, &QShortcut::activated, [this]() { plotView->scale(1.2, 1.2); });
    connect(zoomOutShortcut, &QShortcut::activated, [this]() { plotView->scale(1 / 1.2, 1 / 1.2); });

    plotLayout->addWidget(plotView);
    tabWidget->addTab(plotTab, "Plot");
}

void ProjectWindow::showPlotContextMenu(const QPoint& pos) const
{
    QPointF scenePos = plotView->mapToScene(pos);
    QGraphicsItem* item = plotScene->itemAt(scenePos, plotView->transform());

    QList<QAction*> actions = plotContextMenu->actions();
    bool hasSelection = !plotScene->selectedItems().isEmpty();
    bool isArrow = item && item->type() == QGraphicsLineItem::Type;

    actions[1]->setEnabled(plotScene->selectedItems().size() >= 2);
    actions[2]->setEnabled(hasSelection);
    actions[4]->setEnabled(hasSelection);
    actions[5]->setEnabled(currentGroup != nullptr);
    actions[6]->setEnabled(hasSelection);
    actions[8]->setEnabled(isArrow);

    plotContextMenu->exec(plotView->mapToGlobal(pos));
}

void ProjectWindow::deleteSelectedArrow() const
{
    QList<QGraphicsItem*> selected = plotScene->selectedItems();
    for(QGraphicsItem* item : selected) {
        if(item->type() == QGraphicsLineItem::Type) {
            plotScene->removeItem(item);
            delete item;
        }
    }
}

bool ProjectWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == plotView->viewport() && isDrawingLine)
    {
        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF scenePos = plotView->mapToScene(mouseEvent->pos());

            if (tempLine)
            {
                QLineF line = tempLine->line();
                line.setP2(scenePos);
                tempLine->setLine(line);
            }
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                QPointF scenePos = plotView->mapToScene(mouseEvent->pos());
                QGraphicsItem* item = plotScene->itemAt(scenePos, plotView->transform());

                if (item)
                {
                    QLineF line(lineStartPoint, item->sceneBoundingRect().center());
                    QGraphicsLineItem* arrowLine = new QGraphicsLineItem(line);

                    QPolygonF arrowHead;
                    arrowHead << QPointF(0, 0) << QPointF(-10, -5) << QPointF(-10, 5);

                    QGraphicsPolygonItem* arrow = new QGraphicsPolygonItem(arrowHead, arrowLine);
                    arrow->setBrush(Qt::black);
                    arrow->setPos(line.p2());

                    qreal angle = ::acos(line.dx() / line.length());
                    if (line.dy() >= 0) angle = 2 * M_PI - angle;
                    arrow->setRotation(-angle * 180 / M_PI);

                    plotScene->addItem(arrowLine);
                }

                plotScene->removeItem(tempLine);
                delete tempLine;
                tempLine = nullptr;
                isDrawingLine = false;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void ProjectWindow::connectSelectedItems()
{
    QList<QGraphicsItem*> selected = plotScene->selectedItems();
    if(selected.size() == 2) {
        QPointF p1 = selected[0]->sceneBoundingRect().center();
        QPointF p2 = selected[1]->sceneBoundingRect().center();

        Arrow* arrow = new Arrow(QLineF(p1, p2));
        plotScene->addItem(arrow);
        arrow->updatePosition();
    } else
    {
        isDrawingLine = true;
        lineStartPoint = plotView->mapToScene(plotView->viewport()->mapFromGlobal(QCursor::pos()));
        tempLine = new QGraphicsLineItem(QLineF(lineStartPoint, lineStartPoint));
        tempLine->setPen(QPen(Qt::black, 2, Qt::DashLine));
        plotScene->addItem(tempLine);
    }
}

void ProjectWindow::groupSelectedItems()
{
    QList<QGraphicsItem*> selected = plotScene->selectedItems();
    if (!selected.isEmpty())
    {
        currentGroup = plotScene->createItemGroup(selected);
    }
}

void ProjectWindow::ungroupSelectedItems()
{
    if (currentGroup)
    {
        plotScene->destroyItemGroup(currentGroup);
        currentGroup = nullptr;
    }
}

void ProjectWindow::changeNodeSize()
{
    QList<QGraphicsItem*> selected = plotScene->selectedItems();
    if (selected.isEmpty()) return;

    bool ok;
    double width = QInputDialog::getDouble(this, "Resize Node", "Width:",
                                           selected[0]->boundingRect().width(), 50, 1000, 1, &ok);
    if (!ok) return;

    double height = QInputDialog::getDouble(this, "Resize Node", "Height:",
                                            selected[0]->boundingRect().height(), 50, 1000, 1, &ok);
    if (!ok) return;

    foreach(QGraphicsItem* item, selected)
    {
        if (item->type() == QGraphicsRectItem::Type)
        {
            QGraphicsRectItem* rect = qgraphicsitem_cast<QGraphicsRectItem*>(item);
            rect->setRect(0, 0, width, height);
        }
        else if (item->type() == QGraphicsEllipseItem::Type)
        {
            QGraphicsEllipseItem* ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
            ellipse->setRect(0, 0, width, height);
        }
    }
}

void ProjectWindow::addPlotNode(const QString& title,
                               const QString& description,
                               const QPointF& pos,
                               const QString& shape,
                               const QSizeF& size)
{
    QDialog dialog(this);
    dialog.setWindowTitle(shape.isEmpty() ? "Add Plot Node" : "Edit Plot Node");
    dialog.setFixedSize(450, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

     QGroupBox* textGroup = new QGroupBox("Text Properties");
    QFormLayout* textLayout = new QFormLayout(textGroup);

    QLineEdit* titleEdit = new QLineEdit(title);
    QTextEdit* descEdit = new QTextEdit(description);
    QFontComboBox* fontCombo = new QFontComboBox();
    QSpinBox* fontSizeSpin = new QSpinBox();
    fontSizeSpin->setRange(6, 72);
    fontSizeSpin->setValue(12);

    QPushButton* textColorBtn = new QPushButton("Choose Color");
    QColor textColor = Qt::black;
    textColorBtn->setStyleSheet(QString("background: %1; color: %2;")
        .arg(textColor.name(), textColor.lightness() > 128 ? "black" : "white"));

    textLayout->addRow("Title:", titleEdit);
    textLayout->addRow("Description:", descEdit);
    textLayout->addRow("Font:", fontCombo);
    textLayout->addRow("Font Size:", fontSizeSpin);
    textLayout->addRow("Text Color:", textColorBtn);

    QGroupBox* nodeGroup = new QGroupBox("Node Properties");
    QFormLayout* nodeLayout = new QFormLayout(nodeGroup);

    QComboBox* shapeCombo = new QComboBox();
    shapeCombo->addItem("Rectangle", "rectangle");
    shapeCombo->addItem("Circle", "circle");
    shapeCombo->addItem("Rounded", "rounded");

    QDoubleSpinBox* widthSpin = new QDoubleSpinBox();
    QDoubleSpinBox* heightSpin = new QDoubleSpinBox();
    widthSpin->setRange(50, 1000);
    heightSpin->setRange(50, 1000);
    widthSpin->setValue(size.width());
    heightSpin->setValue(size.height());

    QPushButton* nodeColorBtn = new QPushButton("Choose Color");
    QColor nodeColor = QColor("#5E81AC");
    nodeColorBtn->setStyleSheet(QString("background: %1; color: %2;")
        .arg(nodeColor.name(), nodeColor.lightness() > 128 ? "black" : "white"));

    nodeLayout->addRow("Shape:", shapeCombo);
    nodeLayout->addRow("Width:", widthSpin);
    nodeLayout->addRow("Height:", heightSpin);
    nodeLayout->addRow("Node Color:", nodeColorBtn);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    mainLayout->addWidget(textGroup);
    mainLayout->addWidget(nodeGroup);
    mainLayout->addWidget(buttons);

    connect(textColorBtn, &QPushButton::clicked, [&]() {
        QColor newColor = QColorDialog::getColor(textColor);
        if(newColor.isValid()) {
            textColor = newColor;
            textColorBtn->setStyleSheet(QString("background: %1; color: %2;")
                .arg(newColor.name(), newColor.lightness() > 128 ? "black" : "white"));
        }
    });

    connect(nodeColorBtn, &QPushButton::clicked, [&]() {
        QColor newColor = QColorDialog::getColor(nodeColor);
        if(newColor.isValid()) {
            nodeColor = newColor;
            nodeColorBtn->setStyleSheet(QString("background: %1; color: %2;")
                .arg(newColor.name(), newColor.lightness() > 128 ? "black" : "white"));
        }
    });

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() == QDialog::Accepted)
    {
        QAbstractGraphicsShapeItem* shapeItem = nullptr;
        const QString nodeShape = shapeCombo->currentData().toString();
        const QSizeF nodeSize(widthSpin->value(), heightSpin->value());

        if(nodeShape == "rectangle") {
            shapeItem = new QGraphicsRectItem(0, 0, nodeSize.width(), nodeSize.height());
        }
        else if(nodeShape == "circle") {
            shapeItem = new QGraphicsEllipseItem(0, 0, nodeSize.width(), nodeSize.height());
        }
        else if(nodeShape == "rounded") {
            auto rectItem = new QGraphicsRectItem(0, 0, nodeSize.width(), nodeSize.height());
            rectItem->setPen(QPen(Qt::black, 2, Qt::DashLine));
            shapeItem = rectItem;
        }

        if(shapeItem)
        {
            shapeItem->setPos(pos);
            shapeItem->setBrush(nodeColor);
            shapeItem->setPen(QPen(Qt::black, 2));
            shapeItem->setFlags(QGraphicsItem::ItemIsMovable |
                               QGraphicsItem::ItemIsSelectable |
                               QGraphicsItem::ItemSendsGeometryChanges);

            ResizableTextItem* textItem = new ResizableTextItem(titleEdit->text(), shapeItem);
            textItem->setDefaultTextColor(textColor);
            textItem->setFont(QFont(fontCombo->currentFont().family(), fontSizeSpin->value()));
            textItem->setTextWidth(nodeSize.width() - 20);
            textItem->setPos(10, 10);

            textItem->setData(NodeRole::FontSize, fontSizeSpin->value());
            textItem->setData(NodeRole::TextColor, textColor.name());
            textItem->setData(NodeRole::TextWidth, textItem->textWidth());
            textItem->setData(NodeRole::TextHeight, textItem->boundingRect().height());

            plotScene->addItem(shapeItem);
            shapeItem->setSelected(true);
        }
    }
}

void ProjectWindow::addImageToScene()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty())
    {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull())
        {
            QGraphicsPixmapItem* imageItem = plotScene->addPixmap(pixmap);
            imageItem->setFlag(QGraphicsItem::ItemIsMovable);
            imageItem->setFlag(QGraphicsItem::ItemIsSelectable);
            imageItem->setPos(plotView->mapToScene(plotView->viewport()->rect().center()));
        }
    }
}


void ProjectWindow::removeSelectedPlotItem() const
{
    QList<QGraphicsItem*> selectedItems = plotScene->selectedItems();
    for (QGraphicsItem* item : selectedItems)
    {
        plotScene->removeItem(item);
        delete item;
    }
}


/*---------------------------------------------*/
void ProjectWindow::onSaveClicked()
{
    saveAllChapters();
    saveCharactersToJson();
    saveDeadlinesToJson();
    savePlotToJson();

    QMessageBox::information(this, "Saved", "All project data has been saved");
}

void ProjectWindow::saveAllChapters()
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        QString chapterName = tabWidget->tabText(i);
        if (auto* tab = tabWidget->widget(i))
        {
            if (auto* editor = tab->findChild<QTextEdit*>())
            {
                if (chapterFilePaths.contains(chapterName))
                {
                    QString content = editor->toPlainText();
                    saveChapterContent(chapterName, content);
                }
            }
        }
    }
}

void ProjectWindow::saveChapterContent(const QString& chapterName, const QString& content)
{
    QString filePath = chapterFilePaths.value(chapterName);
    if (filePath.isEmpty())
    {
        qWarning() << "Path not found for chapter:" << chapterName;
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << content;
        file.close();
        qDebug() << "Saved chapter:" << chapterName << "to" << filePath;
    }
    else
    {
        qWarning() << "Failed to save chapter:" << chapterName << "Error:" << file.errorString();
    }
}

void ProjectWindow::setupAutoSave()
{
    QTimer* autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, [this]()
    {
        if (!isWindowModified()) return;
        saveAllChapters();
        saveCharactersToJson();
        saveDeadlinesToJson();
        savePlotToJson();
    });
    autoSaveTimer->start(300000);
}

void ProjectWindow::saveChapterContent(QString& chapterName, QString& content)
{
    if (!chapterFilePaths.contains(chapterName))
    {
        qWarning() << "Chapter path not found for:" << chapterName;
        return;
    }

    QFile file(chapterFilePaths[chapterName]);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << content;
        file.close();
    }
    else
    {
        QMessageBox::warning(this, "Error",
                             tr("Failed to save chapter %1:\n%2").arg(chapterName).arg(file.errorString()));
    }
}

void ProjectWindow::loadChapterContent(QString& chapterName, QTextEdit* editor)
{
    if (!chapterFilePaths.contains(chapterName))
    {
        qWarning() << "Chapter path not found for:" << chapterName;
        return;
    }

    QFile file(chapterFilePaths[chapterName]);
    if (file.open(QIODevice::ReadOnly | QTextStream::Text))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        editor->setPlainText(stream.readAll());
        file.close();
    }
}


void ProjectWindow::restoreSession()
{
    QSettings settings;
    QStringList openChapters = settings.value("session/openChapters").toStringList();

    for (const QString& chapter : openChapters)
    {
        if (chapterFilePaths.contains(chapter))
        {
            openChapterForEditing(chapter);
        }
    }
}

void ProjectWindow::savePlotToJson()
{
    QString filePath = QDir(getAssetsPath()).filePath("plot.json");
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open plot file for writing:" << filePath;
        return;
    }

    QJsonObject root;
    QJsonArray nodesArray;

    for (QGraphicsItem* item : plotScene->items())
    {
        if (item->type() == QGraphicsRectItem::Type ||
            item->type() == QGraphicsEllipseItem::Type)
        {
            QJsonObject nodeObj;

            nodeObj["x"] = item->pos().x();
            nodeObj["y"] = item->pos().y();
            nodeObj["type"] = item->type();

            if (auto textItem = item->childItems().value(0); textItem && textItem->type() == QGraphicsTextItem::Type)
            {
                nodeObj["title"] = static_cast<QGraphicsTextItem*>(textItem)->toPlainText();
            }

            if (auto shapeItem = dynamic_cast<QAbstractGraphicsShapeItem*>(item))
            {
                nodeObj["color"] = shapeItem->brush().color().name();
                QRectF boundingRect = shapeItem->boundingRect();
                nodeObj["width"] = boundingRect.width();
                nodeObj["height"] = boundingRect.height();
            }

            nodesArray.append(nodeObj);
        }
        else if (item->type() == QGraphicsLineItem::Type)
        {
            QJsonObject connectionObj;
            QGraphicsLineItem* line = static_cast<QGraphicsLineItem*>(item);

            connectionObj["x1"] = line->line().x1();
            connectionObj["y1"] = line->line().y1();
            connectionObj["x2"] = line->line().x2();
            connectionObj["y2"] = line->line().y2();

            nodesArray.append(connectionObj);
        }
    }

    root["nodes"] = nodesArray;
    file.write(QJsonDocument(root).toJson());
    file.close();
}

void ProjectWindow::loadPlotFromJson()
{
    QString filePath = QDir(getAssetsPath()).filePath("plot.json");
    if (!QFile::exists(filePath)) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open plot file:" << filePath;
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    QJsonArray nodesArray = root["nodes"].toArray();

    plotScene->clear();

    QList<QGraphicsItem*> items;
    for (const QJsonValue& nodeValue : nodesArray)
    {
        QJsonObject nodeObj = nodeValue.toObject();

        if (nodeObj.contains("type"))
        {
            QAbstractGraphicsShapeItem* shapeItem = nullptr;
            QPointF pos(nodeObj["x"].toDouble(), nodeObj["y"].toDouble());

            if (nodeObj["type"].toInt() == QGraphicsRectItem::Type)
            {
                shapeItem = new QGraphicsRectItem(0, 0,
                                                  nodeObj["width"].toDouble(), nodeObj["height"].toDouble());
            }
            else if (nodeObj["type"].toInt() == QGraphicsEllipseItem::Type)
            {
                shapeItem = new QGraphicsEllipseItem(0, 0,
                                                     nodeObj["width"].toDouble(), nodeObj["height"].toDouble());
            }

            if (shapeItem)
            {
                shapeItem->setPos(pos);
                shapeItem->setBrush(QColor(nodeObj["color"].toString()));
                shapeItem->setPen(QPen(Qt::black, 2));
                shapeItem->setFlag(QGraphicsItem::ItemIsMovable);
                shapeItem->setFlag(QGraphicsItem::ItemIsSelectable);

                if (nodeObj.contains("title"))
                {
                    QGraphicsTextItem* textItem = new QGraphicsTextItem(nodeObj["title"].toString(), shapeItem);
                    textItem->setPos(10, 10);
                    textItem->setDefaultTextColor(Qt::white);
                }

                plotScene->addItem(shapeItem);
                items.append(shapeItem);
            }
        }
    }

    for (const QJsonValue& nodeValue : nodesArray)
    {
        QJsonObject nodeObj = nodeValue.toObject();

        if (!nodeObj.contains("type") && nodeObj.contains("x1"))
        {
            QLineF line(nodeObj["x1"].toDouble(), nodeObj["y1"].toDouble(),
                        nodeObj["x2"].toDouble(), nodeObj["y2"].toDouble());

            QGraphicsLineItem* lineItem = new QGraphicsLineItem(line);
            lineItem->setPen(QPen(Qt::black, 2));
            plotScene->addItem(lineItem);

            QPolygonF arrowHead;
            arrowHead << QPointF(0, 0) << QPointF(-10, -5) << QPointF(-10, 5);

            QGraphicsPolygonItem* arrow = new QGraphicsPolygonItem(arrowHead);
            arrow->setBrush(Qt::black);
            arrow->setPos(line.p2());

            qreal angle = ::acos(line.dx() / line.length());
            if (line.dy() >= 0) angle = 2 * M_PI - angle;
            arrow->setRotation(-angle * 180 / M_PI);

            plotScene->addItem(arrow);
        }
    }
}

void ProjectWindow::saveDeadlinesToJson()
{
    QString filePath = QDir(getAssetsPath()).filePath("deadlines.json");

    QJsonObject root;
    for (auto it = deadlines.constBegin(); it != deadlines.constEnd(); ++it)
    {
        QJsonArray dateArray;
        for (const auto& deadline : it.value())
        {
            QJsonObject dlObj;
            dlObj["text"] = deadline.first;
            dlObj["completed"] = deadline.second;
            dateArray.append(dlObj);
        }
        root[it.key().toString("yyyy-MM-dd")] = dateArray;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QJsonDocument(root).toJson());
    }
}

void ProjectWindow::saveCharactersToJson()
{
    QDir().mkpath(getAssetsPath());
    QString filePath = QDir(getAssetsPath()).filePath("characters.json");

    QJsonArray charactersArray;
    for (const auto& [name, data] : characters.asKeyValueRange())
    {
        QJsonObject charObj;
        charObj["name"] = name;
        charObj["description"] = data.first;
        charObj["imagePath"] = data.second;
        charactersArray.append(charObj);
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QJsonDocument(charactersArray).toJson());
    }
}

void ProjectWindow::loadChaptersFromDisk(const QDir& dir, QStandardItem* parentItem, bool isRoot = false)
{
    QStringList ignoreList = {"Assets", "characters.json", "deadlines.json", "plot.json"};

    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                                              QDir::DirsFirst | QDir::Name);

    foreach(const QFileInfo& fileInfo, entries)
    {
        if (ignoreList.contains(fileInfo.fileName())) continue;

        if (fileInfo.isDir())
        {
            QStandardItem* folderItem = new QStandardItem(fileInfo.fileName());
            folderItem->setIcon(QIcon("../resources/icons/folder.png"));
            folderItem->setData("folder", Qt::UserRole);
            parentItem->appendRow(folderItem);

            loadChaptersFromDisk(QDir(fileInfo.filePath()), folderItem, false
            );
        }
        else if (fileInfo.suffix().toLower() == "md" || isRoot)
        {
            if (!isRoot || fileInfo.suffix().toLower() == "md")
            {
                QString chapterName = fileInfo.baseName();
                QStandardItem* chapterItem = new QStandardItem(chapterName);
                chapterItem->setIcon(QIcon("../resources/icons/document.png"));
                chapterItem->setData("chapter", Qt::UserRole);
                parentItem->appendRow(chapterItem);

                chapterFilePaths[chapterName] = fileInfo.absoluteFilePath();
                qDebug() << "Loaded chapter:" << chapterName << "from" << fileInfo.absoluteFilePath();
            }
        }
    }
}

void ProjectWindow::loadProjectData()
{
    treeModel->clear();
    treeModel->setHorizontalHeaderLabels(QStringList() << "Project Structure");
    chapterFilePaths.clear();

    QDir projectDir(projectPath);
    loadChaptersFromDisk(projectDir, treeModel->invisibleRootItem(), true);

    loadCharactersFromJson();
    loadDeadlinesFromJson();
    loadPlotFromJson();

    treeView->expandAll();
}

void ProjectWindow::loadCharactersFromJson()
{
    QString filePath = QDir(getAssetsPath()).filePath("characters.json");
    if (!QFile::exists(filePath)) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
        for (const auto& value : array)
        {
            QJsonObject obj = value.toObject();
            QString name = obj["name"].toString();
            QString desc = obj["description"].toString();
            QString imgPath = obj["imagePath"].toString();
            characters[name] = {desc, imgPath};

            addCharacterCard(name, desc, imgPath);
        }
    }
}

void ProjectWindow::loadDeadlinesFromJson()
{
    QString filePath = QDir(getAssetsPath()).filePath("deadlines.json");
    if (!QFile::exists(filePath)) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        for (auto it = root.constBegin(); it != root.constEnd(); ++it)
        {
            QDate date = QDate::fromString(it.key(), "yyyy-MM-dd");
            QList<QPair<QString, bool>> dateDeadlines;

            for (const auto& value : it->toArray())
            {
                QJsonObject obj = value.toObject();
                dateDeadlines.append({obj["text"].toString(), obj["completed"].toBool()});
            }

            deadlines[date] = dateDeadlines;
        }
        updateCalendarDates();
    }
}

//----------------------------------------------------------------//

void ProjectWindow::exportToPdf()
{
    QDialog selectionDialog(this);
    selectionDialog.setWindowTitle("Select Chapters to Export");
    selectionDialog.setMinimumSize(600, 600);

    QVBoxLayout mainLayout(&selectionDialog);

    QGroupBox chaptersGroup("Available Chapters");
    QVBoxLayout chaptersLayout(&chaptersGroup);

    QVector<ChapterInfo> chapters;
    QVector<QCheckBox*> chapterCheckboxes;

    fillChaptersList(treeModel->invisibleRootItem(), chapters, chapterCheckboxes, &chaptersLayout);

    QHBoxLayout selectButtonsLayout;
    QPushButton selectAllBtn("Select All");
    QPushButton deselectAllBtn("Deselect All");
    selectButtonsLayout.addWidget(&selectAllBtn);
    selectButtonsLayout.addWidget(&deselectAllBtn);
    chaptersLayout.addLayout(&selectButtonsLayout);

    connect(&selectAllBtn, &QPushButton::clicked, [&chapterCheckboxes]()
    {
        for (QCheckBox* cb : chapterCheckboxes) cb->setChecked(true);
    });
    connect(&deselectAllBtn, &QPushButton::clicked, [&chapterCheckboxes]()
    {
        for (QCheckBox* cb : chapterCheckboxes) cb->setChecked(false);
    });

    QGroupBox settingsGroup("Export Settings");
    QFormLayout settingsLayout(&settingsGroup);

    QComboBox styleCombo;
    styleCombo.addItem("Print Friendly", "print");

    QComboBox pageSizeCombo;
    pageSizeCombo.addItem("A4", QPageSize::A4);
    pageSizeCombo.addItem("Letter", QPageSize::Letter);
    pageSizeCombo.addItem("A5", QPageSize::A5);

    QComboBox orientationCombo;
    orientationCombo.addItem("Portrait", QPageLayout::Portrait);
    orientationCombo.addItem("Landscape", QPageLayout::Landscape);

    QSpinBox marginLeftSpin, marginRightSpin, marginTopSpin, marginBottomSpin;
    for (auto spin : {&marginLeftSpin, &marginRightSpin, &marginTopSpin, &marginBottomSpin})
    {
        spin->setRange(0, 30);
        spin->setValue(5);
        spin->setSuffix(" mm");
    }

    QLineEdit fileNameEdit("MyBook.pdf");
    QPushButton browseButton("Browse...");

    settingsLayout.addRow("Export style:", &styleCombo);
    settingsLayout.addRow("Page size:", &pageSizeCombo);
    settingsLayout.addRow("Orientation:", &orientationCombo);
    settingsLayout.addRow("Left margin:", &marginLeftSpin);
    settingsLayout.addRow("Right margin:", &marginRightSpin);
    settingsLayout.addRow("Top margin:", &marginTopSpin);
    settingsLayout.addRow("Bottom margin:", &marginBottomSpin);
    settingsLayout.addRow("Output file:", &fileNameEdit);
    settingsLayout.addRow("", &browseButton);

    connect(&browseButton, &QPushButton::clicked, [&]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Save PDF",
                                                        QStandardPaths::writableLocation(
                                                            QStandardPaths::DocumentsLocation),
                                                        "PDF Files (*.pdf)");
        if (!fileName.isEmpty()) fileNameEdit.setText(fileName);
    });

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout.addWidget(&chaptersGroup);
    mainLayout.addWidget(&settingsGroup);
    mainLayout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &selectionDialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &selectionDialog, &QDialog::reject);

    if (selectionDialog.exec() != QDialog::Accepted) return;

    QVector<ChapterInfo> selectedChapters;
    for (int i = 0; i < chapters.size(); ++i)
    {
        if (chapterCheckboxes[i]->isChecked())
        {
            selectedChapters.append(chapters[i]);
        }
    }

    if (selectedChapters.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "No chapters selected for export");
        return;
    }

    QString fileName = fileNameEdit.text();
    if (fileName.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please specify output file name");
        return;
    }
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

    createPdfDocument(
        fileName,
        selectedChapters,
        styleCombo.currentData().toString(),
        static_cast<QPageSize::PageSizeId>(pageSizeCombo.currentData().toInt()),
        static_cast<QPageLayout::Orientation>(orientationCombo.currentData().toInt()),
        {
            marginLeftSpin.value(),
            marginRightSpin.value(),
            marginTopSpin.value(),
            marginBottomSpin.value()
        }
    );

    QMessageBox::information(this, "Export Complete",
                             QString("Successfully exported %1 chapters to:\n%2").arg(selectedChapters.size()).arg(
                                 fileName));
}

void ProjectWindow::fillChaptersList(QStandardItem* parentItem, QVector<ChapterInfo>& chapters,
                                     QVector<QCheckBox*>& checkboxes, QVBoxLayout* layout)
{
    for (int i = 0; i < parentItem->rowCount(); ++i)
    {
        QStandardItem* item = parentItem->child(i);
        if (item->data(Qt::UserRole).toString() == "chapter")
        {
            ChapterInfo chapter;
            chapter.name = item->text();
            chapter.filePath = chapterFilePaths.value(chapter.name);
            chapters.append(chapter);

            QCheckBox* checkBox = new QCheckBox(chapter.name);
            checkBox->setChecked(true);
            checkboxes.append(checkBox);
            layout->addWidget(checkBox);
        }
        else if (item->data(Qt::UserRole).toString() == "folder")
        {
            QGroupBox* folderGroup = new QGroupBox(item->text());
            QVBoxLayout* folderLayout = new QVBoxLayout(folderGroup);
            fillChaptersList(item, chapters, checkboxes, folderLayout);
            layout->addWidget(folderGroup);
        }
    }
}

void ProjectWindow::createPdfDocument(const QString& fileName,
                                      const QVector<ChapterInfo>& chapters,
                                      const QString& style,
                                      QPageSize::PageSizeId pageSize,
                                      QPageLayout::Orientation orientation,
                                      const QMargins& marginsMm)
{
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(pageSize));
    pdfWriter.setPageOrientation(orientation);

    QPageLayout layout = pdfWriter.pageLayout();
    layout.setUnits(QPageLayout::Millimeter);
    layout.setMargins(QMarginsF(marginsMm.left(), marginsMm.top(),
                                marginsMm.right(), marginsMm.bottom()));
    pdfWriter.setPageLayout(layout);

    QTextDocument document;
    QTextCursor cursor(&document);

    PdfStyle pdfStyle = getPdfStyle(style);

    document.setDefaultFont(pdfStyle.defaultFont);
    document.setDefaultStyleSheet(QString(
        "body { color: %1; background-color: %2; }"
        "a { color: %3; text-decoration: none; }"
        "pre, code { font-family: %4; background-color: %5; color: %6; }"
        "hr { border: 1px solid %7; }"
    ).arg(pdfStyle.textColor.name(),
          pdfStyle.backgroundColor.name(),
          pdfStyle.linkColor.name(),
          pdfStyle.monospaceFont,
          pdfStyle.codeBackgroundColor.name(),
          pdfStyle.codeTextColor.name(),
          pdfStyle.dividerColor.name()));

    addTitlePage(cursor, pdfStyle);

    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.insertBlock(pageBreakFormat);
    cursor.insertText("");

    if (chapters.size() > 1)
    {
        addTableOfContents(cursor, chapters, pdfStyle);

        cursor.insertBlock(pageBreakFormat);
        cursor.insertText("");
    }

    for (int i = 0; i < chapters.size(); ++i)
    {
        addChapterContent(cursor, chapters[i], pdfStyle, i, chapters.size());

        if (i < chapters.size() - 1)
        {
            cursor.insertBlock(pageBreakFormat);
            cursor.insertText("");
        }
    }

    document.print(&pdfWriter);
}

void ProjectWindow::addTitlePage(QTextCursor& cursor, const PdfStyle& style)
{
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat titleFormat;
    titleFormat.setFont(style.titleFont);
    titleFormat.setForeground(style.titleColor);

    cursor.insertBlock(centerFormat);
    cursor.insertText(projectName, titleFormat);

    cursor.insertBlock();
    cursor.insertText("\n\n");

    QTextCharFormat subtitleFormat;
    subtitleFormat.setFont(style.subtitleFont);
    subtitleFormat.setForeground(style.subtitleColor);
}

void ProjectWindow::addTableOfContents(QTextCursor& cursor,
                                       const QVector<ChapterInfo>& chapters,
                                       const PdfStyle& style)
{
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat titleFormat;
    titleFormat.setFont(style.heading1Font);
    titleFormat.setForeground(style.heading1Color);

    cursor.insertBlock(centerFormat);
    cursor.insertText("Table of Contents", titleFormat);
    cursor.insertBlock();

    QTextBlockFormat tocFormat;
    tocFormat.setLeftMargin(50);

    QTextCharFormat tocItemFormat;
    tocItemFormat.setFont(style.tocFont);
    tocItemFormat.setForeground(style.tocColor);

    for (const ChapterInfo& chapter : chapters)
    {
        cursor.insertBlock(tocFormat);
        cursor.insertText(chapter.name, tocItemFormat);
    }
}

void ProjectWindow::addChapterContent(QTextCursor& cursor,
                                      const ChapterInfo& chapter,
                                      const PdfStyle& style,
                                      int chapterIndex,
                                      int totalChapters)
{
    QTextBlockFormat chapterTitleFormat;
    chapterTitleFormat.setAlignment(Qt::AlignCenter);
    chapterTitleFormat.setBottomMargin(15);

    QTextCharFormat chapterTitleCharFormat;
    chapterTitleCharFormat.setFont(style.heading1Font);
    chapterTitleCharFormat.setForeground(style.heading1Color);

    cursor.insertBlock(chapterTitleFormat);
    cursor.insertText(chapter.name, chapterTitleCharFormat);

    QTextBlockFormat defaultFormat;
    defaultFormat.setAlignment(Qt::AlignLeft);
    defaultFormat.setTopMargin(10);
    defaultFormat.setBottomMargin(10);

    cursor.insertBlock(defaultFormat);
    cursor.insertText("");

    QFile file(chapter.filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open chapter file:" << chapter.filePath;
        return;
    }

    QString content = file.readAll();
    file.close();

    QString html = convertMarkdownToHtml(content, style);
    cursor.insertHtml(html);
}

QString ProjectWindow::convertMarkdownToHtml(const QString& markdown, const PdfStyle& style)
{
    QString html;
    QStringList lines = markdown.split('\n');
    bool inList = false;
    bool inCodeBlock = false;
    bool inQuote = false;

    for (QString line : lines)
    {
        line = line.trimmed();

        if (line.startsWith("```"))
        {
            if (inCodeBlock)
            {
                html += "</pre></code>";
            }
            else
            {
                html += "<pre><code>";
            }
            inCodeBlock = !inCodeBlock;
            continue;
        }

        if (inCodeBlock)
        {
            html += line.toHtmlEscaped() + "\n";
            continue;
        }

        if (line.startsWith("> "))
        {
            if (!inQuote)
            {
                html += "<blockquote>";
                inQuote = true;
            }
            line = line.mid(2);
        }
        else if (inQuote)
        {
            html += "</blockquote>";
            inQuote = false;
        }

        if (line.startsWith("###### "))
        {
            html += QString("<h6 style='color:%1;'>%2</h6>")
                .arg(style.heading6Color.name(), line.mid(7));
        }
        else if (line.startsWith("##### "))
        {
            html += QString("<h5 style='color:%1;'>%2</h5>")
                .arg(style.heading5Color.name(), line.mid(6));
        }
        else if (line.startsWith("#### "))
        {
            html += QString("<h4 style='color:%1;'>%2</h4>")
                .arg(style.heading4Color.name(), line.mid(5));
        }
        else if (line.startsWith("### "))
        {
            html += QString("<h3 style='color:%1;'>%2</h3>")
                .arg(style.heading3Color.name(), line.mid(4));
        }
        else if (line.startsWith("## "))
        {
            html += QString("<h2 style='color:%1;'>%2</h2>")
                .arg(style.heading2Color.name(), line.mid(3));
        }
        else if (line.startsWith("# "))
        {
            html += QString("<h1 style='color:%1;'>%2</h1>")
                .arg(style.heading1Color.name(), line.mid(2));
        }
        else if (line.startsWith("- ") || line.startsWith("* "))
        {
            if (!inList)
            {
                html += "<ul>";
                inList = true;
            }
            html += QString("<li style='color:%1;'>%2</li>")
                .arg(style.textColor.name(), line.mid(2));
        }
        else
        {
            if (inList)
            {
                html += "</ul>";
                inList = false;
            }

            // Форматирование текста
            line.replace(QRegularExpression("\\*\\*(.*?)\\*\\*"),
                         QString("<b style='color:%1;'>\\1</b>").arg(style.boldTextColor.name()));
            line.replace(QRegularExpression("__(.*?)__"),
                         QString("<b style='color:%1;'>\\1</b>").arg(style.boldTextColor.name()));
            line.replace(QRegularExpression("\\*(.*?)\\*"),
                         QString("<i style='color:%1;'>\\1</i>").arg(style.italicTextColor.name()));
            line.replace(QRegularExpression("_(.*?)_"),
                         QString("<i style='color:%1;'>\\1</i>").arg(style.italicTextColor.name()));
            line.replace(QRegularExpression("`([^`]+)`"),
                         QString("<code style='background-color:%1;color:%2;'>\\1</code>")
                         .arg(style.codeBackgroundColor.name(), style.codeTextColor.name()));

            if (line.startsWith("---") || line.startsWith("***"))
            {
                html += QString("<hr style='color:%1;'>").arg(style.dividerColor.name());
                continue;
            }

            line.replace(QRegularExpression("\\[([^\\]]+)\\]\\(([^\\)]+)\\)"),
                         QString("<a href='\\2' style='color:%1;'>\\1</a>").arg(style.linkColor.name()));
            line.replace(QRegularExpression("!\\[([^\\]]+)\\]\\(([^\\)]+)\\)"),
                         QString("<img src='\\2' alt='\\1' style='max-width:100%;'>"));

            if (!line.isEmpty())
            {
                html += QString("<p style='color:%1;'>%2</p>")
                    .arg(style.textColor.name(), line);
            }
        }
    }

    if (inList) html += "</ul>";
    if (inQuote) html += "</blockquote>";
    if (inCodeBlock) html += "</pre></code>";

    return html;
}

PdfStyle ProjectWindow::getPdfStyle(const QString& styleName)
{
    PdfStyle style;

    if (styleName == "dark")
    {
        style.backgroundColor = QColor("#1D1B20");
        style.textColor = QColor("#E0E0E0");
        style.titleColor = QColor("#88C0D0");
        style.subtitleColor = QColor("#81A1C1");
        style.heading1Color = QColor("#8FBCBB");
        style.heading2Color = QColor("#88C0D0");
        style.heading3Color = QColor("#81A1C1");
        style.heading4Color = QColor("#5E81AC");
        style.heading5Color = QColor("#4C566A");
        style.heading6Color = QColor("#3B4252");
        style.linkColor = QColor("#5E81AC");
        style.codeBackgroundColor = QColor("#2E3440");
        style.codeTextColor = QColor("#D8DEE9");
        style.dividerColor = QColor("#4C566A");
        style.boldTextColor = QColor("#88C0D0");
        style.italicTextColor = QColor("#D8DEE9");
    }
    else if (styleName == "light")
    {
        style.backgroundColor = Qt::white;
        style.textColor = Qt::black;
        style.titleColor = QColor("#1A5FB4");
        style.subtitleColor = QColor("#3584E4");
        style.heading1Color = QColor("#1A5FB4");
        style.heading2Color = QColor("#3584E4");
        style.heading3Color = QColor("#26A269");
        style.heading4Color = QColor("#E66100");
        style.heading5Color = QColor("#9141AC");
        style.heading6Color = QColor("#C01C28");
        style.linkColor = QColor("#1A5FB4");
        style.codeBackgroundColor = QColor("#F6F5F4");
        style.codeTextColor = Qt::black;
        style.dividerColor = Qt::darkGray;
        style.boldTextColor = Qt::black;
        style.italicTextColor = Qt::black;
    }
    else
    {
        style.backgroundColor = Qt::white;
        style.textColor = Qt::black;
        style.titleColor = Qt::black;
        style.subtitleColor = Qt::darkGray;
        style.heading1Color = Qt::black;
        style.heading2Color = Qt::black;
        style.heading3Color = Qt::black;
        style.heading4Color = Qt::black;
        style.heading5Color = Qt::black;
        style.heading6Color = Qt::black;
        style.linkColor = Qt::black;
        style.codeBackgroundColor = QColor("#F6F5F4");
        style.codeTextColor = Qt::black;
        style.dividerColor = Qt::black;
        style.boldTextColor = Qt::black;
        style.italicTextColor = Qt::black;
    }

    style.defaultFont = QFont("Arial", 12);
    style.titleFont = QFont("Arial", 24, QFont::Bold);
    style.subtitleFont = QFont("Arial", 14, QFont::Normal, true);
    style.heading1Font = QFont("Arial", 22, QFont::Bold);
    style.heading2Font = QFont("Arial", 20, QFont::Bold);
    style.heading3Font = QFont("Arial", 18, QFont::Bold);
    style.heading4Font = QFont("Arial", 16, QFont::Bold);
    style.heading5Font = QFont("Arial", 14, QFont::Bold);
    style.heading6Font = QFont("Arial", 12, QFont::Bold);
    style.tocFont = QFont("Arial", 12);
    style.monospaceFont = "Courier New, monospace";

    return style;
}
