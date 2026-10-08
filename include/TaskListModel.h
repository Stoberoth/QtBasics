#pragma once
#include <QString>
#include <vector>
#include <QAbstractListModel>
#include <qqmlregistration.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include "ImportWorker.h"

#include <QFutureWatcher>

struct Task
{
    QString title;
    bool completed = false;

    // Serialization
    // transforme une Task en JsonObject
    QJsonObject toJson() const
    {
        QJsonObject task_json;
        task_json["title"] = title;
        task_json["completed"] = completed;
        return task_json;
    }
    // transforme un JsonObject en Task
    static Task fromJson(const QJsonObject &json)
    {
        Task new_task;
        new_task.title = json["title"].toString();
        new_task.completed = json["completed"].toBool();
        return new_task;
    }
};

class TaskListModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TaskListModel(QObject *parent = nullptr);
    ~TaskListModel();

    enum TaskRoles
    {
        TitleRole = Qt::UserRole + 1,
        CompletedRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_PROPERTY(QString importMessage READ importMessage NOTIFY importMessageChanged)
    Q_PROPERTY(int importProgress READ importProgress NOTIFY importProgressChanged)

    Q_INVOKABLE void addTask(const QString &title);
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE void toggleTask(int index);
    Q_INVOKABLE void moveTask(int from, int to);
    Q_INVOKABLE void importFromFile(const QString &path);
    Q_INVOKABLE void importButton();

    QString importMessage();
    void setImportMessage(const QString &message);

    int importProgress();
    void setImportProgress(int importProgress);

signals:
    void importMessageChanged();
    void importedRequested(const QString &path);
    void importProgressChanged();
    void importBigFile(int count);

private:
    std::vector<Task> m_task;
    QString m_message;
    int m_importProgress = 0;

    bool m_workerBusy = false;

    ImportWorker *m_worker;
    QThread *m_thread;

    QFutureWatcher<QJsonArray> m_watcher;

    QString getStorageFilePath() const;
    void saveToFile() const;
    void loadFromFile();

    void applyImport(const QJsonArray &tasks);
    void onImportFailed(const QString &message);
};
