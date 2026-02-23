#if 0 // __has_include("eclipse.eclipse-menu/include/eclipse.hpp")

	#include <string>
	#include <vector>

	#include <Geode/Geode.hpp>
	#include <Geode/loader/Loader.hpp>
	#include <Geode/loader/Mod.hpp>
	#include <Geode/loader/ModEvent.hpp>

	#include <eclipse.eclipse-menu/include/components.hpp>
	#include <eclipse.eclipse-menu/include/config.hpp>

using namespace eclipse;
using namespace geode::prelude;

static void create_bool_setting(const char* id, MenuTab& tab) {
    auto setting = Mod::get()->getSetting(id);
    tab.addToggle(
        Mod::get()->expandSpriteName(id).data(),
        setting->getDisplayName(),
        [id](bool val) {
            Mod::get()->setSettingValue<bool>(id, val);
            config::set<bool>(Mod::get()->expandSpriteName(id).data(), val);
        }
    ).setDescription(setting->getDescription().value_or(""));

    config::set<bool>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<bool>(id));

    listenForSettingChanges(id, [id](bool) {
        config::set<bool>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<bool>(id));
    });
}

static void create_float_setting(const char* id, MenuTab& tab) {
    auto setting = Mod::get()->getSetting(id);
    tab.addInputFloat(
        Mod::get()->expandSpriteName(id).data(),
        setting->getDisplayName(),
        [id](double val) {
            Mod::get()->setSettingValue<double>(id, val);
            config::set<double>(Mod::get()->expandSpriteName(id).data(), val);
        }
    ).setDescription(setting->getDescription().value_or(""));

    config::set<double>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<double>(id));

    listenForSettingChanges(id, [id](double) {
        config::set<double>(Mod::get()->expandSpriteName(id).data(), Mod::get()->getSettingValue<double>(id));
    });
}

static const std::vector<std::string> BOOL_SETTINGS = {
    "enabled", "add-min-width", "smooth-colors", "solid-trail",
    "editor-trail", "persist-trail", "disable-pulse", "disable-trail",
    "enable-gaydient", "tilt-enabled",
};

static const std::vector<std::string> FLOAT_SETTINGS = {
    "speed", "saturation", "trail-size", "pulse-size",
    "tilt-x-offset", "tilt-y-offset",
};

$on_mod(Loaded) {
    Loader::get()->queueInMainThread([] {
        auto toggles_tab = MenuTab::find("Gay Wave Trail - Toggles");
        auto floats_tab = MenuTab::find("Gay Wave Trail - Inputs");

        for (const auto& id : BOOL_SETTINGS) {
            create_bool_setting(id.data(), toggles_tab);
        }
        for (const auto& id : FLOAT_SETTINGS) {
            create_float_setting(id.data(), floats_tab);
        }
    });
}

#endif
