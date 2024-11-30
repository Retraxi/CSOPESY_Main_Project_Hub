#pragma once

#include <string>
#include <tuple>
#include <memory>



class AConsole {
public:
	typedef std::string String;
	AConsole(String name);
	~AConsole() = default;
        bool isConsoleActive() { return this->_active; }
        bool isRemovable() { return this->removable; };
        virtual void start();
        virtual void terminate();



protected:
    virtual void render();
    void moveCursorTo(short row, short col);
    void drawHorizontalBorder(short row, short col, short length, bool doubleLine = false);
    void drawVerticalBorder(short row, short col, short length);
    void writeTextAt(short row, short col, const String& text, short length = -1, bool alignRight = false);
    void decorateCorners(short topRow, short leftCol, short width, short height);
    void drawRectangle(short topRow, short leftCol, short width, short height, bool doubleLine = false);
    std::tuple<short, short> fetchConsoleSize() const;

    String consoleName;
    bool active = false;
    bool removable = false;
};

#endif  // !ACONSOLE_H



