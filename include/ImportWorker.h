#pragma once

#include <QObject>
#include <QJsonArray>

class ImportWorker : public QObject
{
    Q_OBJECT

    public:
    explicit ImportWorker(QObject *parent = nullptr);

public slots:
    // fais le job de importFromFile de taskListModel
    void importFile(const QString &localPath);
    void generateTasks(int count);

signals:
    void imported(const QJsonArray &tasks);
    void failed(const QString &message);
    void progress(int percent);

};
