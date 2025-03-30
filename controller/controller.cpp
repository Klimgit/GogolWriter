#include "controller.h"
#include <QApplication>
#include <QDebug>

MainController::MainController(Model* model, MainWindow* view, QObject* parent)
    : QObject(parent), m_model(model), m_view(view)
{
    connect(m_view, &MainWindow::createProjectRequested, this, &MainController::onCreateProjectRequested);
    connect(m_view, &MainWindow::openProjectWindowRequested, this, &MainController::onOpenProjectWindowRequested);
    connect(m_view, &MainWindow::renameProjectRequested, this, &MainController::onRenameProjectRequested);
    connect(m_view, &MainWindow::deleteProjectRequested, this, &MainController::onDeleteProjectRequested);

    m_view->setProjects(m_model->projects());

    connect(m_model, &Model::projectsChanged, [this]()
    {
        m_view->setProjects(m_model->projects());
    });
}

MainController::~MainController()
{
    for (auto window : m_projectWindows)
    {
        delete window;
    }
}

void MainController::onCreateProjectRequested(const QString& name) const
{
    m_model->addProject(name);
}

void MainController::onRenameProjectRequested(const QString& oldName, const QString& newName) const
{
    QString errorMsg;
    if (!m_model->renameProject(oldName, newName, &errorMsg))
    {
        QMessageBox::critical(
            m_view,
            tr("Error"),
            tr("%3")
            .arg(oldName)
            .arg(newName)
            .arg(errorMsg)
        );
    }
}

void MainController::onOpenProjectWindowRequested(const QString& name)
{
    qDebug() << "Opening project window for: " << name;

    if (!m_projectWindows.contains(name))
    {
        ProjectWindow* projectWindow = new ProjectWindow(name);
        m_projectWindows[name] = projectWindow;

        connect(projectWindow, &ProjectWindow::destroyed, this, [this, name]()
        {
            m_projectWindows.remove(name);
        });
    }

    m_projectWindows[name]->show();
    m_projectWindows[name]->raise();
    m_projectWindows[name]->activateWindow();
}

void MainController::onDeleteProjectRequested(const QString& name) const
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(m_view,
                                  tr("Confirm Deletion"),
                                  tr("Are you sure you want to delete project '%1' and ALL its contents?").arg(name),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    if (!m_model->removeProject(name))
    {
        QMessageBox::critical(m_view,
                              tr("Deletion Error"),
                              tr("Failed to completely delete project '%1'.\n"
                                  "Some files may still remain on disk.").arg(name));
    }
}
