#ifndef COLOR_UTILS_HPP
#define COLOR_UTILS_HPP

#include <cocos2d.h>

using namespace cocos2d;

namespace gay::utils::color_conversion {
  class ColorUtils {
  public:
    static float owo;

    static ccColor3B hsv_to_rgb(float h, float s, float v);
  };
}// namespace gay::utils::color_conversion

#endif