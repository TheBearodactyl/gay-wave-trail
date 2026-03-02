#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>

#include <fmt/base.h>

using namespace geode::prelude;

template<typename T>
using Vec = std::vector<T>;

template<typename T>
using Deque = std::deque<T>;

template<typename T, typename U>
using Pair = std::pair<T, U>;

template<typename T, size_t Size>
using Array = std::array<T, Size>;

template<typename T>
using Option = std::optional<T>;

template<typename T>
using Atomic = std::atomic<T>;

template<typename T>
using LockGuard = std::lock_guard<T>;

template<typename T>
using UniqueLock = std::unique_lock<T>;

#define to(type, var) static_cast<type>(var)

using Mutex = std::mutex;
using Thread = std::thread;
using CondVar = std::condition_variable;

#define None std::nullopt

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

	template<typename... Args>
	void debug(fmt::format_string<Args...> msg, Args&&... args) {
		if (!Mod::get()->getSettingValue<bool>("verbose-logging")) {
			return;
		}

		gay::util::log(LogLevel::Debug, msg, std::forward<Args>(args)...);
	}

	struct FuzzyResult {
		bool matched = false;
		int score = 0;
	};

	static FuzzyResult fuzzy_match(const std::string& query, const std::string& target) {
		if (query.empty()) {
			gay::util::log(gay::util::LogLevel::Debug, "Query empty");
			return {true, 0};
		}

		auto to_lower = [](char c) {
			return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		};

		size_t qi = 0;
		int score = 0;
		int consecutive = 0;
		size_t last_match = std::string::npos;

		for (size_t ti = 0; ti < target.size() && qi < query.size(); ti++) {
			if (to_lower(query[qi]) == to_lower(target[ti])) {
				score += 1;

				if (last_match != std::string::npos && ti == last_match + 1) {
					consecutive++;
					score += consecutive * 2;
				} else {
					consecutive = 0;
				}

				if (ti == 0 || target[ti - 1] == ' ' || target[ti - 1] == '-' ||
					target[ti - 1] == '_') {
					score += 3;
				}

				last_match = ti;
				qi++;
			}
		}

		return {qi == query.size(), score};
	}

	static constexpr ccColor4B rp_c4(ccColor3B c, uint8_t a) {
		return {c.r, c.g, c.b, a};
	}
} // namespace gay::util
