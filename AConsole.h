#pragma once

#include <string>
#include <tuple>
#include <memory>



class AConsole {
public:
	typedef std::string String;
	AConsole(String name);
	~AConsole() = default;
        bool isActive() { return this->_active; }
        bool canRemove() { return this->canRemove; };
        virtual void run();
        virtual void stop();



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
    bool _active = false;
    bool _canRemove  = false;
};

#endif  // !ACONSOLE_H



