#include "ConsoleManager.h"
#include "AConsole.h"
#include <iostream>
#include <memory>
#include <string>
#include "MainConsole.h"
#include "ProcessConsole.h"



ConsoleManager* ConsoleManager::ptr  = nullptr;
ConsoleManager* ConsoleManager::get() {
    return ConsoleManager::ptr;
}

void ConsoleManager::initialize()
{
 ConsoleManager::ptr = new ConsoleManager();
};


void ConsoleManager::destroy()
{
	//deleteance;
	ConsoleManager::ptr = nullptr;
	
};

void ConsoleManager::start() {
    this->_current->run();
}



bool ConsoleManager::newConsole(std::string name, AConsole_ console) {
    if (this->_consoleMap.find(name) != this->_consoleMap.end()) {
        std::cout << "Process '" + name + "' is already running!" << std::endl;
        return false;
    }

    bool found = false;
    if (console == nullptr) {
        std::vector<std::shared_ptr<Process>> copyList = this->_scheduler->_processList;
        for (int i = 0; i < copyList.size(); i++) {
            if (name == copyList.at(i)->getName() && !copyList.at(i)->hasFinished()) {
                console = std::make_shared<ProcessConsole>(copyList.at(i));
                found = true;
                break;
            }
        }
        if (found)
            this->_consoleMap[name] = console;
       
        this->switchConsole(name);
    }
    else {
        this->_consoleMap[name] = console;
    }

    return true;
}



void ConsoleManager::exitApplication()
{
	this->running = false;
}

bool ConsoleManager::isRunning() const
{
	return this->running;
}

void ConsoleManager::drawConsole() const
{
	if (this->currentConsole != nullptr)
	{
		this->currentConsole->display();
	}
	else {
		std::cerr << "There is no assigned console. Please check." << std::endl;
	}
}

void ConsoleManager::process() const
{
	if (this->currentConsole != nullptr)
	{
		this->currentConsole->process();
	}
	else {
		std::cerr << "There is no assigned console, Please check." << std::endl;
	}
}

void ConsoleManager::switchConsole(std::string processName) {
    if (this->_consoleMap.find(processName) == this->_consoleMap.end()) {
        std::cout << "Process " + processName + " not found." << std::endl;
        return;
    }
    else if (this->_consoleMap[processName]->canRemove()) {
        this->_consoleMap.erase(processName);
        std::cout << "Process " + processName + " not found." << std::endl;
        return;
    }

    this->_current->stop();

    this->_current = this->_consoleMap[processName];
    this->_current->run();

    // Wait for console to set active to false
    while (this->_current->isActive()) {}

    if (this->_current->canRemove())
        this->_consoleMap.erase(processName);

    this->_current = this->_mainConsole;
    this->_current->run();
}




void ConsoleManager::unregisterScreen(std::string screenName)
{
	if (this->consoleTable.contains(screenName))
	{
		this->consoleTable.erase(screenName);
	}
	else
	{
		std::cerr << "Unable to unregister " << screenName << ". Was it registered?" << std::endl;
	}
}

void ConsoleManager::registerScreen(std::shared_ptr<Screen> screenRef) 
{ 
	if (this->consoleTable.contains(screenRef->getName()))
	{
		std::cerr << "Screen name " << screenRef->getName() << " already exists. Please use a different name." << std::endl;
		return;
	}

	this->consoleTable[screenRef->getName()] = screenRef;
};

void ConsoleManager::switchToScreen(std::string screenName)
{
	if (this->consoleTable.contains(screenName))
	{
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[screenName];
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "Screen name " << screenName << " not found. Was it initialized?" << std::endl;
	}
}

int ConsoleManager::tableSize()
{
	return this->consoleTable.size();
}

void ConsoleManager::printScreens()
{
	for (auto const& i : this->consoleTable) {
		std::cout << "Process name: " << i.first << std::endl;
	}
}

ConsoleManager::ConsoleManager()
{
	this->running = true;

	//initialize consoles
	//this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	//Declaration of initial special consoles
	//const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();
	

	this->consoleTable[MAIN_CONSOLE] = mainConsole;
	this->consoleTable[MARQUEE_CONSOLE] = marqueeConsole;



	
}

ConsoleManager::~ConsoleManager() {}
