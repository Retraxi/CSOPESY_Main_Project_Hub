#include "MarqueeConsole.h"
#include <thread>
#include <iostream>
#include <conio.h>
#include "ConsoleManager.h"
#include <cstdlib>
#include <chrono>
#include <memory>
#include <string>
#include <tuple>


//using namespace std;

MarqueeConsole::MarqueeConsole(int refreshRate) : AConsole("MarqueeConsole") {
    this->frameRate = refreshRate;
    this->frameInterval = 1000 / refreshRate; 
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
