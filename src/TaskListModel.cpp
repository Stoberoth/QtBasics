#include "TaskListModel.h"
#include "ImportWorker.h"
#include <QByteArray>
#include <algorithm>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QNetworkRequest>
#include <QNetworkReply>

TaskListModel::TaskListModel(QObject *parent) : QAbstractListModel(parent)
{
    m_worker = new ImportWorker();
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    m_network = new QNetworkAccessManager(this);

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

void TaskListModel::fetchFromNetwork()
{
    if(m_workerBusy == true)
    {
        setImportMessage("Déjà en cours d'import");
        return;
    }
    m_workerBusy=true;
    QNetworkRequest request(QUrl("https://jsonplaceholder.typicode.com/todos?_limit=20"));
    request.setTransferTimeout(5000);
    QNetworkReply *reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]
        {
            if(reply->error() != QNetworkReply::NoError)
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage(reply->errorString());
                return;
            }
            if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200)
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage(QString("Status Code not allowed"));
                return;
            }
            QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
            if(!document.isArray())
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage("Ce JSon n'est pas un array");
                return;
            }
            QJsonArray array = document.array();
            applyImport(array);
            reply->deleteLater();
            setImportMessage(QString("All Green"));
        });
}

void TaskListModel::postTask(const QString &title)
{
    if(title.trimmed().isEmpty())
    {
        setImportMessage("Titre vide");
        return;
    }
    if(m_workerBusy)
    {
        setImportMessage("Déjà au boulot");
        return;
    }
    m_workerBusy = true;
    QJsonObject body;
    body["title"] = title.trimmed();
    body["completed"] = false;
    body["userId"] = 1;

    QNetworkRequest request(QUrl("https://jsonplaceholder.typicode.com/todos"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    QNetworkReply *reply = m_network->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]
        {
            if(reply->error() != QNetworkReply::NoError)
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage(reply->errorString());
                return;
            }
            if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 201)
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage("Post non résolu pas de création");
                return;
            }
            QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
            if(!document.isObject())
            {
                m_workerBusy = false;
                reply->deleteLater();
                setImportMessage("Json reçu pas un objet");
                return;
            }
            addTask(document.object().value("title").toString());
            m_workerBusy = false;
            reply->deleteLater();
        });
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
    m_repository.saveToFile(taskJsonArray);
}

void TaskListModel::loadFromFile()
{
    QJsonArray taskJsonArray = m_repository.loadFromFile();
    if(taskJsonArray.isEmpty())
    {
        return;
    }
    beginResetModel();
    m_task.clear();
    for(int i = 0; i < taskJsonArray.size(); i++)
    {
        m_task.push_back(Task::fromJson(taskJsonArray.at(i).toObject()));
    }
    endResetModel();
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
