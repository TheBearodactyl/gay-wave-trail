#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/loader/SettingV3.hpp"
#include <Geode/Result.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

struct ColorList {
  std::vector<std::string> colors;
  bool operator==(const ColorList &other) const {
    return this->colors == other.colors;
  }
};

class ColorListSettingV3 : public SettingBaseValueV3<ColorList> {
public:
  static Result<std::shared_ptr<SettingV3>> parse(const std::string &key, const std::string &modID, const matjson::Value &json);
  SettingNodeV3 *createNode(float width) override;
};

class ColorListSettingNode : public SettingValueNodeV3<ColorListSettingV3> {
protected:
  CCMenuItemSpriteExtra *m_resetBtn;

  bool init(std::shared_ptr<ColorListSettingV3> setting, float width);
  void onView(CCObject *sender);
  void onToggle(CCObject *sender);

public:
  static ColorListSettingNode *create(std::shared_ptr<ColorListSettingV3> setting, float width);
};

template <>
struct geode::SettingTypeForValueType<ColorList> {
  using SettingType = ColorListSettingV3;
};

template <>
struct matjson::Serialize<ColorList> {
  static matjson::Value toJson(const ColorList &value) {
    matjson::Value arr;
    for (const std::string &color: value.colors) {
      arr.push(color);
    }
    return arr;
  }

  static geode::Result<ColorList> fromJson(const matjson::Value &value) {
    ColorList ret;

    if (! value.isArray()) {
      return Err("json is not array");
    }

    matjson::Value array = value.asArray().unwrap();

    for (const matjson::Value &elem: array) {
      if (! elem.isString()) {
        return Err("color must be a string");
      }
      ret.colors.push_back(elem.asString().unwrap());
    }
    return Ok(ret);
  }
};
