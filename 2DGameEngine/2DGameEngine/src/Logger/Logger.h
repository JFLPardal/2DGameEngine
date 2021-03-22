#pragma once

#include <string>
#include <vector>

struct LogEntry;

class Logger
{
public:
	static void Log(const std::string& messageToDisplay);
	static void Error(const std::string& messageToDisplay);
	static std::string CurrentDateTimeToString();
private:
	static std::vector<LogEntry> m_messages;
};

enum class LogType
{
	info,
	warning,
	error
};

struct LogEntry
{
	LogType m_type;
	std::string m_message;
};

