#pragma once

#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>

#include <fmt/base.h>

using namespace geode::prelude;

namespace gay::util {
	enum class LogLevel {
		Debug,
		Info,
		Warn,
		Error,
	};

	template<typename... Args>
	void log(LogLevel lvl, fmt::format_string<Args...> msg, Args&&... args) {
		if (!Mod::get()->getSettingValue<bool>("verbose-logging")) {
			return;
		}

		switch (lvl) {
			case LogLevel::Debug:
				log::debug(msg, std::forward<Args>(args)...);
				break;
			case LogLevel::Info:
				log::info(msg, std::forward<Args>(args)...);
				break;
			case LogLevel::Warn:
				log::warn(msg, std::forward<Args>(args)...);
				break;
			case LogLevel::Error:
				log::error(msg, std::forward<Args>(args)...);
				break;
		}
	}
} // namespace gay::util
