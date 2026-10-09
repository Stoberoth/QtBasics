#pragma once
// classe pour load and save les tasks dans des fichiers ou un bdd
#include <QString>
#include <QJsonArray>

class TaskRepository
{
public:
    explicit TaskRepository();

    QString getStorageFilePath() const;
    QJsonArray loadFromFile() const;
    void saveToFile(const QJsonArray &array) const;
};
