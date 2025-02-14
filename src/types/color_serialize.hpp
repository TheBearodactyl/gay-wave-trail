#pragma once

#include "Geode/Result.hpp"
#include "color.hpp"
#include "matjson.hpp"
#include <optional>

template <>
struct matjson::Serialize<Colors> {
  static geode::Result<Colors> fromJson(matjson::Value const &value) {
    if (! value.isObject()) {
      return geode::Err("Expected object");
    }

    if (! value.contains("colors") || ! value["colors"].isArray()) {
      return geode::Err("Expected array of colors");
    }

    std::vector<std::string> colors;
    for (const auto &color: value["colors"].asArray().unwrap()) {
      if (! color.isString()) {
        return geode::Err("Expected color to be a string");
      }

      colors.push_back(color.asString().unwrap());
    }

    return geode::Ok(Colors{
        .m_colors = std::move(colors)});
  }

  static matjson::Value toJson(Colors const &value) {
    return matjson::makeObject({{"colors", value.m_colors}});
  }
};
