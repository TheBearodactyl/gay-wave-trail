#include <cocos2d.h>

using namespace cocos2d;

class ColorUtils {
public:
  static void hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV);
  static void hex_to_hsv(uint32_t hex, float &h, float &s, float &v);
  static float owo;
};