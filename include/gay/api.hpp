#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Geode/loader/Mod.hpp>

namespace gay {
	struct ModeText;
}

namespace gay::api {
	struct OwnedModeText {
		std::string name;
		std::string desc;
	};

	struct CustomMode {
		std::string label;
		std::unordered_map<std::string, OwnedModeText> setting_overrides;
		std::unordered_map<std::string, std::string> section_overrides;
	};

	GEODE_DLL int register_mode(std::string_view dropdown_label);
	GEODE_DLL void set_mode_text(int mode_idx, std::string_view key, std::string_view name, std::string_view desc);
	GEODE_DLL void set_section_title(int mode_idx, std::string_view key, std::string_view title);
	GEODE_DLL int mode_count();

	std::vector<CustomMode>& get_custom_modes();
} // namespace gay::api
