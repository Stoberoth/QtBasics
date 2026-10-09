#include "TaskRepository.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

TaskRepository::TaskRepository()
{

}

QString TaskRepository::getStorageFilePath() const
{
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

void TaskRepository::saveToFile(const QJsonArray &array) const
{
    QJsonDocument taskJsonDocument(array);
    QFile jsonFile(getStorageFilePath());
    if(!jsonFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return;
    }
    jsonFile.write(taskJsonDocument.toJson());
    jsonFile.close();
}

QJsonArray TaskRepository::loadFromFile() const
{
    QString filePath = getStorageFilePath();
    if(!QFile::exists(filePath))
    {
        return QJsonArray();
    }
    QFile taskJsonFile(filePath);
    if(!taskJsonFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return QJsonArray();
    }
    QByteArray data = taskJsonFile.readAll();
    QJsonDocument taskJsonDocument = QJsonDocument::fromJson(data);
    if(!taskJsonDocument.isArray())
    {
        return QJsonArray();
    }
    return taskJsonDocument.array();
}
