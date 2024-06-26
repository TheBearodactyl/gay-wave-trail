#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <cmath>
#include <cocos2d.h>
#include <random>

#include "Geode/loader/ModEvent.hpp"
#include "Geode/modify/Modify.hpp"
#include "settings/section.hpp"
#include "trail_customization/rainbow_trail.hpp"

using namespace geode::prelude;
using namespace cocos2d;

float phase;

struct MyPlayerObject : Modify<MyPlayerObject, PlayerObject> {
  void flashPlayer(float p0, float p1, cocos2d::ccColor3B mainColor, cocos2d::ccColor3B secondColor) {}
};

struct MyPlayLayer : Modify<MyPlayLayer, PlayLayer> {
  void postUpdate(float p0) {
    PlayLayer::postUpdate(p0);

    float speed = Mod::get()->getSettingValue<double>("speed");
    float saturation = Mod::get()->getSettingValue<double>("saturation");

    bool mirror_players = Mod::get()->getSettingValue<bool>("mirror-players");
    bool use_gradient = Mod::get()->getSettingValue<bool>("use-gradient");
    bool enable = Mod::get()->getSettingValue<bool>("enable");
    bool noRegularTrail = Mod::get()->getSettingValue<bool>("no-reg-trail");
    bool rainbow_icon = Mod::get()->getSettingValue<bool>("rainbow-icon");
    bool disable_trail = Mod::get()->getSettingValue<bool>("disable-wave-trail");
    bool infinite_trail = Mod::get()->getSettingValue<bool>("infinite-trail");

    auto color1 = Mod::get()->getSettingValue<ccColor3B>("color-one");
    auto color2 = Mod::get()->getSettingValue<ccColor3B>("color-two");
    auto color3 = Mod::get()->getSettingValue<ccColor3B>("color-three");
    auto color4 = Mod::get()->getSettingValue<ccColor3B>("color-four");

    if (ColorUtils::owo >= 360) {
      ColorUtils::owo = 0;
    } else {
      ColorUtils::owo += speed / 10;
    }

    phase = fmod(phase + speed, 360.f);
    bool p2 = true;

    _ccColor3B rainbowColor = RainbowTrail::get_rainbow(0, saturation);
    _ccColor3B rainbowColor2 = RainbowTrail::get_rainbow(180, saturation);

    _ccColor3B gradientColor = RainbowTrail::get_gradient(phase, 0.0f, false, color1, color2, color3, color4);
    _ccColor3B gradientColor2 = RainbowTrail::get_gradient(phase, 0.0f, false, color4, color3, color2, color1);

    if (m_player1->m_isDart && noRegularTrail) {
      m_player1->m_regularTrail
          ->setVisible(false);
    }

    if (m_player2->m_isDart && noRegularTrail) {
      m_player2->m_regularTrail
          ->setVisible(false);
    }

    if (enable) {
      if (! use_gradient) {
        if (m_player1->m_waveTrail) {
          m_player1->m_waveTrail
              ->setColor(rainbowColor);

          if (rainbow_icon && m_player1->m_isDart) {
            m_player1->setColor(rainbowColor);
          }
        }

        if (m_player2->m_waveTrail) {
          m_player2->m_waveTrail
              ->setColor(! mirror_players
                             ? rainbowColor2
                             : rainbowColor);

          if (rainbow_icon && m_player1->m_isDart) {
            m_player2->setColor(rainbowColor2);
          }
        }
      } else {
        if (m_player1->m_waveTrail) {
          m_player1->m_waveTrail
              ->setColor(gradientColor);

          if (rainbow_icon && m_player1->m_isDart) {
            m_player1->setColor(gradientColor);
          } else if (! rainbow_icon) {
            reset_colors();
          }
        }

        if (m_player2->m_waveTrail) {
          m_player2->m_waveTrail
              ->setColor(! mirror_players
                             ? gradientColor
                             : gradientColor2);

          if (rainbow_icon && m_player2->m_isDart) {
            m_player2->setColor(! mirror_players ? gradientColor : gradientColor2);
          } else if (! rainbow_icon) {
            reset_colors();
          }
        }
      }

      if (disable_trail) {
        this->m_player1->m_waveTrail->setVisible(false);
        this->m_player2->m_waveTrail->setVisible(false);
      } else {
        this->m_player1->m_waveTrail->setVisible(true);
        this->m_player2->m_waveTrail->setVisible(true);
      }
    }
  }

