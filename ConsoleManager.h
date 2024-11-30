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

typedef std::shared_ptr<AConsole> AConsole_;

//const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
//const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";

class ConsoleManager
{
public:
	//typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* get();
	static void initialize();
	static void destroy();
        void start();
        bool newConsole(std::string name, AConsole_ console = nullptr);

//	void drawConsole() const;//read only
//	void process() const;
	void switchConsole(std::string processName);
        void setScheduler(Scheduler* scheduler) { _scheduler = scheduler; };

	
	//HANDLE getConsoleHandle() const; //not being used for now

private:
	ConsoleManager();
	~ConsoleManager() ;
	ConsoleManager(ConsoleManager const&) {};
        static ConsoleManager* ptr;
        std::unordered_map<std::string, AConsole_> _consoleMap;
        AConsole_ _current = nullptr;
        AConsole_ _mainConsole = nullptr;
        Scheduler* _scheduler = nullptr;
        friend class MainConsole;


}; #endif // !CONSOLEMANAGER_H
