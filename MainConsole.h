#pragma once
#include "AConsole.h"

class MainConsole : public AConsole
{
public:
	MainConsole();
	~MainConsole() = default;

	void onEnabled() override;
	void process() override;
	void display() override;
private:
	bool refresh = false;
	bool initialized = false;
	void printHeader(); //the very top
	void printProcesses();
};

/*
#pragma once
#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

#include "AConsole.h"
#include "ConsoleManager.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

typedef const std::vector<std::string>& argType;


class MainConsole : public AConsole {
public:
    MainConsole(ConsoleManager* conman);
    ~MainConsole() = default;
    void run() override;
    void stop() override;

private:
    void draw() override;
    void printHeader();

    std::unordered_map<std::string, std::function<void(argType)>> _commandMap;

    bool _initialized = false;

    ConsoleManager* _conman = nullptr;
};

#endif // !MAINCONSOLE_H


*/
