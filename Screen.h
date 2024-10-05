#pragma once
#include "AConsole.h"
#include "Process.h"

class Screen : public AConsole{
public:
	Screen(std::shared_ptr<Process> process, String processName);
	
	void onEnabled() override;
	void process() override;
	void display() override;

private:
	void printProcessInfo() const;
	std::shared_ptr<Process> attachedProcess;
	bool refreshed = false;
};


