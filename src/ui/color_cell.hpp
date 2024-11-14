#pragma once

#include <functional>

#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "ui/colors_popup.hpp"

using namespace geode::prelude;

class ColorsPopup;

class ColorCell : public CCNode {
protected:
  ColorListPopup *m_parentPopup;
  std::string *m_color;
  std::function<void()> m_onDelete;

  CCMenuItemToggler *m_toggleButton;

  bool init(ColorListPopup *parentPopup, std::string *color, std::function<void()> onDelete, CCSize const &size);

public:
  static ColorCell *create(ColorListPopup *parentPopup, std::string *color, std::function<void()> onDelete, CCSize const &size);
  void updateUI();
  void onToggle(CCObject *);
  void onDelete(CCObject *);
};
