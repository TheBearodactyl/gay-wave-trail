#pragma once

#include <cocos2d.h>

using namespace cocos2d;

class ColorUtils {
public:
  static cocos2d::ccColor3B hex_to_rgb(const std::string& hex);
  static void hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV);
  static void hex_to_hsv(uint32_t hex, float &h, float &s, float &v);
  static float owo;

private:
  static GLubyte hex_pair_to_dec(const std::string& hex_pair);
  static bool is_valid_hex_code(const std::string& hex_code);
};