#include "ConsoleManager.h"




ConsoleManager* ConsoleManager::sharedInstance = nullptr;
ConsoleManager* ConsoleManager::getInstance()
{
	return sharedInstance;
};

void ConsoleManager::initialize()
{
	sharedInstance = new ConsoleManager();
};

void ConsoleManager::destroy()
{
	delete sharedInstance;
};

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

void ConsoleManager::switchConsole(std::string consoleName)
{
	if (this->consoleTable.contains(consoleName))
	{
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[consoleName];
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "Console name " << consoleName << " not found. Was it initialized?" << std::endl;
	}
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

int ConsoleManager::tableSize() {
	return this->consoleTable.size();
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

ConsoleManager::ConsoleManager()
{
	this->running = true;

	//initialize consoles
	//this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	//Declaration of initial special consoles
	const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;
	

	this->switchConsole(MAIN_CONSOLE);
	
}

void ConsoleManager::returnToPreviousConsole()
{
	if (this->previousConsole != nullptr)
	{
		this->currentConsole = this->consoleTable[this->previousConsole->getName()];
	}
}

