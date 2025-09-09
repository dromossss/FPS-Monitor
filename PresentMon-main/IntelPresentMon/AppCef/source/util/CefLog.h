#pragma once
#include <include/internal/cef_types.h>
#include "Logging.h"

constexpr cef_log_severity_t ToCefLogLevel(::pmon::util::log::Level level)
{
	using ::pmon::util::log::Level;
	switch (level) {
	case Level::Verbose: return cef_log_severity_t::LOGSEVERITY_VERBOSE;
	case Level::Debug: // in CEF, debug and verbose are the same, so we use cef.info for debug
	case Level::Performance:
	case Level::Info: return cef_log_severity_t::LOGSEVERITY_INFO;
	case Level::Warning: return cef_log_severity_t::LOGSEVERITY_WARNING;
	case Level::Error: return cef_log_severity_t::LOGSEVERITY_ERROR;
	case Level::Fatal: return cef_log_severity_t::LOGSEVERITY_FATAL;
	default: return cef_log_severity_t::LOGSEVERITY_DEFAULT;
	}
}