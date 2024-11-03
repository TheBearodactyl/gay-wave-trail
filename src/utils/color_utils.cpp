#include "color_utils.hpp"
#include <cmath>

void ColorUtils::hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
  float c = fV * fS;
  float x = static_cast<float>(static_cast<double>(c) * (1 - std::abs(std::fmod(fH / 60.0f, 2) - 1)));
  float m = fV - c;

  fR = (fH < 60.0f)
           ? c
       : (fH < 120.0f)
           ? x
       : (fH < 180.0f)
           ? 0
       : (fH < 240.0f)
           ? 0
       : (fH < 300.0f)
           ? x
           : c;
  fG = (fH < 60.0f)
           ? x
       : (fH < 120.0f)
           ? c
       : (fH < 180.0f)
           ? c
       : (fH < 240.0f)
           ? x
       : (fH < 300.0f)
           ? 0
           : 0;
  fB = (fH < 60.0f)
           ? 0
       : (fH < 120.0f)
           ? 0
       : (fH < 180.0f)
           ? x
       : (fH < 240.0f)
           ? c
       : (fH < 300.0f)
           ? c
           : x;

  fR += m;
  fG += m;
  fB += m;
}

float ColorUtils::owo = 0;

void ColorUtils::hex_to_hsv(uint32_t hex, float &h, float &s, float &v) {
  float r = ((hex >> 16) & 0xFF) / 255.0f;
  float g = ((hex >> 8) & 0xFF) / 255.0f;
  float b = (hex & 0xFF) / 255.0f;

  float max = std::max(std::max(r, g), b);
  float min = std::min(std::min(r, g), b);
  v = max;

  float delta = max - min;
  if (max != 0.0f) {
    s = delta / max;
  } else {
    s = 0.0f;
    h = 0.0f;
    return;
  }

  if (r == max) {
    h = (g - b) / delta;
  } else if (g == max) {
    h = 2.0f + (b - r) / delta;
  } else {
    h = 4.0f + (r - g) / delta;
  }

  h *= 60.0f;
  if (h < 0.0f) {
    h += 360.0f;
  }
}

cocos2d::ccColor3B ColorUtils::hex_to_rgb(const std::string &hex) {
  if (! is_valid_hex_code(hex)) {
    return cocos2d::ccColor3B{0, 0, 0};
  }

  std::string clean_hex = hex;
  if (clean_hex[0] == '#') {
    clean_hex = clean_hex.substr(1);
  }

  GLubyte r = hex_pair_to_dec(clean_hex.substr(0, 2));
  GLubyte g = hex_pair_to_dec(clean_hex.substr(2, 2));
  GLubyte b = hex_pair_to_dec(clean_hex.substr(4, 2));

  return cocos2d::ccColor3B{r, g, b};
}

GLubyte ColorUtils::hex_pair_to_dec(const std::string &hex_pair) {
  int val = 0;

  for (char c: hex_pair) {
    val = val * 16;

    if (c >= '0' && c <= '9') {
      val += c - '0';
    } else {
      val += std::tolower(c) - 'a' + 10;
    }
  }

  return static_cast<GLubyte>(val);
}

bool ColorUtils::is_valid_hex_code(const std::string &hex_code) {
  if (hex_code.empty())
    return false;

  size_t start_pos = (hex_code[0] == '#') ? 1 : 0;

  if (hex_code.length() != start_pos + 6)
    return false;

  return std::all_of(hex_code.begin() + start_pos, hex_code.end(),
                     [](char c) {
                       c = std::tolower(c);
                       return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
                     });
}