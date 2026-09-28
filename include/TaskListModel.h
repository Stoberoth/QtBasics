#pragma once
#include <QString>
#include <vector>
#include <QAbstractListModel>
#include <qqmlregistration.h>

struct Task
{
    QString title;
    bool completed = false;
};

class TaskListModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TaskListModel(QObject *parent = nullptr);

    enum TaskRoles
    {
        TitleRole = Qt::UserRole + 1,
        CompletedRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addTask(const QString &title);
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE void toggleTask(int index);

private:
    std::vector<Task> m_task;
};
