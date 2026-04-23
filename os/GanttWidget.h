#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QColor>
#include <QMap>
#include <QPaintEvent>

// 1. Define the struct here so it is known to the compiler
struct GanttSegment {
    QString processId;
    int duration;
    int startTime;
    int endTime;
};

class GanttWidget : public QWidget {
    Q_OBJECT // Required for Qt widgets
public:
    explicit GanttWidget(QWidget *parent = nullptr);
    void addSegment(QString id);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor generateColor(QString id);

    QVector<GanttSegment> segments; // Use the struct name
    QMap<QString, QColor> colorMap;
};

#endif // GANTTWIDGET_H
