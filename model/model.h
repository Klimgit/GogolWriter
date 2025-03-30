#pragma once

#include <QObject>
#include <QStringList>
#include <QSettings>
#include <QString>
#include <QMap>

class Model : public QObject
{
    Q_OBJECT

public:
    explicit Model(QObject* parent = nullptr);
    static QString getBaseProjectsDirectory();
    static bool isValidProjectName(const QString& name);

    QStringList projects() const;
    void addProject(const QString& name);
    bool removeProject(const QString& name);
    bool renameProject(const QString& oldName, const QString& newName, QString* errorMessage);

    void openProject(const QString& name);
    QString currentProject() const;

    QString projectPath(const QString& projectName) const;
signals:
    void projectsChanged();
    void projectOpened(const QString& name);
    void chaptersChanged();
    void foldersChanged();
    void languageChanged();
    void styleChanged();

private:
    QSettings m_settings;
    QStringList m_projects;
    QString m_currentProject;
};
