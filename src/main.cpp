#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <cmath>
#include <cocos2d.h>

#include "Geode/cocos/misc_nodes/CCMotionStreak.h"
#include "Geode/loader/ModEvent.hpp"
#include "Geode/modify/Modify.hpp"
#include "settings/multi_string_setting.hpp"
#include "settings/section.hpp"
#include "trail_customization/rainbow_trail.hpp"
#include "utils/color_utils.hpp"

using namespace geode::prelude;
using namespace cocos2d;

float phase;

struct MyPlayLayer : Modify<MyPlayLayer, PlayLayer> {
  void postUpdate(float p0) {
    PlayLayer::postUpdate(p0);

    float speed = Mod::get()->getSettingValue<double>("speed");
    float saturation = Mod::get()->getSettingValue<double>("saturation");

    std::vector<std::string> color_strings = Mod::get()->getSettingValue<MultiStringSettingStruct>("colors-list").m_strings;

    std::vector<cocos2d::ccColor3B> colors;

    for (const auto &color_string: color_strings) {
      colors.push_back(ColorUtils::hex_to_rgb(color_string));
    }

    bool mirror_players = Mod::get()->getSettingValue<bool>("mirror-players");
    bool use_gradient = Mod::get()->getSettingValue<bool>("use-gradient");
    bool enable = Mod::get()->getSettingValue<bool>("enable");
    bool noRegularTrail = Mod::get()->getSettingValue<bool>("no-reg-trail");
    bool disable_trail = Mod::get()->getSettingValue<bool>("disable-wave-trail");
    bool infinite_trail = Mod::get()->getSettingValue<bool>("infinite-trail");

    if (ColorUtils::owo >= 360) {
      ColorUtils::owo = 0;
    } else {
      ColorUtils::owo += speed / 10;
    }

    phase = fmod(phase + speed, 360.f);
    bool p2 = true;

    RainbowTrail traill;

    ccColor3B rainbowColor = RainbowTrail::get_rainbow(0, saturation);
    ccColor3B rainbowColor2 = RainbowTrail::get_rainbow(180, saturation);
    auto gradientColor = traill.get_gradient(phase, 0.0f, false, colors);
    auto gradientColor2 = traill.get_gradient(phase + 180.0f, 0.0f, false, colors);

    if (enable && m_player1->m_isDart) {
        m_player1->m_regularTrail->setVisible(!noRegularTrail);
        if (m_player1->m_waveTrail) {
            m_player1->m_waveTrail->setColor(use_gradient ? gradientColor : rainbowColor);
            m_player1->m_waveTrail->setVisible(!disable_trail);
        }
    }

    if (enable && m_player2->m_isDart) {
        m_player2->m_regularTrail->setVisible(!noRegularTrail);
        if (m_player2->m_waveTrail) {
            ccColor3B p2Color = mirror_players 
                ? (use_gradient ? gradientColor : rainbowColor)
                : (use_gradient ? gradientColor2 : rainbowColor2);
            m_player2->m_waveTrail->setColor(p2Color);
            m_player2->m_waveTrail->setVisible(!disable_trail);
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

/* persist wave trail */
struct MyMotionStreak : Modify<MyMotionStreak, PlayerObject> {
  void fadeOutStreak2(float p0) {
    if (! Mod::get()->getSettingValue<bool>("persist-wave-trail")) {
      PlayerObject::fadeOutStreak2(p0);
    }
  }
};

/* editor trail */
struct MyHardStreak : Modify<MyHardStreak, HardStreak> {
  void updateStroke(float p0) {
    if (LevelEditorLayer::get() && Mod::get()->getSettingValue<bool>("editor-trail")) {
      m_drawStreak = true;
    }

    HardStreak::updateStroke(p0);
  }
};

struct MyPlayerObject : Modify<MyPlayerObject, PlayerObject> {
  void placeStreakPoint() {
    if (LevelEditorLayer::get() && m_isDart && Mod::get()->getSettingValue<bool>("editor-trail")) {
      m_waveTrail->addPoint(this->getPosition());
    } else {
      PlayerObject::placeStreakPoint();
    }
  }

  virtual void update(float dt) {
    PlayerObject::update(dt);

    if (LevelEditorLayer::get() && Mod::get()->getSettingValue<bool>("editor-trail")) {
      m_waveTrail->m_currentPoint = this->getPosition();
    }
  }
};

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<SettingSectionValue>("gaydient-section", "none");
  Mod::get()->addCustomSetting<SettingSectionValue>("chaos-section", "none");
  Mod::get()->addCustomSetting<MultiStringSettingValue>("colors-list",
                                                        Mod::get()->getSettingDefinition("colors-list")->get<CustomSetting>()->json->get<std::vector<std::string>>("default"));
}