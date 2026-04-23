#include "GanttWidget.h"

#include <QPainter>

GanttWidget::GanttWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(2000, 100);
}
void GanttWidget::addSegment(QString id) {
    if (!segments.empty() && segments.back().processId == id) {
        segments.back().duration++;
        segments.back().endTime++;
    } else {
        int start = segments.empty() ? 0 : segments.back().endTime;
        // Ensure you match the struct order: {id, duration, start, end}
        segments.push_back({id, 1, start, start + 1});
    }

    int totalWidth = 0;
    for(const auto& s : segments) totalWidth += (s.duration * 50);
    setMinimumWidth(totalWidth + 50);

    update();
}
// Helper function to generate a uniue color for an ID
QColor GanttWidget::generateColor(QString id)
{
    if (id == "Idle")
        return QColor(200, 200, 200); // Light gray for idle
    // A list of bright, distinct colors that look good on white/light backgrounds
    static const QList<QColor> palette = {
    QColor("#FF7F7F"), // Light Red
        QColor("#7FFF7F"), // Light Green
        QColor("#7F7FFF"), // Light Blue
        QColor("#FFFF7F"), // Light Yellow
        QColor("#7FFFFF"), // Light Cyan
        QColor("#FF7FFF"), // Light Magenta
        QColor("#FFBF7F") // Light Orange
};

// Use the hash to pick an index from our predefined palette
int index = qAbs(qHash(id)) % palette.size();
return palette[index];
}
void GanttWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // 1. Create a bold font
    QFont boldFont = painter.font();
    boldFont.setBold(true);
    painter.setFont(boldFont);

    int x = 15;
    int y = 23;

    for (const auto& seg : segments) {
        if (!colorMap.contains(seg.processId)) {
            colorMap[seg.processId] = generateColor(seg.processId);
        }

        int width = seg.duration * 50;

        painter.setBrush(colorMap[seg.processId]);
        painter.setPen(Qt::black);
        painter.drawRect(x, y, width, 50);

        QString timeLabel = QString("%1-%2").arg(seg.startTime).arg(seg.endTime);

        // Draw Process ID
        if(seg.processId=="Idle")
        painter.drawText(QRect(x, y, width, 25), Qt::AlignCenter, seg.processId);
        else{
            painter.drawText(QRect(x, y, width, 25), Qt::AlignCenter, "P"+seg.processId);
        }

        // Draw Time Range
        painter.drawText(QRect(x, y + 25, width, 25), Qt::AlignCenter, timeLabel);

        x += width;
    }
}
void GanttWidget::clear()
{
    segments.clear(); // Clear the list of segments
    colorMap.clear(); // Clear the color cache so colors reset next time
    update(); // Trigger a repaint to show a blank widget
}
