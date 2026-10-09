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
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)

public:
    explicit DashboardController(QObject *parent = nullptr );

    QString title() const;
    int progress() const;
    QString message() const;
    TaskListModel* taskModel() const;
    void setTitle(const QString &title);
    void setProgress(const int newProgress);
    void setMessage(const QString &message);
    void setTaskModel(TaskListModel *model);
    Q_INVOKABLE void incrementProgress();
    Q_INVOKABLE bool addRequested(const QString &title);

signals:
    void titleChanged();
    void progressChanged();
    void taskModelChanged();
    void messageChanged();

private:
    QString m_title;
    int m_progress = 0;
    TaskListModel* m_taskModel = nullptr;
    QString m_message;

    void updateProgress();
};
