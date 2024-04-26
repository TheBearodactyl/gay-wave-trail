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
    auto ccmenu = this->getChildByIDRecursive("left-button-menu");

    auto *settingsButtonSprite = CCSprite::create("logo.png"_spr);

    settingsButtonSprite->setScale(0.25f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
      settingsButtonSprite, 
      this,
      menu_selector(GayPauseLayer::onGaySettings)
    );

    settingsBtn->setID("gay-settings-button"_spr);
    settingsBtn->setPosition(CCPoint{ 45, 40 });
    ccmenu->addChild(settingsBtn);

    this->addChild(ccmenu);
  }
};
