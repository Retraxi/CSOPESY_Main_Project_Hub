1. saveToBackingStore
Purpose: Saves the current state of a process to a file in the backing store directory (./BackingStore).

Steps:

Opens a file named process_<pid>.txt in the ./BackingStore directory, where <pid> is the process ID.
Writes the process details into the file, including:
Process ID (pid).
Process name (processName).
Current instruction being executed (currentInstructionLine).
Core ID (coreID), indicating the processor core handling the process.
Creation time (createdAt) and finish time (finishedAt) if available, formatted as a human-readable string.
Total number of instructions (instructionList.size()).
Closes the file and prints a success message to the console.
If the file cannot be opened, prints an error message.
Example Output:

arduino
Copy code
Process 123 saved to backing store.
2. loadFromBackingStore
Purpose: Restores a process's state from the corresponding file in the backing store.

Steps:

Opens the file process_<pid>.txt in the ./BackingStore directory.
Reads the file line by line and extracts process details:
Parses and assigns pid, processName, currentInstructionLine, coreID, createdAt, and finishedAt.
Handles the conversion of timestamps (createdAt and finishedAt) from string format to std::tm objects.
Prints a success message if the process state is successfully loaded.
If the file cannot be opened or a parsing error occurs, prints an error message and returns false.
Error Handling:

Checks for file accessibility issues and parsing failures for the timestamp.
Converts timestamps using std::get_time for format validation and parsing.
Example Output:

arduino
Copy code
Process 123 loaded from backing store.
Key Features and Considerations
File Structure:

The file content is human-readable, with each attribute saved in a "key: value" format.
Example file content:
yaml
Copy code
Process ID: 123
Process Name: ExampleProcess
Current Instruction Line: 45
Core ID: 2
Created At: Thu Nov 28 15:30:00 2024
Finished At: Thu Nov 28 16:00:00 2024
Total Instructions: 120
Time Parsing:

Converts string timestamps (createdAt, finishedAt) into std::tm objects for reuse.
Uses std::put_time for formatting timestamps when saving and std::get_time when loading.
Error Handling:

If the file cannot be opened for saving or loading, prints an appropriate error message.
Provides feedback on time parsing errors for debugging.
