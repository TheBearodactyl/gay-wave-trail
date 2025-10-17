#include <Geode/modify/PlayLayer.hpp>

#include <gay/customization/rainbow.hpp>
#include <gay/settings/color_list.hpp>

using namespace geode::prelude;

$execute {
	(void)Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);
}
