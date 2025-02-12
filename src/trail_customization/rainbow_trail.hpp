#pragma once

#include <Geode/cocos/include/ccTypes.h>
#include <colortools.hpp>
#include <vector>

class RainbowTrail {
public:
  static cocos2d::_ccColor3B get_rainbow(float offset, float saturation);
  cocos2d::_ccColor3B get_custom_rainbow(const std::vector<uint32_t> &hex_colors, float offset, float saturation);
  cocos2d::_ccColor3B get_gradient(float phase, float offset, bool smooth, const std::vector<cocos2d::ccColor3B>& colors);
};