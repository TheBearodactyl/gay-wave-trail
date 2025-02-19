#ifndef COLOR_LIST_SETTING_HPP
#define COLOR_LIST_SETTING_HPP

#include "../../types/color.hpp"
#include <cocos2d.h>

using namespace cocos2d;
using namespace geode::prelude;

class ColorListSetting : public SettingBaseValueV3<ColorList> {
public:
  static Result<std::shared_ptr<SettingV3>> parse(std::string const &key, std::string const &modID, matjson::Value const &json);

  SettingNodeV3 *createNode(float width) override;
};

class ColorListSettingNode : public SettingValueNodeV3<ColorListSetting> {
protected:
  CCMenuItemSpriteExtra *m_resetBtn;

  bool init(std::shared_ptr<ColorListSetting> setting, float width);
  void onToggle(CCObject *sender);
  void onView(CCObject *);

public:
  static ColorListSettingNode *create(std::shared_ptr<ColorListSetting>, float width);
};

template <>
struct SettingTypeForValueType<ColorList> {
  using SettingType = ColorListSetting;
};

template <>
struct matjson::Serialize<ColorEntry> {
  static Result<ColorEntry> fromJson(matjson::Value const &value) {
    if (! value["hex"].isString() || ! value["userAdded"].isBool() || ! value["enabled"].isBool()) {
      return Err("Invalid JSON");
    }

    return Ok(ColorEntry(value["hex"].asString().unwrap(), value["userAdded"].asBool().unwrap(), value["enabled"].asBool().unwrap()));
  }

  static matjson::Value toJson(ColorEntry const &value) {
    return matjson::makeObject({{"hex", value.m_hex}, {"userAdded", value.m_userAdded}, {"enabled", value.m_enabled}});
  }
};

template <>
struct matjson::Serialize<ColorList> {
  static Result<ColorList> fromJson(matjson::Value const &value) {
    ColorList ret;

    for (const matjson::Value &elem: value) {
      GEODE_UNWRAP_INTO(ColorEntry color, matjson::Serialize<ColorEntry>::fromJson(elem));

      ret.colors.push_back(color);
    }

    return Ok(ret);
  }

  static matjson::Value toJson(const ColorList &value) {
    matjson::Value arr = matjson::Value::array();

    for (const ColorEntry &elem: value.colors) {
      arr.push(matjson::Serialize<ColorEntry>::toJson(elem));
    }

    return arr;
  }
};

#endif