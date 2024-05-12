#pragma once

#include <Geode/cocos/include/ccTypes.h>
#include "../utils/color_utils.hpp"
#include <vector>

class RainbowTrail {
public:
  static cocos2d::_ccColor3B get_rainbow(float offset, float saturation);
  cocos2d::_ccColor3B get_custom_rainbow(const std::vector<uint32_t> &hex_colors, float offset, float saturation);
  static cocos2d::_ccColor3B get_gradient(float &phase, float offset, bool smooth, ccColor3B c1, ccColor3B c2, ccColor3B c3, ccColor3B c4);
};