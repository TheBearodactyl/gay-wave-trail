#include <Geode/Geode.hpp>
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

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ccmenu = CCMenu::create();

    ccmenu->setPosition(CCPoint{0, 0});
    ccmenu->setID("gay-settings-button-menu"_spr);

    auto *settingsButtonSprite = CCSprite::create("logo.png"_spr);

    settingsButtonSprite->setScale(0.25f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
      settingsButtonSprite, 
      this,
      menu_selector(GayPauseLayer::onGaySettings)
    );

    settingsBtn->setID("gay-settings-button"_spr);
    settingsBtn->setPosition(CCPoint{ 30, 30 });
    ccmenu->addChild(settingsBtn);

    this->addChild(ccmenu);
  }
};
