#include "PrintCommand.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


typedef std::string String;

int PrintCommand::msDelay = 50;

void PrintCommand::execute(int core, String filename) {

    std::this_thread::sleep_for(std::chrono::milliseconds(PrintCommand::msDelay));
}

void PrintCommand::setMsDelay(int delay) {
    if (delay < 0) return;
    PrintCommand::msDelay = delay;
}
