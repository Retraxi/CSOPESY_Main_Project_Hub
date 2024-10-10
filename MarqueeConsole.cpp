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
