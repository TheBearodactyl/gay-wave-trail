#ifndef GAY_SETTINGS_HPP
#define GAY_SETTINGS_HPP

#include <utility>
#include <vector>
#pragma once

#include <Geode/Geode.hpp>

#include <gay/settings/color_list.hpp>
#include <gay/types/color.hpp>

using namespace geode::prelude;

namespace gay::settings {
	enum class CheckMode { All, Any, None };

	template<typename T>
	inline T get(const std::string& name) {
		return Mod::get()->getSettingValue<T>(name);
	}

	template<CheckMode Mode, typename... T>
	inline bool check(const T&... settings) {
		if constexpr (Mode == CheckMode::All) {
			return (get<bool>(settings) and ...);
		} else if (Mode == CheckMode::Any) {
			return (get<bool>(settings) or ...);
		} else {
			return !(get<bool>(settings) or ...);
		}
	}

	template<CheckMode Mode, typename... Names>
	std::pair<bool, std::vector<std::pair<std::string, bool>>> check_detailed(const Names&... settings) {
		std::vector<std::pair<std::string, bool>> results = {{settings, get<bool>(settings)}...};
		bool check_result;

		if constexpr (Mode == CheckMode::All) {
			check_result = (get<bool>(settings) and ...);
		} else if constexpr (Mode == CheckMode::Any) {
			check_result = (get<bool>(settings) or ...);
		} else {
			check_result = !(get<bool>(settings) or ...);
		}

		return {check_result, results};
	}

	inline float get_float(const std::string& name) {
		return static_cast<float>(Mod::get()->getSettingValue<double>(name));
	}

	inline bool get_mod_enabled() {
		return get<bool>("enabled");
	}
} // namespace gay::settings

#endif
