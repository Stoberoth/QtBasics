#include <qqmlregistration.h>

#include <QQuickPaintedItem>
#include <QPainter>
#include <QMouseEvent>

class HabitGauge : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit HabitGauge(QQuickItem *parent = nullptr);
    qreal progress();
    void setProgress(qreal progress);
    void paint(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void progressChanged();

private:
    qreal m_progress = 0;
};
