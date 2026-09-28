#include "TaskListModel.h"

TaskListModel::TaskListModel(QObject *parent) : QAbstractListModel(parent), m_task({{"Vaisselle",false}, {"Cuisine", false}})
{

}

int TaskListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    return static_cast<int>(m_task.size());
}
QVariant TaskListModel::data(const QModelIndex &index, int role) const
{
    // Garde fou pour ne pas sortir de la table ou avoir un mauvais index d'utilisé
    if(!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_task.size()))
        return QVariant();

    // On récupére la tache voulu par l'index
    const Task &task = m_task[index.row()];

    switch (role) {
        case TitleRole:
            return task.title;
        case CompletedRole:
            return task.completed;
        default:
            return QVariant();
    }
}
QHash<int, QByteArray> TaskListModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {CompletedRole, "completed"},
    };
}

void TaskListModel::addTask(const QString &title)
{
    if(title.trimmed().isEmpty()) return;
    int newRow = static_cast<int>(m_task.size());

    // on avertis Qt qu'on va insérer une ligne dans le model
    beginInsertRows(QModelIndex(),newRow, newRow);

    // on modifie la data
    m_task.push_back(Task{title, false});
    // on averti Qt que l'on a fini d'inserer
    endInsertRows();
}

void TaskListModel::removeTask(int index)
{
    if(index < 0 || index >= static_cast<int>(m_task.size()))
        return ;
    beginRemoveRows(QModelIndex(), index, index);
    m_task.erase(m_task.begin() + index);
    endRemoveRows();
}

void TaskListModel::toggleTask(int index)
{
    if(index < 0 || index >= static_cast<int>(m_task.size())) return;

    m_task[index].completed = !m_task[index].completed;

    QModelIndex modelIndex = this->index(index, 0);

    emit dataChanged(modelIndex, modelIndex, {CompletedRole});
}
