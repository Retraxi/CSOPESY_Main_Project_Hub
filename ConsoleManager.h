#pragma once

#include <memory>
#include "AConsole.h"
#include <unordered_map>
#include <Windows.h>
#include "Screen.h"
#include <iostream>
#include "MainConsole.h"
#include "MarqueeConsole.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";

class ConsoleManager
{
public:
	typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* getInstance();
	static void initialize();
	static void destroy();

	void drawConsole() const;//read only
	void process() const;
	void switchConsole(std::string consoleName);

	void printScreens();
	int tableSize();

	void registerScreen(std::shared_ptr<Screen> screenRef);
	void switchToScreen(std::string screenName);
	void unregisterScreen(std::string screenName);

	void returnToPreviousConsole();
	void exitApplication();
	bool isRunning() const;
	
	//HANDLE getConsoleHandle() const; //not being used for now

private:
	ConsoleManager();
	~ConsoleManager() = default;
	ConsoleManager(ConsoleManager const&) {};
	ConsoleManager& operator=(ConsoleManager const&) {};
	static ConsoleManager* sharedInstance;

	ConsoleTable consoleTable;
	std::shared_ptr<AConsole> currentConsole;
	std::shared_ptr<AConsole> previousConsole;

	bool running = true;
	//HANDLE consoleHandle;

};