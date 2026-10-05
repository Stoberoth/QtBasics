#include "TaskListModel.h"
#include <QByteArray>
#include <algorithm>

TaskListModel::TaskListModel(QObject *parent) : QAbstractListModel(parent)
{
    loadFromFile();
    if(m_task.empty())
    {
        m_task = {{"Pas de data", false}};
        saveToFile();
    }
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
    const QString localPath = QUrl(path).toLocalFile();
    if(localPath.isEmpty())
    {
        setImportMessage("No file to import");
        return;
    }
    QFile jsonFile(localPath);
    if(!jsonFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        setImportMessage("No file to import");
        return;
    }
    QByteArray data = jsonFile.readAll();
    QJsonDocument taskJsonDocument = QJsonDocument::fromJson(data);
    if(!taskJsonDocument.isArray())
    {
        setImportMessage("File not imported : Wrong format");
        return;
    }
    QJsonArray array = taskJsonDocument.array();
    if(array.size() == 0)
    {
        setImportMessage("File not imported : Empty Array");
        return;
    }
    beginInsertRows(QModelIndex(),m_task.size(), m_task.size()+array.size()-1);
    for(int i = 0; i < array.size(); i++)
    {
        m_task.push_back(Task::fromJson(array.at(i).toObject()));
    }
    endInsertRows();
    setImportMessage("File imported with success");
    saveToFile();
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
