#pragma once

#include <Geode/cocos/include/ccTypes.h>

class RainbowTrail {
public:
  static void hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV);
  static cocos2d::_ccColor3B get_rainbow(float offset, float saturation);
  static float g;
};