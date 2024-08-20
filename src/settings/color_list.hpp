#pragma once

#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/loader/Setting.hpp"
#include "Geode/loader/SettingNode.hpp"
#include "ccTypes.h"
#include <matjson.hpp>
#include <vector>

using namespace geode::prelude;

struct ColorGridSettingStruct {
public:
  std::vector<ccColor3B> m_colors;
};

class ColorGridSettingValue;
class ColorGridSettingValue : public SettingValue {
protected:
  std::vector<ccColor3B> m_colors;

public:
  ColorGridSettingValue(std::string const &key, std::string const &mod_id, std::vector<ccColor3B> colors) : SettingValue(key, mod_id), m_colors(colors) {}

  bool load(matjson::Value const &json) override {
    m_colors.clear();
    auto array = json.as_array();

    for (auto const &elem : array) {
      m_colors.push_back(elem.as<ccColor3B>());
    }

    return true;
  }

  bool save(matjson::Value &json) const override {
    auto array = matjson::Array();

    for (auto const &color : m_colors) {
      array.push_back(color);
    }

    json = array;
    return true;
  }

  SettingNode *createNode(float width) override;

  void setColors(std::vector<ccColor3B> colors) {
    this->m_colors = colors;
    this->valueChanged();
  }

  std::vector<ccColor3B> getColors() const {
    return this->m_colors;
  }
};

template <>
struct SettingValueSetter<ColorGridSettingStruct> {
  static ColorGridSettingStruct get(SettingValue *setting) {
    return ColorGridSettingStruct{static_cast<ColorGridSettingValue *>(setting)->getColors()};
  };

  static void set(ColorGridSettingValue *setting, ColorGridSettingStruct const &value) {
    setting->setColors(value.m_colors);
  };
};

class ColorGridSettingNode : public SettingNode {
protected:
  ColorGridSettingValue *m_value;
  CCMenuItemSpriteExtra *m_resetBtn;
  CCLabelBMFont *m_label;
  std::vector<ccColor3B> m_localValue;
  std::string m_name;
  std::string m_description;
  std::vector<ccColor3B> m_defaultValue;

  bool init(ColorGridSettingValue *value, float width);

public:
  void updateVisuals();
  void onView(CCObject *);
  void onReset(CCObject *);
  void onDesc(CCObject *);
  void commit() override;
  bool hasUncommittedChanges() override;
  bool hasNonDefaultValue() override;
  void resetToDefault() override;
  static ColorGridSettingNode *create(ColorGridSettingValue *value, float width);
};

