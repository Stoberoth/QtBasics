#include "HabitGauge.h"
#include <QPen>
#include <cmath>
#include <QDebug>
#include <cstdlib>

HabitGauge::HabitGauge(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setImplicitHeight(80);
    setImplicitWidth(80);
    setAcceptedMouseButtons(Qt::LeftButton);
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
    rect.adjust(penBrushSize / 2.0, penBrushSize / 2.0, -penBrushSize / 2.0, -penBrushSize / 2.0);

    const int start = 90 * 16;
    const int full = -360 * 16;
    const int span = -qRound(m_progress / 100.0 * 360 * 16);
    painter->drawArc(rect, start, full);
    painter->setPen(penCompleted);
    painter->drawArc(rect, start, span);
}

void HabitGauge::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    qreal dx = pos.x() - width() /2;
    qreal dy = pos.y() - height() / 2;
    qreal distFromCenter = std::hypot(dx, dy);
    if(abs(distFromCenter - ((width() / 2) - 4)) > 8)
    {
        event->ignore();
        return;
    }
    event->accept();
    qreal degrees = std::atan2(dy, dx) * 180.0 / M_PI;

    degrees += 90;
    if(degrees < 0)
    {
        degrees += 360;
    }
    setProgress(degrees / 360 * 100.0);
}
