#pragma once
#include <memory>
#include <vector>
#include <string>

class Process {
	//what's needed in a process
	/* Reference: Overview of the  OS Emulator [Timestamp: 42:34]
		- Core ID to tell which Core it's using
		- some sort of command list, like instruction thing
		- isRunning since it's a process that is working
		- ETA till finishing
		- current instruction line -> next instruction line
		- getter functions
		- ID
		- name

	*/
public:
	Process(int pid, std::string name);
	~Process() = default;
	int pid;
	

	std::string getName();
	int getProcessID() const;
	int getTotalInstructionLines() const;
	void setCoreID(int coreID);
	void testInitFCFS();

	void execute();
	bool isDone() const;

private:
	int coreID;
	int currentInstructionLine;

	std::tm* createdAt;
	std::tm* finishedAt;
	std::string processName;
	std::vector<std::string> instructionList; 
};