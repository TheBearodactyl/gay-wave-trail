#include <gay/settings.hpp>

using namespace geode::prelude;

$execute {
	(void)Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);
}
