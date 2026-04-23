#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include"scheduler.cpp"
#include <QTableWidgetItem>
#include <vector>
#include<QTimer>
#include<QMessageBox>
#include <iostream>
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::on_tick);

    // Initial table setup

    ui->tableWidget->setColumnCount(4);

    ui->tableWidget->setHorizontalHeaderLabels({"ID", "Arrival", "Remaining burst", "Priority"});

    // Connect the button to the slot

    connect(ui->addBtn, &QPushButton::clicked, this, &MainWindow::on_btnAdd_clicked);

    ganttWidget = new GanttWidget(this); // Initialize your custom widget
    ui->ganttContainerLayout->addWidget(ganttWidget);
    connect(ui->algoBox, &QComboBox::currentTextChanged, this, &MainWindow::on_algoBox_changed);

    // Initialize UI state
    on_algoBox_changed(ui->algoBox->currentText());
}


void MainWindow::calculateAndShowMetrics() {
    // 1. Output the total values to the console
    std::cout << "Total Waiting Time: " << scheduler.totalWaitingTime << std::endl;
    std::cout << "Total Turnaround Time: " << scheduler.totalTurnaround << std::endl;

    // 2. Existing UI updates
    double avgWT = (scheduler.processes.size() > 0) ? (double)scheduler.totalWaitingTime / scheduler.processes.size() : 0.0;
    double avgTAT = (scheduler.processes.size() > 0) ? (double)scheduler.totalTurnaround / scheduler.processes.size() : 0.0;

    ui->avgWTLabel->setText("Avg Wait: " + QString::number(avgWT, 'f', 2));
    ui->avgTATLabel->setText("Avg Turnaround: " + QString::number(avgTAT, 'f', 2));
}
void MainWindow::on_btnAdd_clicked() {
    // 1. Get values and validate (basic check)
    if (ui->idInput->text().isEmpty() || ui->burstInput->text().isEmpty()|| ui->arrivalInput->text().isEmpty()) return;

    int id = ui->idInput->text().toInt();
    int arrival = ui->arrivalInput->text().toInt();
    int burst = ui->burstInput->text().toInt();
    int priority = ui->priorityInput->text().toInt();

    // 2. Create new Process instance and push to vector
    Process newProcess(id, arrival, burst, priority);
    scheduler.processes.push_back(make_unique<Process>(id, arrival, burst, priority));

    // 3. Update Table (UI)
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(arrival)));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(burst)));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(priority)));

    // Clear inputs
    ui->idInput->clear();
    ui->arrivalInput->clear();
    ui->burstInput->clear();
    ui->priorityInput->clear();
}
MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_tick() {
    if (scheduler.processes.size()!=scheduler.Finshedprocesses.size()) {

        if (algo == "FCFS") {
           scheduler.runFCFS();
        }
        else if (algo == "RR") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.quantum = ui->quantumInput->text().toInt();
            // Print the quantum value to console for debugging
            scheduler.runRRPreemptive();
        }
        else if (algo == "SJF Non Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
              scheduler.runSFJNonPreemptive();
        }
        else if (algo == "SJF Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
             scheduler.runSJFPreemptive();
        }
        else if (algo == "Priority Non Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            /*QString message = QString("size: %1").arg(scheduler.processes.size());
            QMessageBox::information(this, "Process Info", message);*/
            scheduler.runPriorityNonPreemptive();

        }
        else if (algo == "Priority Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.runPriorityPreemptive();
        }

        scheduler.currentTime++;

        updateTableVisuals(scheduler.currentTime);
        if (scheduler.currentRunningProcess != nullptr) {
            ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
        } else if(scheduler.processes.size()!=scheduler.Finshedprocesses.size()) {
            ganttWidget->addSegment("Idle");
        }
    }
    else{
    ui->quantumInput->clear();
    calculateAndShowMetrics();
    return;}
}


