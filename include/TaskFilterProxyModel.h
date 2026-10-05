#pragma once
#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

class TaskFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(FilterMode filterMode READ filterMode WRITE setFilterMode NOTIFY filterModeChanged)

public:
    explicit TaskFilterProxyModel(QObject* parent = nullptr);
    enum FilterMode{
        All,
        Active,
        Completed
    };
    // pour que filter mode sois connu de QML
    Q_ENUM(FilterMode)
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    FilterMode filterMode() const;
    void setFilterMode(FilterMode mode);

    Q_INVOKABLE void toggleTask(int proxyRow);
    Q_INVOKABLE void removeTask(int proxyRow);
    Q_INVOKABLE void moveTask(int proxyRow, int proxyRowDest);
    Q_INVOKABLE void importFromFile(const QString &path);

signals:
    void filterModeChanged();

private:
    FilterMode m_filterMode = FilterMode::All;

};
