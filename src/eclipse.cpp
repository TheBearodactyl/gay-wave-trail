#if __has_include("eclipse.eclipse-menu/include/eclipse.hpp")

#include <string>

#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModEvent.hpp>

#include <eclipse.eclipse-menu/include/components.hpp>
#include <eclipse.eclipse-menu/include/config.hpp>
#include <eclipse.eclipse-menu/include/events.hpp>

using namespace eclipse;
using namespace geode::prelude;

static std::string key(const char* id) {
	return std::string(Mod::get()->expandSpriteName(id));
}

static components::Toggle register_bool(const char* id, MenuTab& tab) {
	auto setting = Mod::get()->getSetting(id);
	auto toggle = tab.addToggle(
						 key(id),
						 std::string(setting->getDisplayName()),
						 [id](bool val) {
							 Mod::get()->setSettingValue<bool>(id, val);
							 config::set<bool>(key(id), val);
						 }
	).setDescription(std::string(setting->getDescription().value_or("")));

	config::set<bool>(key(id), Mod::get()->getSettingValue<bool>(id));
	listenForSettingChanges<bool>(id, [id](bool) {
		config::set<bool>(key(id), Mod::get()->getSettingValue<bool>(id));
	});

	return toggle;
}

static components::InputFloat register_float(
	const char* id,
	MenuTab& tab,
	std::optional<float> min = std::nullopt,
	std::optional<float> max = std::nullopt
) {
	auto setting = Mod::get()->getSetting(id);
	auto input = tab.addInputFloat(
						key(id),
						std::string(setting->getDisplayName()),
						[id](float val) {
							Mod::get()->setSettingValue<double>(id, static_cast<double>(val));
							config::set<double>(key(id), static_cast<double>(val));
						}
	).setDescription(std::string(setting->getDescription().value_or("")));

	if (min) {
		input.setMinValue(*min);
	}
	if (max) {
		input.setMaxValue(*max);
	}

	config::set<double>(key(id), Mod::get()->getSettingValue<double>(id));
	listenForSettingChanges<double>(id, [id](double) {
		config::set<double>(key(id), Mod::get()->getSettingValue<double>(id));
	});

	return input;
}

static components::InputFloat register_int(
	const char* id,
	MenuTab& tab,
	std::optional<float> min = std::nullopt,
	std::optional<float> max = std::nullopt
) {
	auto setting = Mod::get()->getSetting(id);
	auto input = tab.addInputFloat(
						key(id),
						std::string(setting->getDisplayName()),
						[id](float val) {
							auto rounded = static_cast<int64_t>(std::round(val));
							Mod::get()->setSettingValue<int64_t>(id, rounded);

							config::set<int>(key(id), static_cast<int>(rounded));
						}
	)
					 .setDescription(std::string(setting->getDescription().value_or("")))
					 .setFormat("%.0f");

	if (min) {
		input.setMinValue(*min);
	}
	if (max) {
		input.setMaxValue(*max);
	}

	config::set<int>(key(id), static_cast<int>(Mod::get()->getSettingValue<int64_t>(id)));
	listenForSettingChanges<int64_t>(id, [id](int64_t) {
		config::set<int>(key(id), static_cast<int>(Mod::get()->getSettingValue<int64_t>(id)));
	});

	return input;
}

static void register_display_mode(MenuTab& tab) {
	static constexpr const char* ID = "display-mode";

	auto key_gay = key("display-mode-gay");
	auto key_straight = key("display-mode-straight");

	auto setting = Mod::get()->getSetting(ID);
	auto desc = std::string(setting->getDescription().value_or(""));

	tab.addLabel("Display Mode");

	tab.addToggle(
		   key_gay,
		   "Gay mode BABYYYYYYYY :3333",
		   [key_straight](bool val) {
			   if (val) {
				   Mod::get()->setSettingValue<int64_t>(ID, 0);
				   config::set<bool>(key_straight, false);
			   } else {
				   Mod::get()->setSettingValue<int64_t>(ID, 1);
				   config::set<bool>(key_straight, true);
			   }
		   }
	).setDescription(desc);

	tab.addToggle(
		   key_straight,
		   "straight mode.",
		   [key_gay](bool val) {
			   if (val) {
				   Mod::get()->setSettingValue<int64_t>(ID, 1);
				   config::set<bool>(key_gay, false);
			   } else {
				   Mod::get()->setSettingValue<int64_t>(ID, 0);
				   config::set<bool>(key_gay, true);
			   }
		   }
	).setDescription(desc);

	bool is_gay = Mod::get()->getSettingValue<int64_t>(ID) == 0;
	config::set<bool>(key_gay, is_gay);
	config::set<bool>(key_straight, !is_gay);

	listenForSettingChanges<int64_t>(ID, [key_gay, key_straight](int64_t idx) {
		config::set<bool>(key_gay, idx == 0);
		config::set<bool>(key_straight, idx == 1);
	});
}

static constexpr const char* TAB_GENERAL = "Gay Wave Trail - General";
static constexpr const char* TAB_COLORS = "Gay Wave Trail - Colors";
static constexpr const char* TAB_SHAPE = "Gay Wave Trail - Trail Shape";
static constexpr const char* TAB_OPACITY = "Gay Wave Trail - Trail Opacity";
static constexpr const char* TAB_OUTLINE = "Gay Wave Trail - Outline";
static constexpr const char* TAB_TILT = "Gay Wave Trail - Tilt";
static constexpr const char* TAB_SEGMENTS = "Gay Wave Trail - Segments";
static constexpr const char* TAB_THICKNESS = "Gay Wave Trail - Thickness";
static constexpr const char* TAB_MISC = "Gay Wave Trail - Misc";

$on_mod(Loaded) {
	Loader::get()->queueInMainThread([] {
		auto general = MenuTab::find(TAB_GENERAL);
		auto colors = MenuTab::find(TAB_COLORS);
		auto shape = MenuTab::find(TAB_SHAPE);
		auto opacity = MenuTab::find(TAB_OPACITY);
		auto outline = MenuTab::find(TAB_OUTLINE);
		auto tilt = MenuTab::find(TAB_TILT);
		auto segments = MenuTab::find(TAB_SEGMENTS);
		auto thickness = MenuTab::find(TAB_THICKNESS);
		auto misc = MenuTab::find(TAB_MISC);

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
