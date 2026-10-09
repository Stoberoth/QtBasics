#include <QObject>
#include <QTest>
#include "TaskListModel.h"


class TestTaskListModel : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void addTaskIncreasesRowCount();
    void emptyTitleIsRejected();
    void spyRowsInserted();
    void modelTesterFollowsMoves();
};

QTEST_GUILESS_MAIN(TestTaskListModel)
#include "test_TaskListModel.moc"
#include <QSignalSpy>
#include <QAbstractItemModelTester>

void TestTaskListModel::initTestCase()
{
    QCoreApplication::setOrganizationName("QtBasicsTests");
    QCoreApplication::setApplicationName("TaskListModelTests");
}

void TestTaskListModel::addTaskIncreasesRowCount()
{
    TaskListModel model;
    int previousCount = model.rowCount();
    model.addTask("Pains");
    QVERIFY(model.rowCount() == previousCount + 1);
    QCOMPARE(model.data(model.index(0), TaskListModel::TitleRole), "Pains");
}

void TestTaskListModel::emptyTitleIsRejected()
{
    TaskListModel model;
    int previousCount = model.rowCount();
    model.addTask("   ");
    QCOMPARE(model.rowCount(), previousCount);
}
void TestTaskListModel::spyRowsInserted()
{
    TaskListModel model;
    QSignalSpy spy(&model, &QAbstractListModel::rowsInserted);
    model.addTask("Test");
    QCOMPARE(spy.count(), 1);
}
void TestTaskListModel::modelTesterFollowsMoves()
{
    TaskListModel model;
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
    model.addTask("A");
    model.addTask("B");
    model.moveTask(0, 1);
    model.removeTask(0);
}
