#if __has_include("eclipse.eclipse-menu/include/eclipse.hpp")

#include <algorithm>
#include <optional>
#include <ranges>
#include <string>

#include <gay/setting_defs.hpp>

#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModEvent.hpp>

#include <eclipse.eclipse-menu/include/components.hpp>
#include <eclipse.eclipse-menu/include/config.hpp>
#include <eclipse.eclipse-menu/include/events.hpp>

using namespace eclipse;
using namespace geode::prelude;

static bool is_gay_mode() {
	return Mod::get()->getSettingValue<int64_t>("display-mode") == 0;
}

[[nodiscard]]
static std::optional<const gay::SettingDisplayInfo*> find_info(std::string_view id) {
	auto it = std::ranges::find_if(
		SETTING_DISPLAY_INFO,
		[id](std::string_view k) { return k == id; },
		&gay::SettingDisplayInfo::key
	);

	if (it == std::ranges::end(SETTING_DISPLAY_INFO)) {
		return std::nullopt;
	}

	return &*it;
}

[[nodiscard]]
static std::string find_name(std::string_view id) {
	if (auto info = find_info(id)) {
		return std::string(is_gay_mode() ? (*info)->gay_name : (*info)->straight_name);
	}

	return std::string(id);
}

[[nodiscard]]
static std::string find_desc(std::string_view id) {
	if (auto info = find_info(id)) {
		return std::string(is_gay_mode() ? (*info)->gay_desc : (*info)->straight_desc);
	}
	return {};
}

[[nodiscard]]
static std::string config_key(const char* id) {
	return std::string(Mod::get()->expandSpriteName(id));
}

static components::Toggle register_bool(const char* id, MenuTab& tab) {
	const auto ckey = config_key(id);

	auto toggle = tab.addToggle(
						 ckey,
						 find_name(id),
						 [id, ckey](bool val) {
							 Mod::get()->setSettingValue<bool>(id, val);
							 config::set<bool>(ckey, val);
						 }
	).setDescription(find_desc(id));

	config::set<bool>(ckey, Mod::get()->getSettingValue<bool>(id));
	listenForSettingChanges<bool>(id, [id, ckey](bool) {
		config::set<bool>(ckey, Mod::get()->getSettingValue<bool>(id));
	});

	return toggle;
}

static components::InputFloat register_float(
	const char* id,
	MenuTab& tab,
	std::optional<float> min = std::nullopt,
	std::optional<float> max = std::nullopt
) {
	const auto ckey = config_key(id);

	auto input = tab.addInputFloat(
						find_name(id),
						ckey,
						[id, ckey](float val) {
							Mod::get()->setSettingValue<double>(id, static_cast<double>(val));
							config::set<double>(ckey, static_cast<double>(val));
						}
	).setDescription(find_desc(id));

	if (min) {
		input.setMinValue(*min);
	}
	if (max) {
		input.setMaxValue(*max);
	}

	config::set<double>(ckey, Mod::get()->getSettingValue<double>(id));
	listenForSettingChanges<double>(id, [id, ckey](double) {
		config::set<double>(ckey, Mod::get()->getSettingValue<double>(id));
	});

	return input;
}

static components::InputFloat register_int(
	const char* id,
	MenuTab& tab,
	std::optional<float> min = std::nullopt,
	std::optional<float> max = std::nullopt
) {
	const auto ckey = config_key(id);

	auto input = tab.addInputFloat(
						find_name(id),
						ckey,
						[id, ckey](float val) {
							const auto rounded = static_cast<int64_t>(std::round(val));
							Mod::get()->setSettingValue<int64_t>(id, rounded);
							config::set<int>(ckey, static_cast<int>(rounded));
						}
	)
					 .setDescription(find_desc(id))
					 .setFormat("%.0f");

	if (min) {
		input.setMinValue(*min);
	}
	if (max) {
		input.setMaxValue(*max);
	}

	config::set<int>(ckey, static_cast<int>(Mod::get()->getSettingValue<int64_t>(id)));
	listenForSettingChanges<int64_t>(id, [id, ckey](int64_t) {
		config::set<int>(ckey, static_cast<int>(Mod::get()->getSettingValue<int64_t>(id)));
	});

	return input;
}

