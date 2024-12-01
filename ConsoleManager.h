#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H


#include <memory>
#include "AConsole.h"
#include <unordered_map>
#include <Windows.h>
//#include "Screen.h"
#include <iostream>
#include "MainConsole.h"
#include <string>
#include <chrono>
#include "Scheduler.h"
//#include "MarqueeConsole.h"

//typedef std::shared_ptr<AConsole> AConsole_;
using ConsolePtr = std::shared_ptr<AConsole>;


//const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
//const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";

class ConsoleManager
{
public:
	//typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

	 static ConsoleManager* getInstance();
	 static void initializeManager();
	 static void shutdownManager();
        void launch();
      bool createConsole(const std::string& name, ConsolePtr console = nullptr);

//	void drawConsole() const;//read only
//	void process() const;
	void switchToConsole(const std::string& consoleName);
        void setScheduler(Scheduler* scheduler) { _scheduler = scheduler; };

	
	//HANDLE getConsoleHandle() const; //not being used for now

private:
	ConsoleManager();
	~ConsoleManager() ;
	ConsoleManager(ConsoleManager const&) {};
        static ConsoleManager* instance;
        std::unordered_map<std::string, ConsolePtr> consoleRegistry;
        AConsole_ _current = nullptr;
        AConsole_ _mainConsole = nullptr;
        Scheduler* _scheduler = nullptr;
        friend class MainConsole;


}; #endif // !CONSOLEMANAGER_H
