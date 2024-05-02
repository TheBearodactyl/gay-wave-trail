#pragma once

#include <Geode/cocos/include/ccTypes.h>
#include <vector>

class RainbowTrail {
public:
  static void hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV);
  static void hex_to_hsv(uint32_t hex, float &h, float &s, float &v);
  static cocos2d::_ccColor3B get_rainbow(float offset, float saturation);
  cocos2d::_ccColor3B get_custom_rainbow(const std::vector<uint32_t> &hex_colors, float offset, float saturation);
  static float g;
};