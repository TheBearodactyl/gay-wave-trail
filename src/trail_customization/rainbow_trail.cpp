#include "rainbow_trail.hpp"
#include <cmath>

void RainbowTrail::hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
  float fC = fV * fS, fHPrime = fmodf(fH / 60.0f, 6), fX = fC * ( 1 - fabsf(fmodf(fHPrime, 2) - 1));
  int iHue = static_cast<int>(fHPrime);
  // ternary operators are fucking awesome.
  fR = ( iHue == 0 )
       ? fC
       : ( iHue == 1 )
         ? fX
         : ( iHue == 4 )
           ? fX
           : ( iHue == 5 )
             ? fC
             : 0;
  fG = ( iHue == 1 )
       ? fC
       : ( iHue == 2 )
         ? fC
         : ( iHue == 3 )
           ? fX
           : 0;
  fB = ( iHue == 3 )
       ? fC
       : ( iHue == 4 )
         ? fC
         : ( iHue == 5 )
           ? fX
           : 0;
  float fM = fV - fC;
  fR += fM;
  fG += fM;
  fB += fM;
}

float RainbowTrail::g = 0;

cocos2d::_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
  float R, G, B;

  float hue = static_cast<float>(fmod(g + offset, 360));
  float sat = static_cast<float>(saturation / 100.0);
  float vc = 1;
  hsv_to_rgb(R, G, B, hue, sat, vc);

  cocos2d::_ccColor3B out;
  out.r = static_cast<GLubyte>(R * 255);
  out.g = static_cast<GLubyte>(G * 255);
  out.b = static_cast<GLubyte>(B * 255);
  return out;
}
