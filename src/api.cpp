#include <gay/api.hpp>
#include <gay/ui/settings_popup.hpp>

namespace gay {
	ModeText get_mode_text(const SettingDisplayInfo& info, int mode_idx) {
		if (mode_idx >= 0 && mode_idx < 3) {
			return info.mode_text[static_cast<size_t>(mode_idx)];
		}

		auto& custom_modes = api::get_custom_modes();
		int custom_idx = mode_idx - 3;

		if (custom_idx >= 0 && custom_idx < static_cast<int>(custom_modes.size())) {
			auto& cm = custom_modes[static_cast<size_t>(custom_idx)];
			auto it = cm.setting_overrides.find(std::string(info.key));

			if (it != cm.setting_overrides.end()) {
				return ModeText {it->second.name, it->second.desc};
			}
		}

		return info.mode_text[1];
	}

	std::string_view get_section_title(const SectionInfo& section, int mode_idx) {
		if (mode_idx >= 0 && mode_idx < 3) {
			return section.titles[static_cast<size_t>(mode_idx)];
		}

		auto& custom_modes = api::get_custom_modes();
		int custom_idx = mode_idx - 3;

		if (custom_idx >= 0 && custom_idx < static_cast<int>(custom_modes.size())) {
			auto& cm = custom_modes[static_cast<size_t>(custom_idx)];
			auto it = cm.section_overrides.find(std::string(section.key));

			if (it != cm.section_overrides.end()) {
				return it->second;
			}
		}

		return section.titles[1];
	}
} // namespace gay

namespace gay::api {
	static std::vector<CustomMode>& modes() {
		static std::vector<CustomMode> s_modes;
		return s_modes;
	}

	std::vector<CustomMode>& get_custom_modes() {
		return modes();
	}

	int register_mode(std::string_view dropdown_label) {
		int idx = 3 + static_cast<int>(modes().size());
		modes().push_back(CustomMode {std::string(dropdown_label), {}, {}});
		return idx;
	}

	void set_mode_text(int mode_index, std::string_view setting_key, std::string_view name, std::string_view desc) {
		int custom_idx = mode_index - 3;

		if (custom_idx < 0 || custom_idx >= static_cast<int>(modes().size())) {
			return;
		}

		modes()[static_cast<size_t>(custom_idx)].setting_overrides[std::string(setting_key)] =
			OwnedModeText {std::string(name), std::string(desc)};
	}

	void set_section_title(int mode_index, std::string_view section_key, std::string_view title) {
		int custom_idx = mode_index - 3;

		if (custom_idx < 0 || custom_idx >= static_cast<int>(modes().size())) {
			return;
		}

		modes()[static_cast<size_t>(custom_idx)].section_overrides[std::string(section_key)] = std::string(title);
	}

	int mode_count() {
		return 3 + static_cast<int>(modes().size());
	}
} // namespace gay::api
