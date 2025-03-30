#include "model.h"

#include <QStandardPaths>

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QProcess>


Model::Model(QObject* parent)
    : QObject(parent), m_settings("MyCompany", "WriterApp")
{
    m_projects = m_settings.value("projects").toStringList();

    QDir baseDir(getBaseProjectsDirectory());
    if (!baseDir.exists())
    {
        baseDir.mkpath(".");
    }
}

QString Model::getBaseProjectsDirectory()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return QDir(documentsPath).filePath("GogolWriter");
}

bool Model::isValidProjectName(const QString& name)
{
    QRegularExpression regex("^[a-zA-Zа-яА-Я0-9_\\- ]+$");
    return regex.match(name).hasMatch();
}


QStringList Model::projects() const
{
    return m_projects;
}

void Model::addProject(const QString& name)
{
    if (!isValidProjectName(name))
    {
        QMessageBox::warning(nullptr, "Ошибка", "Недопустимое имя проекта!");
        return;
    }

    if (!m_projects.contains(name))
    {
        QString projectPath = QDir(getBaseProjectsDirectory()).filePath(name);

        if (QDir(projectPath).exists())
        {
            QMessageBox::warning(nullptr, "Ошибка", "Проект с таким именем уже существует!");
            return;
        }

        if (!QDir().mkpath(projectPath))
        {
            QMessageBox::warning(nullptr, "Ошибка", "Не удалось создать директорию проекта!");
            return;
        }

        m_projects.append(name);
        m_settings.setValue("projects", m_projects);
        m_settings.setValue(name + "/path", projectPath);
        emit projectsChanged();
    }
}

bool Model::removeProject(const QString& name)
{
    if (!m_projects.contains(name))
    {
        qWarning() << "Project not found:" << name;
        return false;
    }

    bool success = true;
    QString path = projectPath(name);

    if (!path.isEmpty() && QDir(path).exists())
    {
        QDir dir(path);

        if (!dir.removeRecursively())
        {
            qWarning() << "Failed to delete project directory:" << path;
            success = false;


#ifdef Q_OS_WIN
            QProcess::execute("cmd", {"/c", "rmdir", "/s", "/q", QString("\"%1\"").arg(path)});
#else
            QProcess::execute("rm", {"-rf", path});
#endif

            if (QDir(path).exists())
            {
                qWarning() << "Directory still exists after alternative deletion attempt";
                return false;
            }
        }
    }

    m_projects.removeOne(name);
    m_settings.remove(name + "/path");
    m_settings.setValue("projects", m_projects);

    emit projectsChanged();
    return success;
}

bool Model::renameProject(const QString& oldName, const QString& newName, QString* errorMessage)
{
    if (oldName.isEmpty() || newName.isEmpty()) {
        if (errorMessage) *errorMessage = "Имена проектов не могут быть пустыми";
        return false;
    }

    if (oldName == newName) {
        if (errorMessage) *errorMessage = "Новое имя совпадает со старым";
        return true;
    }

    if (!m_projects.contains(oldName)) {
        if (errorMessage) *errorMessage = QString("Проект '%1' не найден").arg(oldName);
        return false;
    }

    if (!isValidProjectName(newName)) {
        if (errorMessage) *errorMessage = "Недопустимое имя проекта. Разрешены: буквы, цифры, пробелы, дефисы и подчёркивания";
        return false;
    }

    if (m_projects.contains(newName)) {
        if (errorMessage) *errorMessage = QString("Проект с именем '%1' уже существует").arg(newName);
        return false;
    }

    QString oldPath = projectPath(oldName);
    if (oldPath.isEmpty()) {
        if (errorMessage) *errorMessage = "Не удалось определить путь к проекту";
        return false;
    }

    QDir parentDir = QFileInfo(oldPath).dir();
    QString newPath = parentDir.filePath(newName);

    if (QDir(newPath).exists()) {
        if (errorMessage) *errorMessage = QString("Папка '%1' уже существует").arg(newPath);
        return false;
    }

    QDir dir;
    if (!dir.rename(oldPath, newPath)) {
        QFile file(oldPath);
        if (!file.rename(newPath)) {
            QString error = file.errorString();
            if (errorMessage) *errorMessage = QString("Не удалось переименовать: %1").arg(error);
            return false;
        }
    }

    m_projects.replace(m_projects.indexOf(oldName), newName);
    m_settings.remove(oldName + "/path");
    m_settings.setValue(newName + "/path", newPath);
    m_settings.setValue("projects", m_projects);

    emit projectsChanged();
    return true;
}
void Model::openProject(const QString& name)
{
    if (m_projects.contains(name))
    {
        m_currentProject = name;
        emit projectOpened(name);
        emit chaptersChanged();
        emit foldersChanged();
    }
}

QString Model::currentProject() const
{
    return m_currentProject;
}

QString Model::projectPath(const QString& projectName) const
{
    return m_settings.value(projectName + "/path").toString();
}
