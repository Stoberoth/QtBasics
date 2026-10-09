#include "HabitGauge.h"
#include <QPen>

HabitGauge::HabitGauge(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setImplicitHeight(80);
    setImplicitWidth(80);
}

qreal HabitGauge::progress()
{
    return m_progress;
}

void HabitGauge::setProgress(qreal progress)
{
    if(progress == m_progress)
    {
        return;
    }
    m_progress = progress;
    emit progressChanged();
    update();
}

void HabitGauge::paint(QPainter *painter)
{
    QPen penCompleted("#89b4fa");
    int penBrushSize = 8;
    penCompleted.setWidth(8);
    penCompleted.setCapStyle(Qt::RoundCap);
    QPen penBack("#45475a");
    penBack.setWidth(8);
    penBack.setCapStyle(Qt::RoundCap);
    painter->setPen(penBack);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QRectF rect = boundingRect();
    rect.adjust(penBrushSize / 2, penBrushSize / 2, -penBrushSize / 2, -penBrushSize / 2);

    const int start = 90 * 16;
    const int full = -360 * 16;
    const int span = -qRound(m_progress / 100.0 * 360 * 16);
    painter->drawArc(rect, start, full);
    painter->setPen(penCompleted);
    painter->drawArc(rect, start, span);
}
