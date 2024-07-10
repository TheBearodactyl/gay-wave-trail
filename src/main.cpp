#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <cmath>
#include <cocos2d.h>

#include "Geode/loader/ModEvent.hpp"
#include "Geode/modify/Modify.hpp"
#include "settings/section.hpp"
#include "trail_customization/rainbow_trail.hpp"

using namespace geode::prelude;
using namespace cocos2d;

float phase;

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

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<SettingSectionValue>("gaydient-section", "none");
  Mod::get()->addCustomSetting<SettingSectionValue>("chaos-section", "none");
}
