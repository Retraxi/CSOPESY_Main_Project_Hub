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



/* NEW 

#include "AConsole.h"
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
	this->refreshRate = refreshRate;
	this->interval = 1000.0 / refreshRate;
}

void MarqueeConsole::run() {
	if (this->stopFlag != true)
		return;


	char DELETE = 8;
	char ENTER = 13;
	this->stopFlag = false;
	this->_active = true;
	int key = 0;

	std::thread outputThread(&MarqueeConsole::draw, this);
	outputThread.detach();

	while (!this->stopFlag) {
		if (_kbhit()) {
			key = _getch();
			if (key == DELETE) {
				if (this->input.size() > 0) this->input.pop_back();
			}
			else if (key == ENTER) {
				processCommand();
				this->input = "";
			}
			else {
				this->input.push_back(key);
			}
		}
	}
}

void MarqueeConsole::stop() {
	stopFlag = true;
}

void MarqueeConsole::draw() {
	std::string message = "Hello world in marquee!";

	std::tuple<int, int> bounds = getWindowSize();
	int top = 3;
	int bottom = std::get<1>(bounds) - 3;
	int right = std::get<0>(bounds) - message.size();
	int left = 0;
	int x = left;
	int y = top;
	bool moveUp = false;
	bool moveLeft = false;

	while (!this->stopFlag) {
		system("cls");

		for (int i = 0; i < 41; i++) std::cout << '*';
		std::cout << std::endl;
		std::cout << "* Displaying a marquee console! *" << std::endl;
		for (int i = 0; i < 41; i++) std::cout << '*';
		std::cout << std::endl;

		std::string::difference_type n = std::count(this->previous.begin(), this->previous.end(), '\n');

		PrintAtCoords(y, x, message);
		SetCursorPosition(bottom + 1, left);
		std::cout << this->previous;
		std::cout << "root\\marquee:\\> " << this->input << std::endl;
		SetCursorPosition(bottom + 1 + n, left + 16 + this->input.size());

		if (moveUp) {
			y--;
			if (y <= top) moveUp = false;
		}
		else {
			y++;
			if (y >= bottom) moveUp = true;
		}

		if (moveLeft) {
			x--;
			if (x <= left) moveLeft = false;
		}
		else {
			x++;
			if (x >= right) moveLeft = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(this->interval));
	}
	this->_active = false;
}

void MarqueeConsole::processCommand() {
	if (this->input == "exit") {
		this->previous = "";
		this->stop();
	}
	else if (this->input == "") {
		this->previous += "root\\marquee:\\> \n";
	}
	else {
		this->previous += "root\\marquee:\\> " + this->input + "\n";
		this->previous += "Command processed: " + this->input + "\n";
	}
}

*/

