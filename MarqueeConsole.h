#pragma once
#include "AConsole.h"
#include <Windows.h>
#include <vector>

class MarqueeConsole : public AConsole
{
public:

	//marquee text
	void printMarqueeHeader();
	void moveCursor(int x, int y);

	//keyboard polling
	void keyboardPolling();
	void printProcessed();


	void onEnabled() override;
	void process() override;
	void display() override;
	
	MarqueeConsole();
	~MarqueeConsole() = default;

private:
	bool isRunning = false;
	bool isThreaded = true;

	int width = 100;
	int height = 20;
	int refreshRate = 40;
	int pollingRate = 40;

	int x = 0;
	int y = 3;
	int dx = 1;
	int dy = 1;


	String marqueeText = "Hello World in Marquee Text!";
	std::vector<String> processedCommands;
	String currentInput;

	HANDLE marqueeHandle = nullptr;
};