#pragma once

#include <functional>

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

using ColorsCallback = std::function<void(std::vector<std::string>)>;

class ColorListPopup : public Popup<std::vector<std::string>, ColorsCallback> {
protected:
  std::vector<std::string> m_colors;
  ColorsCallback m_setColorsCallback;
  geode::ScrollLayer *m_list;

  bool setup(std::vector<std::string>, ColorsCallback setColorsCallback) override;
  void createList();
  CCSize getPopupSize();
  void onClose(CCObject *) override;
  void onAdd(CCObject *);

public:
  static ColorListPopup *create(std::vector<std::string>, ColorsCallback setColorsCallback);
};
