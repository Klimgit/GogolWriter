// MainWindow.cpp
#include "MainWindow.h"

#include "../../resources/styles/MainStyle.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);


    setWindowTitle("Gogol Writer");
    setFixedSize(500, 300);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainVerticalLayout = new QVBoxLayout(centralWidget);
    mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
    mainVerticalLayout->setSpacing(0);

    QWidget* titleBar = new QWidget(this);
    titleBar->setStyleSheet("background-color: #4A4458;");
    titleBar->setFixedHeight(30);
    titleBar->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    titleBar->installEventFilter(this);
    titleBar->setMouseTracking(true);

    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(5, 0, 5, 0);

    QLabel* titleLabel = new QLabel("Gogol Writer", titleBar);
    titleLabel->setStyleSheet("color: white; font-size: 14px;" "font-weight: bold;");

    QPushButton* minimizeButton = new QPushButton("---", titleBar);
    minimizeButton->setStyleSheet(MinimizeButtonStyle);

    QPushButton* closeButton = new QPushButton("x", titleBar);
    closeButton->setStyleSheet(CloseButtonStyle);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(closeButton);

    mainVerticalLayout->addWidget(titleBar);

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_projectList = new QListWidget(this);
    mainLayout->addWidget(m_projectList);
    m_projectList->setFixedWidth(200);
    m_projectList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_projectList->setSpacing(3);
    m_projectList->setStyleSheet(ProjectListStyle);

    m_contextMenu = new QMenu(this);
    const QAction* renameAction = m_contextMenu->addAction("Rename");
    const QAction* deleteAction = m_contextMenu->addAction("Delete");

    m_projectList->setContextMenuPolicy(Qt::CustomContextMenu);

    QWidget* rightLayoutWidget = new QWidget(this);
    rightLayoutWidget->setStyleSheet("background-color: #625B71;");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightLayoutWidget);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(20, 10, 20, 10);
    rightLayout->setSpacing(15);

    m_createButton = new QPushButton("Create new project", rightLayoutWidget);
    rightLayout->addWidget(m_createButton);
    m_createButton->setStyleSheet(CreateButtonStyle);


    mainLayout->addWidget(rightLayoutWidget);
    mainVerticalLayout->addLayout(mainLayout);

    setCentralWidget(centralWidget);

    connect(m_projectList, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(renameAction, &QAction::triggered, this, &MainWindow::renameProject);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteProject);

    connect(closeButton, &QPushButton::clicked, this, &QMainWindow::close);
    connect(minimizeButton, &QPushButton::clicked, this, &QMainWindow::showMinimized);

    connect(m_createButton, &QPushButton::clicked, this, &MainWindow::onCreateProjectClicked);
    connect(m_projectList, &QListWidget::itemDoubleClicked, this, &MainWindow::onProjectItemClicked);
}

MainWindow::~MainWindow() = default;


/*-------------Context Menu------------------------*/
void MainWindow::showContextMenu(const QPoint& pos) const
{
    QListWidgetItem* item = m_projectList->itemAt(pos);
    if (item)
    {
        m_contextMenu->exec(m_projectList->mapToGlobal(pos));
    }
}

/*------------Project methods---------------------*/
void MainWindow::onProjectItemClicked(QListWidgetItem* item)
{
    emit openProjectWindowRequested(item->text());
}

void MainWindow::setProjects(const QStringList& projects) const
{
    m_projectList->clear();
    m_projectList->addItems(projects);
}

void MainWindow::onCreateProjectClicked()
{
    QString name = QInputDialog::getText(this, "Create Project", "Enter project name:");
    if (name.isEmpty()) return;

    emit createProjectRequested(name);
}

void MainWindow::renameProject()
{
    QListWidgetItem* item = m_projectList->currentItem();
    if (!item) return;

    QString oldName = item->text();
    QString newName = QInputDialog::getText(this, "Rename project", "New name:", QLineEdit::Normal,
                                            oldName);
    if (!newName.isEmpty() && newName != oldName)
    {
        emit renameProjectRequested(oldName, newName);
    }
}

void MainWindow::deleteProject()
{
    QListWidgetItem* item = m_projectList->currentItem();
    if (!item) return;

    QString projectName = item->text();

    emit deleteProjectRequested(projectName);
}
