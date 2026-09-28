#include "TaskFilterProxyModel.h"
#include "TaskListModel.h"

TaskFilterProxyModel::TaskFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{

}

TaskFilterProxyModel::FilterMode TaskFilterProxyModel::filterMode() const
{
    return m_filterMode;
}

void TaskFilterProxyModel::setFilterMode(FilterMode mode)
{
    if(m_filterMode == mode)
    {
        return;
    }
    beginFilterChange();
    m_filterMode = mode;
    endFilterChange();
    emit filterModeChanged();
}

bool TaskFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_filterMode == FilterMode::All)
    {
        return true;
    }

    if(!sourceModel())
        return true;

    // on cherche dans le model source la vrai valeur
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    bool isCompleted = sourceModel()->data(sourceIndex, TaskListModel::CompletedRole).toBool();

    if(m_filterMode == FilterMode::Active)
    {
        return !isCompleted;
    }
    if (m_filterMode == FilterMode::Completed) {
        return isCompleted;
    }
    return true;
}

void TaskFilterProxyModel::toggleTask(int proxyRow)
{
    QModelIndex sourceIndex = mapToSource(this->index(proxyRow, 0));
    static_cast<TaskListModel*>(sourceModel())->toggleTask(sourceIndex.row());
    beginFilterChange();
    endFilterChange();
}

void TaskFilterProxyModel::removeTask(int proxyRow)
{
    QModelIndex sourceIndex = mapToSource(this->index(proxyRow, 0));
    static_cast<TaskListModel*>(sourceModel())->removeTask(sourceIndex.row());
}
