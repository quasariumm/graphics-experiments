module;

#include <cassert>
#include <windows.h>

export module dx_wrapper.core.log;
import std;

namespace Log
{
enum class LogType
{
	Info,
	Warning,
	Error,
	Critical
};

struct DebugLog
{
	LogType		  m_type;
	std::string	  m_message;
	std::uint32_t m_count;

	bool operator==(const DebugLog& rhs) const { return m_message == rhs.m_message; }
};

namespace
{
	inline constexpr auto magenta = "\033[35m";
	inline constexpr auto green	  = "\033[32m";
	inline constexpr auto red	  = "\033[31m";
	inline constexpr auto reset	  = "\033[0m";
} // namespace

std::vector<DebugLog> gLogs = {};

export {
template <typename... Args>
void Info(std::format_string<Args...> fmt, Args&&... args)
{
	std::printf("[%sINFO%s] ", green, reset);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	std::printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Info, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Warning(std::format_string<Args...> fmt, Args&&... args)
{
	std::printf("[%sWARN%s] ", magenta, reset);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	std::printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Warning, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Error(std::format_string<Args...> fmt, Args&&... args)
{
	std::printf("[%sERR!%s] ", red, reset);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	std::printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Error, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Critical(std::format_string<Args...> fmt, Args&&... args)
{
	std::printf("[%sCRIT%s] ", red, reset);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	std::printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Critical, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);

	MessageBoxA(nullptr, message.c_str(), "Critical error", MB_OK | MB_ICONERROR);
	assert(false);
}
}

} // namespace Log
