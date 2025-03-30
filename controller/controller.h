#pragma once

#include <QObject>
#include <QMap>
#include "../model/model.h"
#include "../view/MainWindow/MainWindow.h"
#include "../view/ProjectWindow/ProjectWindow.h"

class MainController : public QObject
{
    Q_OBJECT

public:
    explicit MainController(Model* model, MainWindow* view, QObject* parent = nullptr);
    ~MainController() override;

private slots:
    void onCreateProjectRequested(const QString& name) const;
    void onOpenProjectWindowRequested(const QString& name);
    void onRenameProjectRequested(const QString& oldName, const QString& newName) const;
    void onDeleteProjectRequested(const QString& name) const;

private:
    Model* m_model;
    MainWindow* m_view;
    QMap<QString, ProjectWindow*> m_projectWindows;
};
