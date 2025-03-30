#pragma once

#include <QMainWindow>
#include <QGraphicsSceneMouseEvent>
#include <QTabWidget>
#include <QTreeView>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include <QDir>
#include "QStandardPaths"
#include <QApplication>
#include <QCalendarWidget>
#include <QListWidget>
#include <utility>
#include <QToolTip>
#include <QTableView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTreeView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QStyle>
#include <QContextMenuEvent>
#include <QTextBrowser>
#include <QPushButton>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QInputDialog>
#include <QTextDocument>
#include <QRegularExpression>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QColorDialog>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
// #include <QPrinter>
#include <QFileDialog>
#include <QImageWriter>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileSystemWatcher>
#include <qpagelayout.h>
#include <qpdfwriter.h>
#include <QShortcut>

#include "../tools/MarkdownHighlighter.h"
#include "../tools/MarkdownEditor.h"


struct PdfStyle
{
    // Цвета
    QString author;
    QColor backgroundColor; ///< Фоновый цвет документа
    QColor textColor; ///< Основной цвет текста
    QColor titleColor; ///< Цвет заголовка документа
    QColor subtitleColor; ///< Цвет подзаголовка
    QColor heading1Color; ///< Цвет заголовков 1 уровня
    QColor heading2Color; ///< Цвет заголовков 2 уровня
    QColor heading3Color; ///< Цвет заголовков 3 уровня
    QColor heading4Color; ///< Цвет заголовков 4 уровня
    QColor heading5Color; ///< Цвет заголовков 5 уровня
    QColor heading6Color; ///< Цвет заголовков 6 уровня
    QColor linkColor; ///< Цвет ссылок
    QColor codeBackgroundColor; ///< Фоновый цвет блоков кода
    QColor codeTextColor; ///< Цвет текста в блоках кода
    QColor dividerColor; ///< Цвет горизонтальных линий
    QColor boldTextColor; ///< Цвет жирного текста
    QColor italicTextColor; ///< Цвет курсивного текста
    QColor quoteTextColor; ///< Цвет текста в цитатах
    QColor quoteBorderColor; ///< Цвет границы цитат
    QColor tocColor; ///< Цвет текста оглавления

    // Шрифты
    QFont defaultFont; ///< Основной шрифт документа
    QFont titleFont; ///< Шрифт заголовка документа
    QFont subtitleFont; ///< Шрифт подзаголовка
    QFont heading1Font; ///< Шрифт заголовков 1 уровня
    QFont heading2Font; ///< Шрифт заголовков 2 уровня
    QFont heading3Font; ///< Шрифт заголовков 3 уровня
    QFont heading4Font; ///< Шрифт заголовков 4 уровня
    QFont heading5Font; ///< Шрифт заголовков 5 уровня
    QFont heading6Font; ///< Шрифт заголовков 6 уровня
    QFont tocFont; ///< Шрифт оглавления
    QFont codeFont; ///< Шрифт для блоков кода

    QString monospaceFont; ///< Название моноширинного шрифта

    // Отступы
    qreal paragraphSpacing; ///< Интервал между параграфами
    qreal lineSpacing; ///< Межстрочный интервал
    qreal chapterSpacing; ///< Отступ между главами

    /**
     * @brief Конструктор по умолчанию, инициализирует значения
     */
    PdfStyle() :
        backgroundColor(Qt::white),
        textColor(Qt::black),
        titleColor(Qt::black),
        subtitleColor(Qt::darkGray),
        heading1Color(Qt::black),
        heading2Color(Qt::black),
        heading3Color(Qt::black),
        heading4Color(Qt::black),
        heading5Color(Qt::black),
        heading6Color(Qt::black),
        linkColor(Qt::blue),
        codeBackgroundColor(QColor(240, 240, 240)),
        codeTextColor(Qt::black),
        dividerColor(Qt::lightGray),
        boldTextColor(Qt::black),
        italicTextColor(Qt::black),
        quoteTextColor(Qt::darkGray),
        quoteBorderColor(Qt::gray),
        tocColor(Qt::black),
        paragraphSpacing(10.0),
        lineSpacing(1.2),
        chapterSpacing(30.0)
    {
        defaultFont = QFont("Arial", 12);
        titleFont = QFont("Arial", 24, QFont::Bold);
        subtitleFont = QFont("Arial", 14, QFont::Normal, true);
        heading1Font = QFont("Arial", 22, QFont::Bold);
        heading2Font = QFont("Arial", 20, QFont::Bold);
        heading3Font = QFont("Arial", 18, QFont::Bold);
        heading4Font = QFont("Arial", 16, QFont::Bold);
        heading5Font = QFont("Arial", 14, QFont::Bold);
        heading6Font = QFont("Arial", 12, QFont::Bold);
        tocFont = QFont("Arial", 12);
        codeFont = QFont("Courier New", 11);
        monospaceFont = "Courier New, monospace";
    }
};

