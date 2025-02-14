#include "multi_string_setting.hpp"
#include "../ui/colors_popup.hpp"
#include "Geode/loader/SettingV3.hpp"
#include <memory>
#include <vector>

using namespace geode::prelude;

SettingNodeV3 *ColorListSettingV3::createNode(float width) {
  return ColorListSettingNode::create(std::static_pointer_cast<ColorListSettingV3>(shared_from_this()), width);
}


bool ColorListSettingNode::init(std::shared_ptr<ColorListSettingV3> setting, float width) {
  if (! SettingValueNodeV3::init(setting, width)) {
    return false;
  }

  float height = 40.0f;
  this->setContentSize({width, height});
  CCMenu *menu = CCMenu::create();
  menu->setPosition({0.0f, 0.0f});
  menu->setID("inputs-menu");

  auto viewSpr = ButtonSprite::create("E N T");
  viewSpr->setScale(0.72f);

  auto viewBtn = CCMenuItemSpriteExtra::create(viewSpr, this, menu_selector(ColorListSettingNode::onView));
  viewBtn->setPosition(width - 40.f, height - 20.f);
  menu->addChild(viewBtn);

  this->addChild(menu);
  handleTouchPriority(this);

  return true;
}

void ColorListSettingNode::onView(CCObject *) {
  auto setting = this->getSetting();
  ColorListPopup::create(setting->getValue().colors, [this](std::vector<std::string> colors) {
    this->setValue(ColorList{colors}, this);
  })->show();
}

ColorListSettingNode *ColorListSettingNode::create(std::shared_ptr<ColorListSettingV3> setting, float width) {
  auto *ret = new ColorListSettingNode();

  if (ret->init(setting, width)) {
    ret->autorelease();
    return ret;
  }

  CC_SAFE_DELETE(ret);
  return nullptr;
}

Result<std::shared_ptr<SettingV3>> ColorListSettingV3::parse(const std::string &key, const std::string &modID, const matjson::Value &json) {
  auto ret = std::make_shared<ColorListSettingV3>();
  auto root = checkJson(json, "ColorListSettingV3");
  (void) ret->parseBaseProperties(key, modID, json);

  root.checkUnknownKeys();

  return root.ok(ret).map([](std::shared_ptr<ColorListSettingV3> value) {
    return std::static_pointer_cast<SettingV3>(value);
  });
}