static void register_display_mode(MenuTab& tab) {
	static constexpr const char* ID = "display-mode";

	const auto key_gay = config_key("display-mode-gay");
	const auto key_straight = config_key("display-mode-straight");
	const auto desc = find_desc(ID);

	tab.addLabel("Display Mode");

	tab.addToggle(
		   key_gay,
		   "Gay mode BABYYYYYYYY :3333",
		   [key_straight](bool val) {
			   const int64_t mode = val ? 0 : 1;
			   Mod::get()->setSettingValue<int64_t>(ID, mode);
			   config::set<bool>(key_straight, !val);
		   }
	).setDescription(desc);

	tab.addToggle(
		   key_straight,
		   "straight mode.",
		   [key_gay](bool val) {
			   const int64_t mode = val ? 1 : 0;
			   Mod::get()->setSettingValue<int64_t>(ID, mode);
			   config::set<bool>(key_gay, !val);
		   }
	).setDescription(desc);

	const bool is_gay = Mod::get()->getSettingValue<int64_t>(ID) == 0;
	config::set<bool>(key_gay, is_gay);
	config::set<bool>(key_straight, !is_gay);

	listenForSettingChanges<int64_t>(ID, [key_gay, key_straight](int64_t idx) {
		config::set<bool>(key_gay, idx == 0);
		config::set<bool>(key_straight, idx == 1);
	});
}

static constexpr std::string_view TAB_GENERAL = "Gay Wave Trail - General";
static constexpr std::string_view TAB_COLORS = "Gay Wave Trail - Colors";
static constexpr std::string_view TAB_SHAPE = "Gay Wave Trail - Trail Shape";
static constexpr std::string_view TAB_OPACITY = "Gay Wave Trail - Trail Opacity";
static constexpr std::string_view TAB_OUTLINE = "Gay Wave Trail - Outline";
static constexpr std::string_view TAB_TILT = "Gay Wave Trail - Tilt";
static constexpr std::string_view TAB_SEGMENTS = "Gay Wave Trail - Segments";
static constexpr std::string_view TAB_THICKNESS = "Gay Wave Trail - Thickness";
static constexpr std::string_view TAB_MISC = "Gay Wave Trail - Misc";

$on_mod(Loaded) {
	Loader::get()->queueInMainThread([] {
		const auto tab = [](std::string_view name) { return MenuTab::find(std::string(name)); };

		auto general = tab(TAB_GENERAL);
		auto colors = tab(TAB_COLORS);
		auto shape = tab(TAB_SHAPE);
		auto opacity = tab(TAB_OPACITY);
		auto outline = tab(TAB_OUTLINE);
		auto tilt = tab(TAB_TILT);
		auto segments = tab(TAB_SEGMENTS);
		auto thickness = tab(TAB_THICKNESS);
		auto misc = tab(TAB_MISC);

		register_display_mode(general);
		register_bool("enabled", general);
		register_float("speed", general, 0.f, 10.f);
		register_bool("disable-behind-streak", general);
		register_float("saturation", general, 0.f, 100.f);
		register_float("trail-brightness", general, 0.f, 3.f);
		register_bool("sync-to-music", general);

		register_bool("smooth-colors", colors);
		register_bool("enable-gaydient", colors);
		register_bool("separate-player-colors", colors);

		register_bool("solid-trail", shape);
		register_bool("rewrite-trail", shape);
		register_float("trail-size", shape, 0.1f, 5.f);
		register_float("pulse-size", shape, 0.f, 5.f);
		register_bool("disable-trail", shape);
		register_bool("disable-pulse", shape);

		register_int("trail-opacity", opacity, 0.f, 255.f);
		register_bool("enable-trail-fade", opacity);
		register_float("trail-fade-speed", opacity, 0.f, 5.f);
		register_int("trail-min-opacity", opacity, 0.f, 255.f);
		register_bool("persist-trail", opacity);

		register_bool("wave-outline", outline);
		register_float("wave-outline-width", outline, 0.f, 20.f);
		register_int("wave-outline-blur", outline, 0.f, 10.f);
		register_int("wave-outline-opacity", outline, 0.f, 255.f);

		register_bool("tilt-enabled", tilt);
		register_float("tilt-x-offset", tilt, -50.f, 50.f);
		register_float("tilt-y-offset", tilt, -50.f, 50.f);
		register_bool("trail-dashing", tilt);

		register_bool("trail-segments", segments);
		register_float("segment-length", segments, 1.f, 200.f);
		register_float("segment-gap", segments, 0.f, 100.f);
		register_bool("segment-persist", segments);

		register_bool("add-min-width", thickness);
		register_bool("trail-thickness-variation", thickness);
		register_float("thickness-multiplier", thickness, 0.f, 5.f);

		register_bool("editor-trail", misc);
		register_bool("enable-pause-btn", misc);
		register_bool("verbose-logging", misc);
	});
}

#endif
