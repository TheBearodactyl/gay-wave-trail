#if __has_include("eclipse.eclipse-menu/include/eclipse.hpp")
	#include <Geode/Geode.hpp>
	#include <Geode/loader/Loader.hpp>
	#include <Geode/loader/Mod.hpp>
	#include <Geode/loader/ModEvent.hpp>

	#include <string>
	#include <vector>

	#include <eclipse.eclipse-menu/include/components.hpp>
	#include <eclipse.eclipse-menu/include/config.hpp>

using namespace eclipse;
using namespace geode::prelude;

void create_bool_setting(const char* id, MenuTab& tab) {
	auto setting = Mod::get()->getSetting(id);

	tab.addToggle(
		   Mod::get()->expandSpriteName(id).data(),
		   setting->getDisplayName(),
		   [id](bool val) {
			   Mod::get()->setSettingValue<bool>(id, val);
			   eclipse::config::set<bool>(Mod::get()->expandSpriteName(id).data(), val);
		   }
	).setDescription(setting->getDescription().value_or(""));

	eclipse::config::set<bool>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<bool>(id));

	listenForSettingChanges(id, [id](bool _) {
		eclipse::config::set<bool>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<bool>(id));
	});
}

void create_float_setting(const char* id, MenuTab& tab) {
	auto setting = Mod::get()->getSetting(id);

	tab.addInputFloat(
		   Mod::get()->expandSpriteName(id).data(),
		   setting->getDisplayName(),
		   [id](double val) {
			   Mod::get()->setSettingValue<double>(id, val);
			   eclipse::config::set<double>(Mod::get()->expandSpriteName(id).data(), val);
		   }
	).setDescription(setting->getDescription().value_or(""));

	eclipse::config::set<double>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<double>(id));

	listenForSettingChanges(id, [id](double _) {
		eclipse::config::set<double>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<double>(id));
	});
}

std::vector<std::string> bool_settings = {
	"enabled",
	"add-min-width",
	"smooth-colors",
	"solid-trail",
	"editor-trail",
	"persist-trail",
	"disable-pulse",
	"disable-trail",
	"enable-gaydient",
	"tilt-enabled",
};

std::vector<std::string> float_settings = {
	"speed",
	"saturation",
	"trail-size",
	"pulse-size",
	"tilt-x-offset",
	"tilt-y-offset",
};

$on_mod(Loaded) {
	Loader::get()->queueInMainThread([] {
		auto toggles_tab = MenuTab::find("Gay Wave Trail - Toggles");
		auto floats_tab = MenuTab::find("Gay Wave Trail - Inputs");

		for (auto bool_setting : bool_settings) {
			create_bool_setting(bool_setting.data(), toggles_tab);
		}

		for (auto float_setting : float_settings) {
			create_float_setting(float_setting.data(), floats_tab);
		}
	});
}
#endif
