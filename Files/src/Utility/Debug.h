#ifdef _DEBUG
#pragma once
#include <string>

namespace Debug
{
#define WRITE_LOG(msg) WriteDebugLog(msg, __FILE__, __LINE__)

	void WriteDebugLog(const std::string& msg, const char* file, int line);
}

#endif
