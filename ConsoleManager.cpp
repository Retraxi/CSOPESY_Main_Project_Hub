#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include <iostream>
#include <memory>
#include <string>

ConsoleManager* ConsoleManager::instance = nullptr; 

ConsoleManager* ConsoleManager::getInstance() {
    return instance;
}

void ConsoleManager::initializeManager() {
    instance = new ConsoleManager();
}

void ConsoleManager::shutdownManager() {
    delete instance;
    instance = nullptr;
}

void ConsoleManager::launch() {
    if (activeConsole) {
        activeConsole->run();
    } else {
        std::cerr << "No active console to launch." << std::endl;
    }
}

bool ConsoleManager::createConsole(const std::string& name, ConsolePtr console) {
    if (consoleRegistry.find(name) != consoleRegistry.end()) {
        std::cerr << "Console '" << name << "' is already running!" << std::endl;
        return false;
    }

    if (!console) {
        auto processList = scheduler->getProcesses(); // Assuming `getProcesses()` exists
        for (const auto& process : processList) {
            if (process->getName() == name && !process->hasFinished()) {
                console = std::make_shared<ProcessConsole>(process);
                break;
            }
        }

        if (!console) {
            std::cerr << "Unable to create console for process: " << name << std::endl;
            return false;
        }
    }

    consoleRegistry[name] = console;
    switchToConsole(name);
    return true;
}

void ConsoleManager::switchToConsole(const std::string& consoleName) {
    auto it = consoleRegistry.find(consoleName);
    if (it == consoleRegistry.end()) {
        std::cerr << "Console '" << consoleName << "' not found." << std::endl;
        return;
    }

    if (activeConsole) {
        activeConsole->stop();
    }

    activeConsole = it->second;
    activeConsole->run();

    // Wait for the console to finish its execution
    while (activeConsole->isActive()) {}

    // Check if the console needs to be removed
    if (activeConsole->canRemove()) {
        consoleRegistry.erase(consoleName);
        activeConsole = nullptr;
    }

    // Return to main console
    if (!activeConsole) {
        activeConsole = mainConsole;
        activeConsole->run();
    }
}

ConsoleManager::ConsoleManager() {
    mainConsole = std::make_shared<MainConsole>(this);
    activeConsole = mainConsole;
}

ConsoleManager::~ConsoleManager() {
   
}
