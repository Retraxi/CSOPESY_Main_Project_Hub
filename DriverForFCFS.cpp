#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <ctime>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// Struct to represent a process
struct Process {
    string name;
    int printCount; // Number of print commands
    time_t startTime;
};

// Global variables for process queues and threads
queue<Process> processQueue;
mutex mtx;
condition_variable cv;
atomic<bool> finished(false);
const int NUM_CORES = 4;
atomic<bool> coresBusy[NUM_CORES] = { false };

// Function to get current timestamp
string getTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", ltm);
    return string(timestamp);
}

// Function to simulate a process being executed on a core
void executeProcess(Process p, int coreId) {
    ofstream logFile(p.name + ".txt", ios_base::app);
    if (!logFile) {
        cerr << "Error creating log file for process: " << p.name << endl;
        return;
    }

    logFile << "Process name: " << p.name << endl;
    logFile << "Logs:" << endl;

    for (int i = 0; i < p.printCount; i++) {
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate some work
        logFile << getTimestamp() << " Core:" << coreId << " \"Hello world from " << p.name << "!\"" << endl;
    }

    logFile.close();
}

// Function to simulate CPU cores
void cpuWorker(int coreId) {
    while (!finished) {
        Process p;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return !processQueue.empty() || finished; });

            if (finished && processQueue.empty()) {
                return; // Exit the thread if the scheduler is finished and no more processes are left
            }

            // Get the next process in the queue
            if (!processQueue.empty()) {
                p = processQueue.front();
                processQueue.pop();
                coresBusy[coreId] = true;
            }
            else {
                continue;
            }
        }

        // Execute the process outside of the lock
        executeProcess(p, coreId);

        // Mark core as free after finishing the process
        coresBusy[coreId] = false;
    }
}

// Function to simulate the scheduler
void scheduler() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        if (processQueue.empty()) {
            break;
        }

        cv.notify_all();  // Wake up CPU worker threads
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(1)); // Scheduler checks every second
    }

    finished = true;
    cv.notify_all(); // Wake up all threads to finish execution
}

// Function to display running and finished processes
void displayProcesses() {
    unique_lock<mutex> lock(mtx);
    cout << "\nRunning processes:" << endl;
    bool hasRunning = false;
    for (int i = 0; i < NUM_CORES; i++) {
        if (coresBusy[i]) {
            cout << "Core:" << i << " is running a process." << endl;
            hasRunning = true;
        }
    }
    if (!hasRunning) {
        cout << "No processes are currently running." << endl;
    }

    cout << "\nFinished processes:" << endl;
    if (processQueue.empty() && !hasRunning) {
        cout << "All processes are finished!" << endl;
    }
    else {
        cout << "Some processes are still running." << endl;
    }
}

// Main function
int main() {
    // Create 10 processes with 100 print commands each
    for (int i = 1; i <= 10; i++) {
        Process p = { "process" + to_string(i), 100, time(0) };
        processQueue.push(p);
    }

    // Create threads for CPU cores
    vector<thread> coreThreads;
    for (int i = 0; i < NUM_CORES; i++) {
        coreThreads.push_back(thread(cpuWorker, i));
    }

    // Start the scheduler thread
    thread schedulerThread(scheduler);

    // Command input loop
    string command;
    while (true) {
        cout << "Enter command (screen -ls or exit): ";
        cin >> command;
        if (command == "screen -ls") {
            displayProcesses();
        }
        else if (command == "exit") {
            break;
        }
    }

    // Wait for the scheduler and core threads to finish
    schedulerThread.join();
    for (thread& t : coreThreads) {
        t.join();
    }

    cout << "Emulator finished. All processes are complete." << endl;
    return 0;
}
