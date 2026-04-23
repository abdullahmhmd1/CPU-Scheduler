#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"scheduler.cpp"
#include"GanttWidget.h"
#include<QTableWidget>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    Ui::MainWindow *ui;
    QTimer *timer;
    Scheduler scheduler;
    QString algo;
    QTableWidget *tableWidget; // Ensure this matches your UI
    GanttWidget *ganttWidget; // Change the type from QWidget to GanttWidget
    void calculateAndShowMetrics();
    void updateTableVisuals(int time);
    void on_algoBox_changed(const QString &text);


private slots:
    void on_btnAdd_clicked();
    void on_tick(); // Declare the slot here
    //void on_startBtn_clicked();
    void on_resetBtn_clicked();
    void on_startLiveBtn_clicked();
    void on_startStaticBtn_clicked();
};
#endif // MAINWINDOW_H
