#include "TaskListModel.h"
#include "ImportWorker.h"
#include <QByteArray>
#include <algorithm>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

TaskListModel::TaskListModel(QObject *parent) : QAbstractListModel(parent)
{
    m_worker = new ImportWorker();
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);


    connect(this, &TaskListModel::importedRequested, m_worker, &ImportWorker::importFile);
    connect(m_worker, &ImportWorker::imported, this, &TaskListModel::applyImport);
    connect(m_worker, &ImportWorker::failed, this, &TaskListModel::onImportFailed);
    connect(this, &TaskListModel::importBigFile, m_worker, &ImportWorker::generateTasks);
    connect(m_worker, &ImportWorker::progress, this, &TaskListModel::setImportProgress);
    connect(&m_watcher, &QFutureWatcher<QJsonArray>::finished, this, [this]{
        if(m_watcher.isCanceled())
        {
            return;
        }
        setImportProgress(100);
        applyImport(m_watcher.result());
    });

    m_thread->start();
    loadFromFile();
    if(m_task.empty())
        setImportMessage("Pas de données trouvées");
}
TaskListModel::~TaskListModel()
{
    m_watcher.cancel();
    m_watcher.waitForFinished();
    m_thread->quit();
    m_thread->wait();
    delete m_worker;
}

QString TaskListModel::importMessage()
{
    return m_message;
}
void TaskListModel::setImportMessage(const QString &message)
{
    m_message = message;
    emit importMessageChanged();
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
    beginInsertRows(QModelIndex(),0, 0);

    // on modifie la data
    //m_task.push_back(Task{title, false});
    m_task.insert(m_task.begin(), Task{title, false});
    // on averti Qt que l'on a fini d'inserer
    endInsertRows();
    saveToFile();
}

void TaskListModel::removeTask(int index)
{
    if(index < 0 || index >= static_cast<int>(m_task.size()))
        return ;
    beginRemoveRows(QModelIndex(), index, index);
    m_task.erase(m_task.begin() + index);
    endRemoveRows();
    saveToFile();
}

void TaskListModel::toggleTask(int index)
{
    if(index < 0 || index >= static_cast<int>(m_task.size())) return;

    m_task[index].completed = !m_task[index].completed;

    QModelIndex modelIndex = this->index(index, 0);

    emit dataChanged(modelIndex, modelIndex, {CompletedRole});
    saveToFile();
}

QString TaskListModel::getStorageFilePath() const
{
    // On cherche le dossier pour enregistrer les données
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    // Si le dossier n'existe pas on le créer
    QDir dir(dirPath);
    if(!dir.exists())
    {
        // avec le nom de ce projet
        dir.mkpath(".");
    }
    // on retourne le chemin complet du fichier que l'on créer ici
    return dir.filePath("task.json");
}

void TaskListModel::moveTask(int from, int to)
{
    if(from == to) return;

    if (to >= this->rowCount() || to < 0) return;

    int destinationChild = (from < to) ? to + 1 : to;

    if(!beginMoveRows(QModelIndex(), from, from, QModelIndex(), destinationChild)) return;

    if(from < to)
    {
        std::rotate(m_task.begin() + from, m_task.begin() + from + 1, m_task.begin() + to + 1);
    }
    else {
        std::rotate(m_task.begin() + to, m_task.begin() + from, m_task.begin() + from + 1);
    }
    endMoveRows();
    saveToFile();


}

void TaskListModel::importFromFile(const QString &path)
{
    if(m_workerBusy)
    {
        setImportMessage("Un autre import est déjà en cours");
        return;
    }
    const QString localPath = QUrl(path).toLocalFile();
    if(localPath.isEmpty())
    {
        setImportMessage("No file to import");
        return;
    }
    setImportMessage("Import en cours ....");
    m_workerBusy = true;
    emit importedRequested(localPath);
}


void TaskListModel::saveToFile() const
{
    QJsonArray taskJsonArray;
    for(Task t : m_task)
    {
        taskJsonArray.append(t.toJson());
    }
    QJsonDocument taskJsonDocument(taskJsonArray);
    QFile jsonFile(getStorageFilePath());
    if(!jsonFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return;
    }
    jsonFile.write(taskJsonDocument.toJson());
    jsonFile.close();
}

void TaskListModel::loadFromFile()
{
    QString filePath = getStorageFilePath();
    if(!QFile::exists(filePath))
    {
        return;
    }
    QFile taskJsonFile(filePath);
    if(!taskJsonFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return;
    }
    QByteArray data = taskJsonFile.readAll();
    QJsonDocument taskJsonDocument = QJsonDocument::fromJson(data);
    if(taskJsonDocument.isArray())
    {
        beginResetModel();
        m_task.clear();
        QJsonArray array = taskJsonDocument.array();
        for(int i = 0; i < array.size(); i++)
        {
            m_task.push_back(Task::fromJson(array.at(i).toObject()));
        }
        endResetModel();
    }
}

void TaskListModel::applyImport(const QJsonArray &tasks)
{
    m_workerBusy = false;
    if(tasks.empty()) {
        setImportMessage("Json Array empty");
        return;
    }

    beginInsertRows(QModelIndex(), m_task.size(), m_task.size()+tasks.size()-1);
    for(int i = 0; i < tasks.size(); i++)
    {
        m_task.push_back(Task::fromJson(tasks.at(i).toObject()));
    }
    endInsertRows();
    setImportMessage("Import réussi");
    saveToFile();
}

void TaskListModel::onImportFailed(const QString &message)
{
    m_workerBusy = false;
    setImportMessage(message);
}

int TaskListModel::importProgress()
{
    return m_importProgress;
}

void TaskListModel::setImportProgress(int importProgress)
{
    m_importProgress = importProgress;
    emit importProgressChanged();
}


QJsonArray buildGeneratedTasks(int count)
{
    if(count <= 0)
    {
        return QJsonArray();
    }

    QJsonArray array;
    for(int i = 0; i < count; ++i)
    {
        QJsonObject task;
        task["title"] = QString("Task %1").arg(i+1);
        task["completed"] = false;
        array.append(task);
        if(i % 1000 == 0)
        {
            QThread::msleep(3);
        }
    }
    return array;
}

void TaskListModel::importButton()
{
    if(m_workerBusy) return;
    m_workerBusy = true;
    // emit(importBigFile(50000));
    QFuture<QJsonArray> future = QtConcurrent::run([] {
        return buildGeneratedTasks(50000);
    });
    m_watcher.setFuture(future);
}
