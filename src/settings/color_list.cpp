#include "color_list.hpp"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include <Geode/loader/Setting.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/ListView.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>
#include <ccTypes.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

using namespace geode::prelude;

SettingNode *ColorGridSettingValue::createNode(float width) {
  return ColorGridSettingNode::create(this, width);
}

class ColorGridSettingPopup : public Popup<std::vector<ccColor3B>, std::function<void(std::vector<ccColor3B>)>> {
protected:
  std::vector<ccColor3B> m_localValue;
  std::function<void(std::vector<ccColor3B>)> m_newColorsCallback;
  CCMenu *m_gridMenu;
  ListView *m_list;

  bool setup(std::vector<ccColor3B> localValue, std::function<void(std::vector<ccColor3B>)> newColorsCallback) override {
    auto win_size = CCDirector::sharedDirector()->getWinSize();

    m_localValue = localValue;
    m_newColorsCallback = newColorsCallback;

    m_gridMenu = CCMenu::create();
    m_gridMenu->setPosition(0, 0);

    this->m_mainLayer->addChild(m_gridMenu);

    createGrid();
    handleTouchPriority(this);

    return true;
  }

  void onActionBtn(CCObject *sender) {
    auto id = static_cast<CCNode *>(sender)->getID();
    auto index = std::stoi(id.substr(4));

    if (index == m_localValue.size() - 1) {
      m_localValue.push_back(ccColor3B{0xff, 0xff, 0xff});
    } else {
      m_localValue.erase(m_localValue.begin() + index);
    }

    m_newColorsCallback(m_localValue);
    createGrid();
  }

  void createGrid() {
    std::optional<float> previous_list_position = {};

    if (m_list != nullptr) {
      auto table_view = m_list->m_tableView;
      float view_height = table_view->getContentHeight();
      float entire_list_height = table_view->m_contentLayer->getContentHeight();

      if (entire_list_height > view_height) {
        float most_possible_moved_height_bottom = entire_list_height - view_height;
        float entire_list_y_pos = table_view->m_contentLayer->getPositionY();
        float most_possible_moved_height_top = -most_possible_moved_height_bottom - entire_list_y_pos;

        previous_list_position = most_possible_moved_height_top;
      }
    }

    m_gridMenu->removeAllChildren();
    m_list = nullptr;

    auto cells = CCArray::create();
    auto size = this->m_mainLayer->getContentSize();

    for (int i = 0; i < m_localValue.size(); i++) {
      auto last = i == m_localValue.size() - 1;
      auto size2 = CCSize{size.width - 10.f, 40.f};
      auto menu = CCMenu::create();
      menu->setPosition(0, 0);

      ColorPickPopup *input_node = ColorPickPopup::create(false);
      input_node->setScale(1.f);
      input_node->setPosition(size2.width / 2 - 15.f, size2.height / 2);
      input_node->setContentWidth(size2.width - 60.f);
      input_node->setColor(m_localValue[i]);
      menu->addChild(input_node);

      auto spr = last ? CCSprite::create("addIcon.png"_spr) : CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
      spr->setScale(0.75f);

      auto btn = CCMenuItemSpriteExtra::create(
          spr,
          this,
          menu_selector(ColorGridSettingPopup::onActionBtn));
      btn->setID(fmt::format("btn-{}", i));
      btn->setPosition(size2.width - 15.f, size2.height / 2);

      menu->addChild(btn);
      cells->addObject(menu);
    }

    auto list = ListView::create(cells, 40.f, size.width - 8.f, size.height - 16.f);
    list->setPosition(4.f, 10.f);

    std::optional<float> new_list_position = {};

    if (previous_list_position.has_value()) {
      auto table_view = list->m_tableView;
      float view_height = table_view->getContentHeight();
      float entire_list_height = table_view->m_contentLayer->getContentHeight();
      float most_possible_moved_height_bottom = entire_list_height - view_height;
      float most_possible_moved_height_top = entire_list_height - view_height;

      new_list_position = -previous_list_position.value() - most_possible_moved_height_bottom;
      new_list_position = std::clamp(new_list_position.value(), -most_possible_moved_height_bottom, 0.f);

      if (entire_list_height < view_height) {
        new_list_position = view_height - entire_list_height;
      }
    }

    if (new_list_position.has_value()) {
      list->m_tableView->m_contentLayer->setPosition(CCPoint{0, new_list_position.value()});
    }

    m_gridMenu->addChild(list);
    m_list = list;
  }

public:
  static ColorGridSettingPopup *create(std::vector<ccColor3B> localValue, std::function<void(std::vector<ccColor3B>)> newColorsCallback) {
    auto ret = new ColorGridSettingPopup();

    if (ret && ret->initAnchored(420.f, 160.f, localValue, newColorsCallback)) {
      ret->autorelease();
      return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
  }
};

bool ColorGridSettingNode::init(ColorGridSettingValue *value, float width) {
  if (!SettingNode::init(value)) {
    return false;
  }

  this->m_value = value;

  for (ccColor3B color : value->getColors()) {
    m_localValue.push_back(color);
  }

  float height = 40.f;
  this->setContentSize({width, height});

  auto menu = CCMenu::create();
  menu->setPosition(0, 0);
  menu->setID("inputs-menu");

  auto settings_json = Mod::get()->getSettingDefinition("gaydient-colors")->get<CustomSetting>()->json;
  m_defaultValue = settings_json->get<std::vector<ccColor3B>>("default");
  m_name = settings_json->get<std::string>("name");
  m_description = settings_json->get<std::string>("description");
  m_label = CCLabelBMFont::create(m_name.c_str(), "bigFont.fnt");
  m_label->setAnchorPoint({0.f, 0.5f});
  m_label->setPosition(20.f, height / 2);
  m_label->setScale(0.5f);

  menu->addChild(m_label);

  auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
  infoSpr->setScale(.6f);
  auto infoBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(ColorGridSettingNode::onDesc));
  infoBtn->setPosition(m_label->getScaledContentSize().width + 40.f, height / 2);
  menu->addChild(infoBtn);

