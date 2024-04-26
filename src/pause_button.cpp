#include <Geode/modify/Modify.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

struct GayPauseLayer : Modify<GayPauseLayer, PauseLayer> {
  void onGaySettings(CCObject *object) {
    geode::openSettingsPopup(Mod::get());
  }

  void customSetup() {
    PauseLayer::customSetup();

    auto settingsButtonSprite = CCSprite::create("logo.png"_spr);
    settingsButtonSprite->setScale(.95f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
      settingsButtonSprite, 
      this,
      menu_selector(GayPauseLayer::onGaySettings)
    );

    settingsBtn->setID("gay-settings-button"_spr);
    if (auto leftMenu = getChildByIDRecursive("left-button-menu")) {
      leftMenu->addChild(settingsBtn);
      leftMenu->updateLayout();
    }
  }
};