#include "MarqueeConsole.h"
#include <thread>
#include <iostream>
#include <conio.h>
#include "ConsoleManager.h"
using namespace std;

MarqueeConsole::MarqueeConsole() : AConsole("MARQUEE_CONSOLE")
{
	
}

void MarqueeConsole::printMarqueeHeader()
{
	cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "+ Dislaying a marquee console! +" << endl;
	cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
}

void MarqueeConsole::moveCursor(int x, int y)
{
	//function for moving the console cursor
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(this->marqueeHandle, coord);
}

void MarqueeConsole::printProcessed()
{
	for (size_t i = 0; i < processedCommands.size(); i++)
	{
		cout << processedCommands[i];
	}
}

void MarqueeConsole::onEnabled()
{
	//initialization
	this->marqueeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	this->isRunning = true;
	thread keyboardPoller(&MarqueeConsole::keyboardPolling, this); //??? save me
	keyboardPoller.detach();
}

void MarqueeConsole::display()
{
	//assuming this constantly gets called and no other function moves the cursor
	moveCursor(0, 0); //reset
	system("cls"); //unsafe
	
	printMarqueeHeader();

	//code to move marquee
	moveCursor(x, y);
	std::cout << marqueeText << std::flush;

	// Update position for the next frame
	
	//below the line where the input will be
	moveCursor(0, height + 2);
	printProcessed();

	moveCursor(0, height + 1);
	cout << "Enter a command for MARQUEE_CONSOLE!: ";
	cout << currentInput;

	if (this->isThreaded)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(pollingRate));
	}
}

void MarqueeConsole::process()
{
	//update values like the marquee console
	x += dx;
	y += dy;

	// Hitbox + Border checking
	if (x <= 0 || x >= width - marqueeText.length()) {
		dx = -dx;
	}
	if (y <= 3 || y >= height) {
		dy = -dy;
	}
}

void MarqueeConsole::keyboardPolling()
{
	//_kbhit()
	while (this->isRunning) {
		if (_kbhit())
		{
			char key = _getch();
			if (key == 8 && currentInput.length() > 0) //backspace
			{
				currentInput.pop_back();
			}
			else if (key == 13) {
				if (currentInput == "exit")
				{
					//return to previous console (to the main console)
				}
				String temp = "Command processed in MARQUEE CONSOLE: ";
				temp += currentInput + '\n';
				currentInput.clear();
				processedCommands.push_back(temp);
			}	else {
				currentInput += key;
			}
		}

		if (this->isThreaded) {
			std::this_thread::sleep_for(std::chrono::milliseconds(pollingRate));
		}
	}
}

// New

#include "MarqueeConsole.h"
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <tuple>

MarqueeConsole::MarqueeConsole(int refreshRate) : AConsole("MarqueeConsole") {
    this->frameRate = refreshRate;
    this->frameInterval = 1000 / refreshRate; // Changed variable name for clarity
}

void MarqueeConsole::startConsole() {
    if (!this->isStopped)
        return;

    char backspaceKey = 8;
    char enterKey = 13;
    this->isStopped = false;
    this->_active = true;
    int key = 0;

    std::thread renderThread(&MarqueeConsole::render, this);
    renderThread.detach();

    while (!this->isStopped) {
        if (_kbhit()) {
            key = _getch();
            if (key == backspaceKey) {
                if (!this->userInput.empty()) {
                    this->userInput.pop_back();
                }
            } else if (key == enterKey) {
                handleCommand();
                this->userInput = "";
            } else {
                this->userInput.push_back(key);
            }
        }
    }
}

void MarqueeConsole::terminateConsole() {
    this->isStopped = true;
}

void MarqueeConsole::render() {
    std::string marqueeMessage = "Hello world in marquee!";

    auto [consoleWidth, consoleHeight] = getWindowSize();
    int topBoundary = 3;
    int bottomBoundary = consoleHeight - 3;
    int rightBoundary = consoleWidth - marqueeMessage.size();
    int leftBoundary = 0;

    int xPosition = leftBoundary;
    int yPosition = topBoundary;

    bool moveUpward = false;
    bool moveLeftward = false;

    while (!this->isStopped) {
        system("cls");

        for (int i = 0; i < 41; i++) std::cout << '*';
        std::cout << std::endl;
        std::cout << "* Displaying a marquee console! *" << std::endl;
        for (int i = 0; i < 41; i++) std::cout << '*';
        std::cout << std::endl;

        PrintAtCoords(yPosition, xPosition, marqueeMessage);

        displayCommandHistory();
        displayPrompt();

        moveMarquee(xPosition, yPosition, moveUpward, moveLeftward, leftBoundary, rightBoundary, topBoundary, bottomBoundary);
        std::this_thread::sleep_for(std::chrono::milliseconds(this->frameInterval));
    }

    this->_active = false;
}

void MarqueeConsole::handleCommand() {
    if (this->userInput == "exit") {
        this->commandHistory = "";
        this->terminateConsole();
    } else if (this->userInput.empty()) {
        this->commandHistory += "root\\marquee:\\> \n";
    } else {
        this->commandHistory += "root\\marquee:\\> " + this->userInput + "\n";
        this->commandHistory += "Command processed: " + this->userInput + "\n";
    }
}

void MarqueeConsole::moveMarquee(
    int& xPosition, int& yPosition, bool& moveUpward, bool& moveLeftward,
    int leftBoundary, int rightBoundary, int topBoundary, int bottomBoundary) {
    if (moveUpward) {
        yPosition--;
        if (yPosition <= topBoundary) moveUpward = false;
    } else {
        yPosition++;
        if (yPosition >= bottomBoundary) moveUpward = true;
    }

    if (moveLeftward) {
        xPosition--;
        if (xPosition <= leftBoundary) moveLeftward = false;
    } else {
        xPosition++;
        if (xPosition >= rightBoundary) moveLeftward = true;
    }
}

void MarqueeConsole::displayCommandHistory() {
    auto [consoleWidth, consoleHeight] = getWindowSize();
    int bottomLine = consoleHeight - 3;
    int commandPromptLine = bottomLine + 1;
    int historyLine = bottomLine;

    SetCursorPosition(historyLine, 0);
    std::cout << this->commandHistory;

    SetCursorPosition(commandPromptLine, 0);
    std::cout << "root\\marquee:\\> " << this->userInput;
}


*/

