#pragma once
#include <QObject>
#include <TaskListModel.h>

#include <qqmlregistration.h>

class DashboardController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(TaskListModel* taskModel READ taskModel WRITE setTaskModel NOTIFY taskModelChanged)
public:
    explicit DashboardController(QObject *parent = nullptr );

    QString title() const;
    int progress() const;
    TaskListModel* taskModel() const;
    void setTitle(const QString &title);
    void setProgress(const int newProgress);
    void setTaskModel(TaskListModel *model);
    Q_INVOKABLE void incrementProgress();

signals:
    void titleChanged();
    void progressChanged();
    void taskModelChanged();

private:
    QString m_title;
    int m_progress = 0;
    TaskListModel* m_taskModel = nullptr;

    void updateProgress();
};
