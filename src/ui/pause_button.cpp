// Credits go to RayDeeUx for the pause button code :D

#include <Geode/modify/Modify.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;
using namespace gay::settings;

class $modify(GayPauseLayer, PauseLayer) {
	void onGaySettings(CCObject*) {
		geode::openSettingsPopup(Mod::get(), false);
	}

	void customSetup() override {
		PauseLayer::customSetup();

		if (gay::settings::get<bool>("enable-pause-btn")) {
			auto settingsButtonSprite = CCSprite::create("button.png"_spr);
			settingsButtonSprite->setScale(.25f);
			if (auto betterPause = Loader::get()->getLoadedMod("tpdea.betterpause-Better")) {
				if (betterPause->getSettingValue<int64_t>("type-pause") == 1)
					settingsButtonSprite->setScale(.95f);
			}

			auto settingsBtn = CCMenuItemSpriteExtra::create(settingsButtonSprite, this, menu_selector(GayPauseLayer::onGaySettings));

			settingsBtn->setID("gay-settings-button"_spr);
			if (auto leftMenu = getChildByIDRecursive("left-button-menu")) {
				leftMenu->addChild(settingsBtn);
				leftMenu->updateLayout();
			}
		}
	}
};
