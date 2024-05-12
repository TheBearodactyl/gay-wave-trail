#include "rainbow_trail.hpp"
#include <cmath>
#include <vector>
#include <cstdint>

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

cocos2d::_ccColor3B RainbowTrail::get_gradient(float &phase, float offset, bool smooth, ccColor3B c1, ccColor3B c2, ccColor3B c3, ccColor3B c4) {
    float t = fmodf(phase + offset, 360.0f);
    float y = t / 90.0f;
    int quadrant = static_cast<int>(t / 90.0f);

    cocos2d::_ccColor3B out;

    out.r = static_cast<GLubyte>(
        quadrant == 0 ? c1.r + (c2.r - c1.r) * y
        : quadrant == 1 ? c2.r + (c3.r - c2.r) * (y - 1.0f)
        : quadrant == 2 ? c3.r + (c4.r - c3.r) * (y - 2.0f)
        : c4.r + (c1.r - c4.r) * (y - 3.0f));

    out.g = static_cast<GLubyte>(
        quadrant == 0 ? c1.g + (c2.g - c1.g) * y
        : quadrant == 1 ? c2.g + (c3.g - c2.g) * (y - 1.0f)
        : quadrant == 2 ? c3.g + (c4.g - c3.g) * (y - 2.0f)
        : c4.g + (c1.g - c4.g) * (y - 3.0f));

    out.b = static_cast<GLubyte>(
        quadrant == 0 ? c1.b + (c2.b - c1.b) * y
        : quadrant == 1 ? c2.b + (c3.b - c2.b) * (y - 1.0f)
        : quadrant == 2 ? c3.b + (c4.b - c3.b) * (y - 2.0f)
        : c4.b + (c1.b - c4.b) * (y - 3.0f));

    return out;
}