  auto resetSpr = CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png");
  resetSpr->setScale(.5f);

  m_resetBtn = CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(ColorGridSettingNode::onReset));
  m_resetBtn->setPosition(m_label->getScaledContentSize().width + 40.f + 20.f, height / 2);

  menu->addChild(m_resetBtn);

  auto viewSpr = ButtonSprite::create("View");
  viewSpr->setScale(0.72f);

  auto viewBtn = CCMenuItemSpriteExtra::create(viewSpr, this, menu_selector(ColorGridSettingNode::onView));
  viewBtn->setPosition(width - 40.f, height - 20.f);

  menu->addChild(viewBtn);

  this->addChild(menu);
  handleTouchPriority(this);

  updateVisuals();

  return true;
}

void ColorGridSettingNode::onView(CCObject *) {
  auto popup = ColorGridSettingPopup::create(m_localValue, [this](std::vector<ccColor3B> newColors) {
    m_localValue = newColors;

    updateVisuals();
  });

  popup->show();
}

void ColorGridSettingNode::onReset(CCObject *) { resetToDefault(); }
void ColorGridSettingNode::onDesc(CCObject *) {
  FLAlertLayer::create(m_name.c_str(), m_description.c_str(), "OK")->show();
}

void ColorGridSettingNode::updateVisuals() {
  m_resetBtn->setVisible(hasNonDefaultValue());
  m_label->setColor(
      hasUncommittedChanges()
          ? ccColor3B{0x11, 0xdd, 0x00}
          : ccColor3B{0xff, 0xff, 0xff});

  this->dispatchChanged();
}

void ColorGridSettingNode::commit() {
  this->m_value->setColors(m_localValue);
  updateVisuals();
  this->dispatchCommitted();
}

bool ColorGridSettingNode::hasUncommittedChanges() {
  if (m_localValue.size() != m_value->getColors().size()) {
    return true;
  }

  for (int i = 0; i < m_localValue.size(); i++) {
    if (m_localValue[i] != m_value->getColors()[i]) {
      return true;
    }
  }

  return false;
}

bool ColorGridSettingNode::hasNonDefaultValue() {
  if (m_localValue.size() != m_defaultValue.size()) {
    return true;
  }

  for (int i = 0; i < m_localValue.size(); i++) {
    if (m_localValue[i] != m_defaultValue[i]) {
      return true;
    }
  }

  return false;
}

void ColorGridSettingNode::resetToDefault() {
  m_localValue.clear();
  for (ccColor3B color : m_defaultValue) {
    m_localValue.push_back(color);
  }

  updateVisuals();
}

ColorGridSettingNode* ColorGridSettingNode::create(ColorGridSettingValue* value, float width) {
  auto ret = new ColorGridSettingNode();
  if (ret && ret->init(value, width)) {
    ret->autorelease();
    return ret;
  }

  CC_SAFE_DELETE(ret);
  return nullptr;
}
