#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <string>
#include "trail_customization/rainbow_trail.hpp"

using namespace geode::prelude;

class $modify(PlayerObject) {
  void flashPlayer(float p0, float p1, cocos2d::ccColor3B mainColor, cocos2d::ccColor3B secondColor) {}
};

class $modify(PlayLayer) {
    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);

        float speed = Mod::get()->getSettingValue<double>("speed");
        float saturation = Mod::get()->getSettingValue<double>("saturation");
        bool mirror_players = Mod::get()->getSettingValue<bool>("mirror-players");

        if (RainbowTrail::g >= 360) {
            RainbowTrail::g = 0;
        } else {
            RainbowTrail::g += speed / 10;
        }

        bool enable = Mod::get()->getSettingValue<bool>("enable");
        bool noRegularTrail = Mod::get()->getSettingValue<bool>("no-reg-trail");

        if (m_player1->m_isDart && noRegularTrail) {
            m_player1->m_regularTrail->setVisible(false);
        }

        if (m_player2->m_isDart && noRegularTrail) {
            m_player2->m_regularTrail->setVisible(false);
        }

        if (enable) {
            if (m_player1->m_waveTrail) {
                m_player1->m_waveTrail->setColor(rainbowColor);
            }

            if (m_player2->m_waveTrail) {
                m_player2->m_waveTrail->setColor(!mirror_players ? rainbowColor2 : rainbowColor);
            }
        }
    }
};
