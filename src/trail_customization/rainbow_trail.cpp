#include "rainbow_trail.hpp"
#include <cmath>
#include <cstdint>
#include <vector>

cocos2d::_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
  float R, G, B;

  float hue = static_cast<float>(fmod(ColorUtils::owo + offset, 360));
  float sat = static_cast<float>(static_cast<double>(saturation) / 100.0);
  float vc = 1;
  ColorUtils::hsv_to_rgb(R, G, B, hue, sat, vc);

  cocos2d::_ccColor3B out{};
  out.r = static_cast<GLubyte>(R * 255);
  out.g = static_cast<GLubyte>(G * 255);
  out.b = static_cast<GLubyte>(B * 255);
  return out;
}

cocos2d::_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const std::vector<cocos2d::ccColor3B> &colors) {
  float t = fmodf(phase + offset, 360.0f);
  float y = t / 90.0f;
  int quadrant = static_cast<int>(t / 90.0f);

  cocos2d::_ccColor3B out;

  out.r = static_cast<GLubyte>(
      quadrant == 0   ? colors[0].r + (colors[1].r - colors[0].r) * y
      : quadrant == 1 ? colors[1].r + (colors[2].r - colors[1].r) * (y - 1.0f)
      : quadrant == 2 ? colors[2].r + (colors[3].r - colors[2].r) * (y - 2.0f)
                      : colors[3].r + (colors[0].r - colors[3].r) * (y - 3.0f));

  out.g = static_cast<GLubyte>(
      quadrant == 0   ? colors[0].g + (colors[1].g - colors[0].g) * y
      : quadrant == 1 ? colors[1].g + (colors[2].g - colors[1].g) * (y - 1.0f)
      : quadrant == 2 ? colors[2].g + (colors[3].g - colors[2].g) * (y - 2.0f)
                      : colors[3].g + (colors[0].g - colors[3].g) * (y - 3.0f));

  out.b = static_cast<GLubyte>(
      quadrant == 0   ? colors[0].b + (colors[1].b - colors[0].b) * y
      : quadrant == 1 ? colors[1].b + (colors[2].b - colors[1].b) * (y - 1.0f)
      : quadrant == 2 ? colors[2].b + (colors[3].b - colors[2].b) * (y - 2.0f)
                      : colors[3].b + (colors[0].b - colors[3].b) * (y - 3.0f));

  return out;
}