  int get_player_set_color(bool player2) {
    if (player2) {
      return GameManager::sharedState()->m_playerColor2;
    } else {
      return GameManager::sharedState()->m_playerColor;
    }
  }

  ccColor3B int_to_rgb(int color) {
    GLubyte r = (color >> 16) & 0xFF;
    GLubyte g = (color >> 8) & 0xFF;
    GLubyte b = color & 0xFF;

    return ccColor3B{r, g, b};
  }

  ccColor3B get_player_color_rgb(bool player2) {
    int color = get_player_set_color(player2);

    return int_to_rgb(color);
  }

  void reset_colors() {
    m_player1->setColor(get_player_color_rgb(false));
    m_player2->setColor(get_player_color_rgb(true));
  }
};

struct MyHardStreak : Modify<MyHardStreak, HardStreak> {
  void addPoint(CCPoint p0) {
    float max_wave_pulse_size = Mod::get()->getSettingValue<double>("max-wave-pulse-size");
    float min_wave_pulse_size = Mod::get()->getSettingValue<double>("min-wave-pulse-size");
    float max_wave_trail_size = Mod::get()->getSettingValue<double>("max-wave-trail-size");
    float min_wave_trail_size = Mod::get()->getSettingValue<double>("min-wave-trail-size");
    float min_y_pos_offset = Mod::get()->getSettingValue<double>("min-y-pos-offset");
    float max_y_pos_offset = Mod::get()->getSettingValue<double>("max-y-pos-offset");
    float min_x_pos_offset = Mod::get()->getSettingValue<double>("min-x-pos-offset");
    float max_x_pos_offset = Mod::get()->getSettingValue<double>("max-x-pos-offset");
    float skip_chance = Mod::get()->getSettingValue<double>("skip-chance");

    bool chaos = Mod::get()->getSettingValue<bool>("chaos");

    if (chaos) {
      std::mt19937 rng(std::time(nullptr));
      std::uniform_real_distribution<float> dist_pulse(min_wave_pulse_size, max_wave_pulse_size);
      std::uniform_real_distribution<float> dist_trail(min_wave_trail_size, max_wave_trail_size);

      auto random_float_pulse = [&]() { return dist_pulse(rng); };
      auto random_float_trail = [&]() { return dist_trail(rng); };

      std::vector<std::function<void()>> operations = {
          [&]() { p0.x += random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x -= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x *= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x /= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y += random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y -= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y *= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y /= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { m_waveSize += random_float_trail(); },
          [&]() { m_waveSize -= random_float_trail(); },
          [&]() { m_waveSize *= random_float_trail(); },
          [&]() { m_waveSize /= random_float_trail(); },
          [&]() { m_pulseSize += random_float_trail(); },
          [&]() { m_pulseSize -= random_float_trail(); },
          [&]() { m_pulseSize *= random_float_trail(); },
          [&]() { m_pulseSize /= random_float_trail(); }};

      std::bernoulli_distribution enable_op(skip_chance);

      for (auto &op: operations) {
        if (enable_op(rng)) {
          op();
        }
      }
    }

    HardStreak::addPoint(p0);
  }

  float random_float(float min = -25.f, float max = 25.f) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
  }
};

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<SettingSectionValue>("gaydient-section", "none");
  Mod::get()->addCustomSetting<SettingSectionValue>("chaos-section", "none");
}
