#include "ImportWorker.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <stdio.h>
#include <stdlib.h>


ImportWorker::ImportWorker(QObject *parent): QObject(parent)
{

}

void ImportWorker::importFile(const QString &localPath)
{
    QFile jsonFile(localPath);
    if(!jsonFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        emit failed("No file to import");
        return;
    }
    QByteArray data = jsonFile.readAll();
    QJsonDocument taskJsonDocument = QJsonDocument::fromJson(data);
    if(!taskJsonDocument.isArray())
    {
        emit failed("File not imported : Wrong format");
        return;
    }
    QJsonArray array = taskJsonDocument.array();
    emit imported(array);
}

void ImportWorker::generateTasks(int count)
{
    if(count <= 0)
    {
        emit failed("Rien à générer");
        return;
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
            emit(progress(i*100 / count));
            QThread::msleep(30);
        }
    }
    emit progress(100);
    emit imported(array);
}
