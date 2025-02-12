#include "rainbow_trail.hpp"
#include <cmath>
#include <vector>

cocos2d::_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
  float R, G, B;

  auto hue = static_cast<float>(fmod(OWO + offset, 360));
  auto sat = static_cast<float>(static_cast<double>(saturation) / 100.0);
  float vc = 1;
  hsv_to_rgb(&R, &G, &B, &hue, &sat, &vc);

  cocos2d::_ccColor3B out{};
  out.r = static_cast<GLubyte>(R * 255);
  out.g = static_cast<GLubyte>(G * 255);
  out.b = static_cast<GLubyte>(B * 255);
  return out;
}

cocos2d::_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const std::vector<cocos2d::ccColor3B> &colors) {
  float t = fmodf(phase + offset, 360.0f);
  float segment_size = 360.0f / static_cast<float>(colors.size());
  int segment = static_cast<int>(t / segment_size);
  float y = (t - static_cast<float>(segment) * segment_size) / segment_size;

  int next_segment = (segment + 1) % colors.size();

  cocos2d::_ccColor3B out{};
  out.r = static_cast<GLubyte>(colors[segment].r + (colors[next_segment].r - colors[segment].r) * y);
  out.g = static_cast<GLubyte>(colors[segment].g + (colors[next_segment].g - colors[segment].g) * y);
  out.b = static_cast<GLubyte>(colors[segment].b + (colors[next_segment].b - colors[segment].b) * y);

  return out;
}