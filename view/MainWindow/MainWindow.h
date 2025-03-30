#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

signals:
    void createProjectRequested(const QString& name);
    void openProjectWindowRequested(const QString& name);
    void renameProjectRequested(const QString& oldName, const QString& newName);
    void deleteProjectRequested(const QString& name);

public slots:
    void setProjects(const QStringList& projects) const;

private slots:
    void onCreateProjectClicked();
    void onProjectItemClicked(QListWidgetItem* item);
    void showContextMenu(const QPoint& pos) const;
    void renameProject();
    void deleteProject();

private:
    QListWidget* m_projectList;
    QPushButton* m_createButton;
    QComboBox* m_languageCombo;
    QMenu* m_contextMenu;
};
