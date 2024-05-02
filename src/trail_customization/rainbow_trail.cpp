#include "rainbow_trail.hpp"
#include <cmath>
#include <vector>
#include <cstdint>

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

void RainbowTrail::hex_to_hsv(uint32_t hex, float &h, float &s, float &v) {
  float r = (( hex >> 16 ) & 0xFF ) / 255.0f;
  float g = (( hex >> 8 ) & 0xFF ) / 255.0f;
  float b = ( hex & 0xFF ) / 255.0f;

  float max = std::max(std::max(r, g), b);
  float min = std::min(std::min(r, g), b);
  v = max;

  float delta = max - min;
  if ( max != 0.0f ) {
    s = delta / max;
  } else {
    s = 0.0f;
    h = 0.0f;
    return;
  }

  if ( r == max ) {
    h = ( g - b ) / delta;
  } else if ( g == max ) {
    h = 2.0f + ( b - r ) / delta;
  } else {
    h = 4.0f + ( r - g ) / delta;
  }

  h *= 60.0f;
  if ( h < 0.0f ) {
    h += 360.0f;
  }
}

cocos2d::_ccColor3B RainbowTrail::get_custom_rainbow(const std::vector<uint32_t> &hex_colors, float offset, float saturation) {
  float R, G, B;

  int num_colors = static_cast<int>(hex_colors.size());
  if ( num_colors == 0 ) {
    return cocos2d::_ccColor3B{0, 0, 0};
  }

  float hue, sat, vc = 1.0f;
  hex_to_hsv(hex_colors[static_cast<unsigned int>(static_cast<int>(fmod(g + offset, num_colors)))], hue, sat, vc);
  sat = static_cast<float>(static_cast<double>(saturation) / 100.0);

  hsv_to_rgb(R, G, B, hue, sat, vc);

  cocos2d::_ccColor3B out{};
  out.r = static_cast<GLubyte>(R * 255);
  out.g = static_cast<GLubyte>(G * 255);
  out.b = static_cast<GLubyte>(B * 255);
  return out;
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