void MainWindow::updateTableVisuals(int time) {
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        // 1. Get the ID from the UI table
        int id = ui->tableWidget->item(i, 0)->text().toInt();

        // 2. Find the process in the scheduler
        Process* p = nullptr;
        for (const auto& procPtr : scheduler.processes) {
            if (procPtr->id == id) {
                p = procPtr.get();
                break;
            }
        }

        if (!p) continue;

        // 3. Calculate remaining time
        // If the process hasn't arrived yet, remaining is burstTime
        // If it's finished, remaining is 0
        // If it's running/waiting, use its current remainingBurstTime
        int remaining = 0;

        if (time < p->arrivalTime) {
            remaining = p->burstTime;
        } else if (p->remainingBurstTime > 0) {
            remaining = p->remainingBurstTime;
        } else {
            remaining = 0;
        }

        // 4. Update the UI table (Column 2 is Burst Time)
        QTableWidgetItem* item = ui->tableWidget->item(i, 2);
        if (item) {
            item->setText(QString::number(remaining));
        }
    }
}

void MainWindow::on_resetBtn_clicked()
{
    // 1. Stop the simulation timer
    timer->stop();

    // 2. Clear scheduler internal data
    // Assuming your scheduler class has clear() or similar members
    scheduler.processes.clear();
    scheduler.Finshedprocesses.clear();
    scheduler.currentTime = 0;
    scheduler.currentRunningProcess = nullptr;
    scheduler.totalWaitingTime = 0;
    scheduler.totalTurnaround = 0;

    // 3. Clear the UI Table
    ui->tableWidget->setRowCount(0);

    // 4. Reset labels
    ui->avgWTLabel->setText("Avg Waiting Time: 0");
    ui->avgTATLabel->setText("Avg Turnaround Time: 0");

    // 5. Reset Gantt Chart
    // Assuming your GanttWidget has a clear() method
    ganttWidget->clear();

    // Optional: Enable inputs again if you disable them during Start
    ui->addBtn->setEnabled(true);
    ui->startLiveBtn->setEnabled(true);
}

//60% contrbution

void MainWindow::on_startLiveBtn_clicked()
{
    algo = ui->algoBox->currentText();
    timer->start(1000);
}


void MainWindow::on_startStaticBtn_clicked()
{
    algo = ui->algoBox->currentText();
    while (1) {

        if (algo == "FCFS") {
            scheduler.runFCFS();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }

        }
        else if (algo == "RR") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.quantum   = ui->quantumInput->text().toInt();
            scheduler.runRRPreemptive();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }

        }
        else if (algo == "SJF Non Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.runSFJNonPreemptive();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }
        }
        else if (algo == "SJF Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.runSJFPreemptive();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }
        }
        else if (algo == "Priority Non Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            /*QString message = QString("size: %1").arg(scheduler.processes.size());
            QMessageBox::information(this, "Process Info", message);*/
            scheduler.runPriorityNonPreemptive();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }

        }
        else if (algo == "Priority Preemptive") {
            // Note: Use .toInt() to convert string input to an integer for the quantum
            scheduler.runPriorityPreemptive();
            updateTableVisuals(scheduler.currentTime);
            if (scheduler.Finshedprocesses.size() == scheduler.processes.size()) break;
            scheduler.currentTime++;
            if (scheduler.currentRunningProcess != nullptr) {
                ganttWidget->addSegment(QString::number(scheduler.currentRunningProcess->id));
            } else{
                ganttWidget->addSegment("Idle");
            }
        }





    }
        calculateAndShowMetrics();
        ui->quantumInput->clear();
        return;
}
void MainWindow::on_algoBox_changed(const QString &text) {
    // Logic: Only Round Robin needs the Quantum field
    bool isRR = (text == "RR");
    ui->quantumInput->setEnabled(isRR);

    // Logic: Priority algorithms need the Priority field
    bool isPriority = (text == "Priority Non Preemptive" || text == "Priority Preemptive");
    ui->priorityInput->setEnabled(isPriority);

    // Optional: Clear fields when disabled to avoid confusion
    if (!isRR) ui->quantumInput->clear();
    if (!isPriority) ui->priorityInput->clear();
}

