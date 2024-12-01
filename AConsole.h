#pragma once
#ifndef ACONSOLE_H
#define ACONSOLE_H

#include <memory>
#include <string>
#include <tuple>


class AConsole {
public:
    AConsole(std::string name);
    ~AConsole() = default;

    bool isActive() { return this->_active; }
    bool canRemove() { return this->_canRemove; };

    virtual void run();
    virtual void stop();

protected:
    virtual void draw();
    void moveCursorTo(short row, short col);
    void DrawHorizontalBorder(short row, short col, short length, bool two = false);
    void DrawVerticalBorder(short row, short col, short length);
    void writeTextAt(short row, short col, std::string string, short length = -1, bool right = false);
    void decorateCorners(short topRow, short leftCol, short width, short height);
    void drawRectangle(short topRow, short leftCol, short width, short height, bool two = false);
    std::tuple<short, short> getWindowSize();

    std::string _name;

    bool _active = false;
    bool _canRemove = false;
};

#endif // !ACONSOLE_H

