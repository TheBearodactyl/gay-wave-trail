#include <Geode/modify/PlayLayer.hpp>

#include "settings/gay_settings.hpp"
#include "settings/color_list/color_list_setting.hpp"
#include "trail_customization/rainbow_trail.hpp"
#include "constants.hpp"

using namespace geode::prelude;

$execute {
    (void) Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);
    (void) Mod::get()->registerCustomSettingType("outline-colors", ColorListSetting::parse);
}