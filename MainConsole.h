#pragma once
#include "AConsole.h"

class MainConsole : public AConsole
{
public:
	MainConsole();
	~MainConsole() = default;

	void onEnabled() override;
	void process() override;
	void display() override;
private:
	bool refresh = false;
	bool initialized = false;
	void printHeader(); //the very top
	void printProcesses();
};