#include <bits/stdc++.h>
#include "Process.cpp"
using namespace std;
struct CompareProcess
{
    bool operator()(const Process *p1, const Process *p2)
    {
        // Tie-breaker: If priorities are equal, use Arrival Time (FCFS)
        if (p1->priority == p2->priority)
        {
            return p1->arrivalTime > p2->arrivalTime;
        }

        // Lower number = Higher priority
        return p1->priority > p2->priority;
    }
};

struct CompareSJF
{
    bool operator()(const Process *p1, const Process *p2)
    {
        // Tie-breaker: If burstTime are equal, use Arrival Time (FCFS)
        if (p1->burstTime  == p2->burstTime )
        {
            return p1->arrivalTime > p2->arrivalTime;
        }

        return p1->burstTime  > p2->burstTime ;
    }
};

struct CompareSJFPree
{
    bool operator()(const Process *p1, const Process *p2)
    {
        // Tie-breaker: If remainingBurstTime are equal, use Arrival Time (FCFS)
        if (p1->remainingBurstTime  == p2->remainingBurstTime )
        {
            return p1->arrivalTime > p2->arrivalTime;
        }

        return p1->remainingBurstTime  > p2->remainingBurstTime ;
    }
};


class Scheduler
{

public:
    int NumberOfFinshedProcess;
    Process *currentRunningProcess = nullptr;
    int currentTime = 0;
    int timeSlice=0;
    vector<unique_ptr<Process>> processes;
    vector<Process> Finshedprocesses;
    map<Process *, bool> isQueued;
    int totalTurnaround = 0;
    int totalWaitingTime = 0;
    priority_queue<Process *, vector<Process *>, CompareProcess> readyQueue;
    priority_queue<Process *, vector<Process *>, CompareSJF> readyQueue2;
    priority_queue<Process *, vector<Process *>, CompareSJFPree> readyQueue3;
    queue<Process*> readyQueue4;
    //Standard queues for FCFS and Round Robin
    queue<Process *> fcfsQueue;
    queue<Process *> rrQueue;

    // Variables needed for Round Robin
    int quantum = 2;        // You can change this to any quantum size
    int currentQuantumTime = 0; // Tracks how long the current process has been running
    Scheduler() {}

    void runPriorityNonPreemptive()
    {
        // it will enter this if the process is runing
        if (currentRunningProcess != nullptr)
        {
            currentRunningProcess->remainingBurstTime--;
            if (currentRunningProcess->remainingBurstTime == 0)
            {
                currentRunningProcess->completionTime = currentTime;
                currentRunningProcess->turnAroundTime = currentTime - currentRunningProcess->arrivalTime;
                currentRunningProcess->waitingTime = currentRunningProcess->turnAroundTime - currentRunningProcess->burstTime;
                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;
                Finshedprocesses.push_back(*currentRunningProcess);
                readyQueue.pop();
                currentRunningProcess = nullptr;
            }
        }
        // it will enter this in the start or the current process is finshed
        if (currentRunningProcess == nullptr && processes.size() != Finshedprocesses.size())
        {
            // if there is new process commed it will push it in the readyQueue
            // Corrected loop to work with std::unique_ptr
            for (int i = 0; i < processes.size(); i++)
            {
                // 1. Access the process via the unique_ptr using ->
                Process *p = processes[i].get(); // Get the raw pointer

                // 2. Now use 'p' to check arrival time and update the map
                if (p->arrivalTime <= currentTime && !isQueued[p])
                {
                    readyQueue.push(p);
                    isQueued[p] = true;
                }
            }
        }

        if (!readyQueue.empty())
            currentRunningProcess = readyQueue.top();
        else
        {
            currentRunningProcess = nullptr;
        }
    }

    void runSFJNonPreemptive()
    {
        // it will enter this if the process is runing
        if (currentRunningProcess != nullptr)
        {
            currentRunningProcess->remainingBurstTime--;
            if (currentRunningProcess->remainingBurstTime == 0)
            {
                currentRunningProcess->completionTime = currentTime;
                currentRunningProcess->turnAroundTime = currentTime - currentRunningProcess->arrivalTime;
                currentRunningProcess->waitingTime = currentRunningProcess->turnAroundTime - currentRunningProcess->burstTime;
                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;
                Finshedprocesses.push_back(*currentRunningProcess);
                readyQueue2.pop();
                currentRunningProcess = nullptr;
            }
        }
        // it will enter this in the start or the current process is finshed
        if (currentRunningProcess == nullptr && processes.size() != Finshedprocesses.size())
        {
            // if there is new process commed it will push it in the readyQueue2
            // Corrected loop to work with std::unique_ptr
            for (int i = 0; i < processes.size(); i++)
            {
                // 1. Access the process via the unique_ptr using ->
                Process *p = processes[i].get(); // Get the raw pointer

                // 2. Now use 'p' to check arrival time and update the map
                if (p->arrivalTime <= currentTime && !isQueued[p])
                {
                    readyQueue2.push(p);
                    isQueued[p] = true;
                }
            }
        }

        if (!readyQueue2.empty())
            currentRunningProcess = readyQueue2.top();
        else
        {
            currentRunningProcess = nullptr;
        }
    }

