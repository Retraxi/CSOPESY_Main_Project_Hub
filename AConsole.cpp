#include "AConsole.h"
#include <iostream>
#include <thread>
#include <windows.h>
#include <tuple>

AConsole::AConsole(String consoleName) : consoleName(std::move(consoleName)) {}

void AConsole::start() {
    std::cout << "Activating Console: " << this->consoleName << std::endl;
    this->active = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Increased delay slightly
    this->terminate();
}

void AConsole::terminate() {
    if (this->active) {
        this->active = false;
        std::cout << "Deactivating Console: " << this->consoleName << std::endl;
    }
}

void AConsole::render() {
    std::cout << "Console: " << this->consoleName << std::endl;
    auto [width, height] = this->fetchConsoleSize();
    std::cout << "Width: " << width << ", Height: " << height << std::endl;
}

void AConsole::moveCursorTo(short row, short col) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { col, row };
    SetConsoleCursorPosition(handle, position);
}

void AConsole::drawHorizontalBorder(short row, short col, short length, bool doubleLine) {
    moveCursorTo(row, col);
    char borderChar = doubleLine ? '=' : '-';
    for (short i = 0; i < length; i++) {
        std::cout << borderChar;
    }
}

void AConsole::drawVerticalBorder(short row, short col, short length) {
    for (short i = 0; i < length; i++) {
        moveCursorTo(row + i, col);
        std::cout << '|';
    }
}

void AConsole::writeTextAt(short row, short col, const String& text, short length, bool alignRight) {
    moveCursorTo(row, col);
    if (length != -1) {
        if (alignRight)
            printf("%-*s", length, text.c_str());
        else
            printf("%*s", length, text.c_str());
    } else {
        std::cout << text;
    }
}

void AConsole::decorateCorners(short topRow, short leftCol, short width, short height) {
    width -= 1;
    height -= 1;
    writeTextAt(topRow, leftCol, "+");
    writeTextAt(topRow, leftCol + width, "+");
    writeTextAt(topRow + height, leftCol, "+");
    writeTextAt(topRow + height, leftCol + width, "+");
}

void AConsole::drawRectangle(short topRow, short leftCol, short width, short height, bool doubleLine) {
    width -= 1;
    height -= 1;
    if (width > 0) {
        drawHorizontalBorder(topRow, leftCol, width, doubleLine);
        drawHorizontalBorder(topRow + height, leftCol, width, doubleLine);
    }
    if (height > 0) {
        drawVerticalBorder(topRow, leftCol, height);
        drawVerticalBorder(topRow, leftCol + width, height);
    }
    decorateCorners(topRow, leftCol, width + 1, height + 1);
}

std::tuple<short, short> AConsole::fetchConsoleSize() const {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    short width, height;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

    return std::make_tuple(width, height);
}
