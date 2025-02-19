#include "rainbow_trail.hpp"
#include "../constants.hpp"
#include "../settings/gay_settings.hpp"
#include "../utils/color_utils.hpp"
#include "../utils/hex_utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

using namespace gay::utils::color_conversion;
using namespace gay::utils::hex_code;

_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
  float hue = fmodf(ColorUtils::owo + offset, 360.0f);
  float sat = saturation / 100.0f;
  float val = 1.0f;

  float c = val * sat;
  float x = c * (1.0f - fabsf(fmodf(hue / 60.0f, 2.0f) - 1.0f));
  float m = val - c;

  float r, g, b;
  if (hue < 60.0f) {
    r = c; g = x; b = 0.0f;
  } else if (hue < 120.0f) {
    r = x; g = c; b = 0.0f;
  } else if (hue < 180.0f) {
    r = 0.0f; g = c; b = x;
  } else if (hue < 240.0f) {
    r = 0.0f; g = x; b = c;
  } else if (hue < 300.0f) {
    r = x; g = 0.0f; b = c;
  } else {
    r = c; g = 0.0f; b = x;
  }

  return {
      static_cast<GLubyte>((r + m) * 255.0f),
      static_cast<GLubyte>((g + m) * 255.0f),
      static_cast<GLubyte>((b + m) * 255.0f)
  };
}

_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const ColorList &colors) {
  const auto &clrs = colors.colors;
  if (clrs.empty())
    return {255, 255, 255};

  float t = fmodf(phase + offset, 360.0f);
  float segment_size = 360.0f / static_cast<float>(clrs.size());
  auto segment = static_cast<int>(t / segment_size);
  float y = (t - segment * segment_size) / segment_size;
  int next_segment = (segment + 1) % clrs.size();

  auto color = HexUtils::hex_to_rgb(clrs[segment].m_hex);
  auto next_color = HexUtils::hex_to_rgb(clrs[next_segment].m_hex);

  if (! smooth) {
    return color;
  }

  return {static_cast<GLubyte>(std::lerp(static_cast<float>(color.r), static_cast<float>(next_color.r), y)), static_cast<GLubyte>(std::lerp(static_cast<float>(color.g), static_cast<float>(next_color.g), y)), static_cast<GLubyte>(std::lerp(static_cast<float>(color.b), static_cast<float>(next_color.b), y))};
}

float phase;

class $modify(PlayLayer) {
  void postUpdate(float dt) override {
    PlayLayer::postUpdate(dt);

    auto gradient_colors = gay::settings::get<ColorList>("gaydient-colors");
    float speed = gay::settings::get_float("speed");

    ColorUtils::owo = fmodf(ColorUtils::owo + (speed * 60.0f * dt), 360.0f);

    phase = fmodf(phase + (speed * 60.0f * dt), 360.0f);

    auto rainbow_one = RainbowTrail::get_rainbow(0, gay::settings::get_float("saturation"));
    auto rainbow_two = RainbowTrail::get_rainbow(180, gay::settings::get_float("saturation"));
    auto gradient_one = RainbowTrail::get_gradient(phase, 0.0f, gay::settings::get<bool>("smooth-colors"), gradient_colors);
    auto gradient_two = RainbowTrail::get_gradient(phase + 180.0f, 0.0f, gay::settings::get<bool>("smooth-colors"), gradient_colors);

    if (this->m_player1 && gay::settings::get_mod_enabled() && this->m_player1->m_isDart) {
      this->m_player1->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient") ? gradient_one : rainbow_one);
    }

    if (this->m_player2 && gay::settings::get_mod_enabled() && this->m_player2->m_isDart) {
      this->m_player2->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient") ? gradient_two : rainbow_two);
    }
  }
};