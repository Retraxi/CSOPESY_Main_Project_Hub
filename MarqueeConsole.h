#pragma once
#include "AConsole.h"
#include <Windows.h>
#include <vector>
#include <string>
#ifndef MARQUEECONSOLE_H
#define MARQUEECONSOLE_H

class MarqueeConsole : public AConsole
{
public:


	

	//void onEnabled() override;
	//void process() override;
	//void display() override;
	
	MarqueeConsole(int refreshRate);
	~MarqueeConsole() = default;
        void run;
        void stop();

private:
	void draw();
	void processCommand();

	bool stopFlag = true;
	int refreshRate;
	int interval;

	std::string input = "";
	std::string previous = "";
};

#endif // !MARQUEECONSOLE_H
