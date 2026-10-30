#ifdef _DEBUG
#include <windows.h>
#include "Debug.h"

void Debug::WriteDebugLog(const std::string& msg, const char* file, int line)
{
	std::string write = msg + "\n";
	write += "File: " + std::string(file) + "\n";
	write += "Line: " + std::to_string(line) + "\n";

	OutputDebugString(write.c_str());
}

#endif