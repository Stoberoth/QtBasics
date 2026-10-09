#include "DashboardController.h"
#include "TaskListModel.h"

DashboardController::DashboardController(QObject *parent) : QObject(parent), m_title(QStringLiteral("Mon Objectif Quotidien")), m_progress(0)
{
}

void DashboardController::setTitle(const QString &title)
{
    if (m_title == title) {
        return;
    }
    m_title = title;
    emit titleChanged();
}

void DashboardController::setProgress(const int progress)
{
    if(m_progress == progress)
    {
        return;
    }
    m_progress = progress;
    emit progressChanged();
}

void DashboardController::setTaskModel(TaskListModel *model)
{
    if(m_taskModel == model) return;

    if (m_taskModel)
        disconnect(m_taskModel, nullptr, this, nullptr);
    m_taskModel = model;
    if(m_taskModel){
        connect(m_taskModel, &QAbstractItemModel::rowsInserted, this, &DashboardController::updateProgress);
        connect(m_taskModel, &QAbstractItemModel::dataChanged, this, &DashboardController::updateProgress);
        connect(m_taskModel, &QAbstractListModel::rowsRemoved, this, &DashboardController::updateProgress);
    }
    updateProgress();
    emit taskModelChanged();
}

QString DashboardController::title() const
{
    return m_title;
}

int DashboardController::progress() const
{
    return m_progress;
}

TaskListModel* DashboardController::taskModel() const
{
    return m_taskModel;
}

void DashboardController::incrementProgress()
{
    setProgress(std::min(m_progress + 10, 100));
}

void DashboardController::updateProgress()
{
    if(!m_taskModel || m_taskModel->rowCount() == 0)
    {
        setProgress(0);
        return;
    }
    int task_completed = 0;
    for(int i = 0; i < m_taskModel->rowCount(); i++)
    {
        QModelIndex index = m_taskModel->index(i, 0);

        QVariant data = m_taskModel->data(index, TaskListModel::CompletedRole);

        if(data.toBool() == true)
            task_completed ++;
    }
    setProgress((task_completed * 100 / m_taskModel->rowCount()));
}

bool DashboardController::addRequested(const QString &title)
{
    if(title.trimmed().isEmpty())
    {
        setMessage("Titre non valide");
        return false;
    }
    setMessage("");
    if(m_taskModel == nullptr)
    {
        return false;
    }
    m_taskModel->addTask(title);
    return true;
}

QString DashboardController::message() const
{
    return m_message;
}

void DashboardController::setMessage(const QString &message)
{
    m_message = message;
    emit messageChanged();
}