struct ChapterInfo
{
    QString name; ///< Название главы
    QString filePath; ///< Полный путь к файлу главы
    bool selected; ///< Флаг выбора для экспорта
    int wordCount; ///< Количество слов в главе
    int pageCount; ///< Количество страниц в главе (при экспорте)
    QDateTime lastModified; ///< Время последнего изменения

    ChapterInfo() :
        name(""),
        filePath(""),
        selected(false),
        wordCount(0),
        pageCount(0),
        lastModified(QDateTime::currentDateTime())
    {
    }

    ChapterInfo(const QString& chapterName, const QString& path) :
        name(chapterName),
        filePath(path),
        selected(true),
        wordCount(0),
        pageCount(0),
        lastModified(QDateTime::currentDateTime())
    {
    }

    bool exists() const
    {
        return QFile::exists(filePath);
    }

    qint64 size() const
    {
        return QFileInfo(filePath).size();
    }

    void updateLastModified()
    {
        lastModified = QFileInfo(filePath).lastModified();
    }
};

class Arrow : public QGraphicsLineItem {
public:
    Arrow(const QLineF& line, QGraphicsItem* parent = nullptr)
        : QGraphicsLineItem(line, parent) {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        this->setAcceptHoverEvents(true);
        setPen(QPen(Qt::black, 2));

        arrowHead = new QGraphicsPolygonItem(this);
        QPolygonF head;
        head << QPointF(0, 0) << QPointF(-10, -5) << QPointF(-10, 5);
        arrowHead->setPolygon(head);
        arrowHead->setBrush(Qt::black);
        updatePosition();
    }

    void updatePosition() {
        QLineF l = line();
        arrowHead->setPos(l.p2());

        qreal angle = ::acos(l.dx() / l.length());
        if(l.dy() >= 0) angle = 2 * M_PI - angle;
        arrowHead->setRotation(-angle * 180 / M_PI);
    }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
        QGraphicsLineItem::mouseMoveEvent(event);
        updatePosition();
    }

private:
    QGraphicsPolygonItem* arrowHead;
};


class ResizableTextItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    explicit ResizableTextItem(const QString& text, QGraphicsItem* parent = nullptr)
        : QGraphicsTextItem(text, parent) {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    QRectF boundingRect() const override {
        return QGraphicsTextItem::boundingRect().adjusted(-5, -5, 5, 5);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        QGraphicsTextItem::paint(painter, option, widget);

        if (isSelected()) {
            painter->setPen(Qt::blue);
            painter->drawRect(boundingRect().adjusted(1, 1, -1, -1));

            painter->setBrush(Qt::white);
            QRectF rect = boundingRect();
            painter->drawRect(QRectF(rect.right() - 5, rect.bottom() - 5, 10, 10));
        }
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (event->button() == Qt::LeftButton &&
            event->pos().x() > boundingRect().right() - 10 &&
            event->pos().y() > boundingRect().bottom() - 10) {
            m_resizing = true;
            event->accept();
            return;
        }
        QGraphicsTextItem::mousePressEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
        if (m_resizing) {
            QRectF newRect = boundingRect();
            newRect.setRight(event->pos().x());
            newRect.setBottom(event->pos().y());
            document()->setTextWidth(newRect.width());
            update();
            event->accept();
        } else {
            QGraphicsTextItem::mouseMoveEvent(event);
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override {
        m_resizing = false;
        QGraphicsTextItem::mouseReleaseEvent(event);
    }

private:
    bool m_resizing = false;
};



class TextEventFilter : public QObject {
public:
    explicit TextEventFilter(ResizableTextItem* item) : QObject(item), textItem(item) {}

    bool eventFilter(QObject* obj, QEvent* event) override {
        if(event->type() == QEvent::GraphicsSceneWheel &&
           QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            QFont font = textItem->font();
            font.setPointSize(font.pointSize() + (wheelEvent->angleDelta().y() > 0 ? 1 : -1));
            textItem->setFont(font);
            return true;
        }
        return QObject::eventFilter(obj, event);
    }

private:
    ResizableTextItem* textItem;
};

enum NodeRole {
    Type = Qt::UserRole + 1,
    Title,
    Description,
    Color,
    Shape,
    TextWidth = Qt::UserRole + 10,
    TextHeight,
    Size,
    FontSize,
    TextColor
};

class ProjectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectWindow(QString name, QWidget* parent = nullptr);
    ~ProjectWindow() override;
    QString constructFolderPath(QStandardItem* folderItem) const;
    QString constructChapterPath(QStandardItem* chapterItem) const;
    void onSaveClicked();

private slots:
    void setupAutoSave();
    void onAddChapterClicked();
    void onAddFolderClicked();
    void addFolder(QString& folderName, QStandardItem* parentItem = nullptr);
    void addChapter(const QString& chapterName, QStandardItem* parentFolder = nullptr);
    void showTreeContextMenu(const QPoint& pos);

    void showTextEditContextMenu(const QPoint& pos);
    void insertMarkdownFormatting(const QString& prefix, const QString& suffix) const;

    void addChapterToFolder();
    void onCloseEditorTab(int index) const;

    void alignTextLeft() const;
    void alignTextCenter() const;
    void alignTextRight() const;

    void updateTextFormatting() const;
    void openChapterForEditing(const QString& chapterName);
    void closeTabForChapter(const QString& chapterName) const;
    void toggleTreePanel() const;

    void settingsOpen() const;
    void calendarOpen() const;
    void plotOpen() const;
    void charactersOpen() const;
    void openChapterForEditing(const QModelIndex& index);

    void setupEditorContextMenu();
    void renameItem(QStandardItem* item);
    void deleteItem(QStandardItem* item);
    void addChapterToFolder(QStandardItem* folderItem);

    void showCalendarContextMenu(const QPoint& pos);
    void addDeadlineForDate(const QDate& date);
    void toggleDeadlineCompletion(QListWidgetItem* item);
    void showDeadlinesPopup(const QDate& date);
    void updateCalendarDates() const;
    void removeSelectedDeadline();
    void setupCalendarNavigation() const;
    static QToolButton* createNavigationButton(const QString& iconPath);
    void showAddDeadlineDialog();

    void setupCharactersTab();
    void showAddCharacterDialog();
    void addCharacterCard(const QString& name, const QString& description, const QString& imagePath = "");

    void removeSelectedCharacter();
    void showCharacterDetails(QListWidgetItem* item);
    void updateCharacterCard(QListWidgetItem* item, const QString& name, const QString& description,
                             const QString& imagePath);

    void addPlotNode(const QString& title, const QString& description, const QPointF& pos,
                     const QString& shape = "rectangle", const QSizeF& size = QSizeF(150, 100));
    void showPlotContextMenu(const QPoint& pos) const;
    void deleteSelectedArrow() const;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void removeSelectedPlotItem() const;
    void connectSelectedItems();
    void groupSelectedItems();
    void ungroupSelectedItems();
    void changeNodeSize();
    void addImageToScene();


    void exportToPdf();
    void fillChaptersList(QStandardItem* parentItem, QVector<ChapterInfo>& chapters, QVector<QCheckBox*>& checkboxes,
                          QVBoxLayout* layout);
    void createPdfDocument(const QString& fileName, const QVector<ChapterInfo>& chapters, const QString& style,
                           QPageSize::PageSizeId pageSize, QPageLayout::Orientation orientation,
                           const QMargins& marginsMm);
    void addTitlePage(QTextCursor& cursor, const PdfStyle& style);
    void addTableOfContents(QTextCursor& cursor, const QVector<ChapterInfo>& chapters, const PdfStyle& style);
    void addChapterContent(QTextCursor& cursor, const ChapterInfo& chapter, const PdfStyle& style, int chapterIndex,
                           int totalChapters);
    QString convertMarkdownToHtml(const QString& markdown, const PdfStyle& style);
    PdfStyle getPdfStyle(const QString& styleName);

private:
    QString projectName;
    QString projectPath;

    QTreeView* treeView;
    QStandardItemModel* treeModel;

    QWidget* centralWidget;
    QHBoxLayout* mainLayout;
    QVBoxLayout* topLayout;
    QHBoxLayout* buttonLayout;

    QVBoxLayout* sidePanelLayout;
    QWidget* sidePanel;

    // QToolButton* toolButtonEditor;
    QToolButton* toolButtonCharacters;
    QToolButton* toolButtonPlot;
    QToolButton* toolButtonCalendar;
    QToolButton* toolButtonSettings;
    QToolButton* toolButtonAddChapter;
    QToolButton* toolButtonAddFolder;
    QToolButton* toolButtonSave;
    QToolButton* toolButtonCollapse;
    QToolButton* toolButtonCompile;

    QTabWidget* tabWidget;
    QMenu* tabContextMenu;
    QTextEdit* editor;
    MarkdownHighlighter* highlighter;

    QMenu* contextMenu;

    [[nodiscard]] MarkdownEditor* createMarkdownEditor() const;
    QMenu* editorContextMenu = nullptr;

    QCalendarWidget* calendarWidget;
    QMap<QDate, QList<QPair<QString, bool>>> deadlines;
    QDialog* deadlinesDialog;
    QListWidget* deadlinesList;
    QPushButton* closeButton;
    QMenu* calendarContextMenu;
    QAction* addDeadlineAction;
    QAction* removeDeadlineAction;
    QPushButton* removeSelectedButton;
    void setupCalendarTab();

    QListWidget* charactersList;
    QMap<QString, QPair<QString, QString>> characters;

    QGraphicsScene* plotScene;
    QGraphicsView* plotView;
    QMenu* plotContextMenu;
    QGraphicsItemGroup* currentGroup;
    QPointF lineStartPoint;
    QGraphicsLineItem* tempLine;
    bool isDrawingLine;
    void setupPlotTab();

    QFileSystemWatcher* fsWatcher;

    bool moveFile(const QString& oldPath, const QString& newPath);
    bool checkWritePermissions(const QString& path);
    QString getUniqueFilename(const QString& path);
    void showFileMoveError(const QString& error);
    void refreshProjectStructure(const QString& path);
    QModelIndexList getExpandedItems() const;
    void collectExpandedIndexes(const QModelIndex& parent, QModelIndexList& list) const;
    void restoreExpandedState(const QModelIndexList& indexes);
    QString getCurrentSelectionPath() const;
    void restoreSelection(const QString& path);
    bool isValidMove(QStandardItem* source, QStandardItem* target);

    void saveChapterContent(QString& chapterName, QString& content);
    void loadChapterContent(QString& chapterName, QTextEdit* editor);
    void saveAllChapters();
    void saveChapterContent(const QString& chapterName, const QString& content);
    void restoreSession();

    void saveCharactersToJson();
    void loadCharactersFromJson();
    void loadChaptersFromDisk(const QDir& dir, QStandardItem* parentItem, bool isRoot);
    void loadProjectData();

    void saveDeadlinesToJson();
    void loadDeadlinesFromJson();
    void setupDragDrop();
    bool dropMimeData(QStandardItemModel* model, const QMimeData* data, Qt::DropAction action, int row, int column,
                      const QModelIndex& parent);
    void updateFilesystemAfterMove(QStandardItem* movedItem, QStandardItem* newParentItem, QStandardItem* sourceParent,
                                   int sourceRow);

    void savePlotToJson();
    void loadPlotFromJson();
    QMap<QString, QString> chapterFilePaths;

    static bool isValidName(const QString& name)
    {
        static const QRegularExpression regex("^[^\\\\/:*?\"<>|]+$");

        return !name.isEmpty() &&
            name.length() <= 255 &&
            regex.match(name).hasMatch() &&
            name != "." && name != ".." &&
            !name.endsWith(' ') &&
            !name.startsWith(' ');
    }

    QString getAssetsPath() const
    {
        return QDir(projectPath).filePath("Assets");
    }
};
