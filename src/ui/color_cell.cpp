#include "color_cell.hpp"

#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/ui/Layout.hpp"
#include "ui/colors_popup.hpp"
#include <fmt/format.h>

bool ColorCell::init(ColorListPopup *parentPopup, std::string *color,
                     std::function<void()> onDelete, CCSize const &size) {
  if (! CCNode::init()) {
    return false;
  }

  static const float HORIZONTAL_PADDING = 2.5f;

  m_parentPopup = parentPopup;
  m_color = color;
  m_onDelete = onDelete;

  this->setContentSize(size);
  this->setAnchorPoint(CCPoint{0.5f, 0.5f});

  auto bg = CCScale9Sprite::create("square02b_001.png");
  bg->setColor({0, 0, 0});
  bg->setOpacity(75);
  bg->setScale(0.3f);
  bg->setContentSize(size / bg->getScale());
  this->addChildAtPosition(bg, Anchor::Center);

  float m_buttonsSize = 0.f;

  auto sprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
  sprite->setScale(0.7f);
  auto deleteButton = CCMenuItemSpriteExtra::create(
      sprite, this, menu_selector(ColorCell::onDelete));
  deleteButton->setID("delete-button");
  m_buttonsSize += deleteButton->getContentSize().width;

  auto buttonsMenu = CCMenu::create();
  buttonsMenu->addChild(deleteButton);
  buttonsMenu->setAnchorPoint(CCPoint{1.0f, 0.5f});
  buttonsMenu->setContentSize(CCSize{50.f, 30.f});

  buttonsMenu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(
      AxisAlignment::Even));
  buttonsMenu->updateLayout();
  buttonsMenu->setID("button-menu");

  auto inputNode =
      TextInput::create(size.width - HORIZONTAL_PADDING * 2 - m_buttonsSize,
                        "Color name", "chatFont.fnt");
  inputNode->setScale(1.f);
  inputNode->setCommonFilter(CommonFilter::Any);
  inputNode->setMaxCharCount(50);
  inputNode->setString(*m_color, false);
  inputNode->setTextAlign(TextInputAlign::Left);
  inputNode->setCallback([this](std::string const &str) {
    *m_color = str;
  });

  auto menu = CCMenu::create();
  menu->addChild(inputNode);
  menu->addChild(buttonsMenu);

  menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(
      AxisAlignment::Between));
  menu->setID("menu");
  menu->setAnchorPoint(CCPoint{0.5f, 0.5f});
  menu->setContentSize(
      CCSize{size.width - HORIZONTAL_PADDING * 2, size.height});
  menu->updateLayout();

  this->addChildAtPosition(menu, Anchor::Center, CCPoint{0.0f, 0.0f});
  return true;
}

void ColorCell::onDelete(CCObject *) {
  m_onDelete();
}

ColorCell *ColorCell::create(ColorListPopup *parentPopup, std::string *color,
                             std::function<void()> onDelete,
                             CCSize const &size) {
  auto ret = new ColorCell();
  if (ret && ret->init(parentPopup, color, onDelete, size)) {
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}
