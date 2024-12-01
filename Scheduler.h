// Scheduler.h

#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <memory>
#include <queue>
#include <vector>

#include "CPU.h"
#include "MemoryManager.h"
#include "Process.h"
#include <mutex>

using namespace std;

struct compare {
    bool operator()(shared_ptr<Process> a, shared_ptr<Process> b) {
        return a->getBurst() > b->getBurst();
    }
};

class Scheduler {
public:
    static Scheduler* get();

    void startFCFS(int delay);
    void startRR(int delay, int quantumCycles);

    void stop();
    void destroy();

    static void initialize(int cpuCount,
        float batchProcessFreq,
        int minIns, int maxIns,
        int minMemProc, int maxMemProc,
        int maxMem, int minPage, int maxPage);

    void addProcess(std::shared_ptr<Process> process);
    void schedulerTest();
    void schedulerTestStop();

    void printStatus();
    void printMem();
    void processSmi();
    void vmstat();

private:
    Scheduler();
    ~Scheduler() = default;

    int getTotalTicks();
    int getInactiveTicks();

    std::mutex mtx;

    void runFCFS(float delay); 
    

void runRR(float delay, int quantumCycles);

    void schedulerRun();

    static Scheduler* _ptr;

    queue<shared_ptr<Process>> _readyQueue;
    vector<shared_ptr<CPU>> _cpuList;
    vector<shared_ptr<Process>> _processList;
    MemoryManager* _memMan = nullptr;

    float batchProcessFreq;
    int minIns;
    int maxIns;
    int _minMemProc;
    int _maxMemProc;
    int _minPage;
    int _maxPage;
    int _cycleCount;

    bool _testRunning = false;
    bool running = false;
    friend class ConsoleManager;
    friend class FlatAllocator;
    friend class PagingAllocator;
};

#endif // SCHEDULER_H