    void runSJFPreemptive()
    {
        if (!processes.empty())
        {
            for (int i = 0; i < processes.size(); i++)
            {
                // 1. Access the process via the unique_ptr using ->
                Process *p = processes[i].get(); // Get the raw pointer

                // 2. Now use 'p' to check arrival time and update the map
                if (p->arrivalTime <= currentTime && !isQueued[p])
                {
                    readyQueue3.push(p);
                    isQueued[p] = true;
                }
            }
        }
        else {
            return;
        }


        // it will enter this if the process is the highst prirty then it will countinue running
        // else then it will update the current with the highst priorty
        if (currentRunningProcess)
        {
            currentRunningProcess->remainingBurstTime--;
            if (currentRunningProcess->remainingBurstTime == 0)
            {
                currentRunningProcess->completionTime = currentTime;
                currentRunningProcess->turnAroundTime = currentTime - currentRunningProcess->arrivalTime;
                currentRunningProcess->waitingTime = currentRunningProcess->turnAroundTime - currentRunningProcess->burstTime;
                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;
                Finshedprocesses.push_back(*currentRunningProcess);
                readyQueue3.pop();
                currentRunningProcess = nullptr;
            }
        }


        // it will enter this in the start or the current process is finshed
        if (currentRunningProcess == nullptr && processes.size() != Finshedprocesses.size()&&!readyQueue3.empty())
        {
            currentRunningProcess = readyQueue3.top();
        }
        //in case of there process has high priorty the the current
        if(!readyQueue3.empty())
            currentRunningProcess = readyQueue3.top();
        else
        {
            currentRunningProcess = nullptr;
        }



    }
    void runPriorityPreemptive()
    {
        if (!processes.empty())
        {
            for (int i = 0; i < processes.size(); i++)
            {
                // 1. Access the process via the unique_ptr using ->
                Process *p = processes[i].get(); // Get the raw pointer

                // 2. Now use 'p' to check arrival time and update the map
                if (p->arrivalTime <= currentTime && !isQueued[p])
                {
                    readyQueue.push(p);
                    isQueued[p] = true;
                }
            }
        }
        else {
            return;
        }


        // it will enter this if the process is the highst prirty then it will countinue running
        // else then it will update the current with the highst priorty
        if (currentRunningProcess)
        {
            currentRunningProcess->remainingBurstTime--;
            if (currentRunningProcess->remainingBurstTime == 0)
            {
                currentRunningProcess->completionTime = currentTime;
                currentRunningProcess->turnAroundTime = currentTime - currentRunningProcess->arrivalTime;
                currentRunningProcess->waitingTime = currentRunningProcess->turnAroundTime - currentRunningProcess->burstTime;
                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;
                Finshedprocesses.push_back(*currentRunningProcess);
                readyQueue.pop();
                currentRunningProcess = nullptr;
            }
        }


        // it will enter this in the start or the current process is finshed
        if (currentRunningProcess == nullptr && processes.size() != Finshedprocesses.size()&&!readyQueue.empty())
        {
            currentRunningProcess = readyQueue.top();
        }
        //in case of there process has high priorty the the current
        if(!readyQueue.empty())
            currentRunningProcess = readyQueue.top();
        else
        {
            currentRunningProcess = nullptr;
        }


    }
    void runFCFS()
    {
        // 1. Add newly arrived processes to the FCFS queue
        for (int i = 0; i < processes.size(); i++)
        {
            Process *p = processes[i].get();
            if (p->arrivalTime <= currentTime && !isQueued[p])
            {
                fcfsQueue.push(p);
                isQueued[p] = true;
            }
        }

        // 2. Execute the currently running process
        if (currentRunningProcess != nullptr)
        {
            currentRunningProcess->remainingBurstTime--;

            // Check if the process has finished
            if (currentRunningProcess->remainingBurstTime == 0)
            {
                // Note: Assuming you want to use the same logic as your other functions,
                // setting completion to currentTime (though technically it finishes at the END of the current second)
                currentRunningProcess->completionTime = currentTime;
                currentRunningProcess->turnAroundTime = currentTime - currentRunningProcess->arrivalTime;
                currentRunningProcess->waitingTime = currentRunningProcess->turnAroundTime - currentRunningProcess->burstTime;

                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;

                Finshedprocesses.push_back(*currentRunningProcess);
                currentRunningProcess = nullptr; // Free the CPU
            }
        }

        // 3. Fetch the next process if the CPU is idle
        if (currentRunningProcess == nullptr && !fcfsQueue.empty())
        {
            currentRunningProcess = fcfsQueue.front();
            fcfsQueue.pop();
        }
    }

    void runRRPreemptive()
    {
        //  add arrived processes
        if (!processes.empty())
        {
            for (int i = 0; i < processes.size(); i++)
            {
                Process *p = processes[i].get();

                if (p->arrivalTime <= currentTime && !isQueued[p])
                {
                    readyQueue4.push(p);
                    isQueued[p] = true;
                }
            }
        }
        else
        {
            return ;
        }


        // execute current process
        if (currentRunningProcess != nullptr)
        {
            currentRunningProcess->remainingBurstTime--;
            timeSlice++;

            if (currentRunningProcess->remainingBurstTime == 0)
            {
                currentRunningProcess->completionTime = currentTime + 1;

                currentRunningProcess->turnAroundTime =
                    currentRunningProcess->completionTime -
                    currentRunningProcess->arrivalTime;

                currentRunningProcess->waitingTime =
                    currentRunningProcess->turnAroundTime -
                    currentRunningProcess->burstTime;

                NumberOfFinshedProcess++;
                totalTurnaround += currentRunningProcess->turnAroundTime;
                totalWaitingTime += currentRunningProcess->waitingTime;

                Finshedprocesses.push_back(*currentRunningProcess);

                currentRunningProcess = nullptr;
                timeSlice = 0;
            }

            else if (timeSlice == quantum)
            {
                readyQueue4.push(currentRunningProcess);
                currentRunningProcess = nullptr;
                timeSlice = 0;
            }
        }

        // if no running process get one
        if (currentRunningProcess == nullptr && !readyQueue4.empty())
        {
            currentRunningProcess = readyQueue4.front();
            readyQueue4.pop();
            timeSlice = 0;
        }
    }
};
