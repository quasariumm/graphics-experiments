module;

#include <cassert>
#include <vector>

#include <format>

#include <windows.h>

export module dx_wrapper.core.log;

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
	LogType		m_type;
	std::string m_message;
	uint32_t	m_count;

	bool operator==(const DebugLog& rhs) const { return m_message == rhs.m_message; }
};

static constexpr auto MAGENTA = "\033[35m";
static constexpr auto GREEN	  = "\033[32m";
static constexpr auto RED	  = "\033[31m";
static constexpr auto RESET	  = "\033[0m";

inline std::vector<DebugLog> gLogs = {};

export {
template <typename... Args>
void Info(std::format_string<Args...> fmt, Args&&... args)
{
	printf("[%sINFO%s] ", GREEN, RESET);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Info, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Warning(std::format_string<Args...> fmt, Args&&... args)
{
	printf("[%sWARN%s] ", MAGENTA, RESET);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Warning, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Error(std::format_string<Args...> fmt, Args&&... args)
{
	printf("[%sERR!%s] ", RED, RESET);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	printf("%s\n", message.c_str());

	auto log = DebugLog{LogType::Error, message, 1};
	if (!gLogs.empty() && gLogs.back() == log)
		gLogs.back().m_count++;
	else
		gLogs.push_back(log);
}

template <typename... Args>
void Critical(std::format_string<Args...> fmt, Args&&... args)
{
	printf("[%sCRIT%s] ", RED, RESET);
	const std::string message = std::format(fmt, std::forward<Args>(args)...);
	printf("%s\n", message.c_str());

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
