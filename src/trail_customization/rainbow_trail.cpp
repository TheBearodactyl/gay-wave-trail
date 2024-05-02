#include "rainbow_trail.hpp"
#include <cmath>

void RainbowTrail::hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
  float c = fV * fS;
  float x = static_cast<float>(static_cast<double>(c) * ( 1 - std::abs(std::fmod(fH / 60.0f, 2) - 1)));
  float m = fV - c;

  fR = ( fH < 60.0f )
       ? c
       : ( fH < 120.0f )
         ? x
         : ( fH < 180.0f )
           ? 0
           : ( fH < 240.0f )
             ? 0
             : ( fH < 300.0f )
               ? x
               : c;
  fG = ( fH < 60.0f )
       ? x
       : ( fH < 120.0f )
         ? c
         : ( fH < 180.0f )
           ? c
           : ( fH < 240.0f )
             ? x
             : ( fH < 300.0f )
               ? 0
               : 0;
  fB = ( fH < 60.0f )
       ? 0
       : ( fH < 120.0f )
         ? 0
         : ( fH < 180.0f )
           ? x
           : ( fH < 240.0f )
             ? c
             : ( fH < 300.0f )
               ? c
               : x;

  fR += m;
  fG += m;
  fB += m;
}

float RainbowTrail::g = 0;

cocos2d::_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
  float R, G, B;

  float hue = static_cast<float>(fmod(g + offset, 360));
  float sat = static_cast<float>(static_cast<double>(saturation) / 100.0);
  float vc = 1;
  hsv_to_rgb(R, G, B, hue, sat, vc);

  cocos2d::_ccColor3B out{};
  out.r = static_cast<GLubyte>(R * 255);
  out.g = static_cast<GLubyte>(G * 255);
  out.b = static_cast<GLubyte>(B * 255);
  return out;
}
