// Credits go to RayDeeUx for the pause button code :D

#include <Geode/modify/Modify.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class $modify(GayPauseLayer, PauseLayer) {
  void onGaySettings(CCObject *) {// NOLINT(*-convert-member-functions-to-static)
    geode::openSettingsPopup(Mod::get(), false);
  }

  void customSetup() override {
    PauseLayer::customSetup();

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
};