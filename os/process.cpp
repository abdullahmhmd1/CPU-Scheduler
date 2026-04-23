class Process {
public:
    int id;
    int arrivalTime;
    int burstTime;
    int remainingBurstTime;
    int priority;
    int completionTime;
    int turnAroundTime;
    int waitingTime;

    Process(int id, int arrival, int burst, int priority = 0)
        : id(id), arrivalTime(arrival), burstTime(burst),
        remainingBurstTime(burst), priority(priority) {}
};
