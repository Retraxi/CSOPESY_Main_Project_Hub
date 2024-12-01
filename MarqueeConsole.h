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
       void startConsole();
       void terminateConsole();

private:
	void render();
	void handleCommand();
        void moveMarquee();

	
       bool isStopped = true; 
	 int frameRate;
         int frameInterval;

	 std::string userInput = "";
	  std::string commandHistory = "";
};

#endif // !MARQUEECONSOLE_H


