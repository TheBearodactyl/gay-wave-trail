#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <string>
#include <cocos2d.h>
#include <cmath>

#include "trail_customization/rainbow_trail.hpp"

using namespace geode::prelude;
using namespace cocos2d;

float phase;

class $modify(PlayerObject) {
    void flashPlayer(float p0, float p1, cocos2d::ccColor3B mainColor, cocos2d::ccColor3B secondColor) {}
};

class $modify(PlayLayer) {
    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);

        float speed = Mod::get()->getSettingValue<double>("speed");
        float saturation = Mod::get()->getSettingValue<double>("saturation");

        bool mirror_players = Mod::get()->getSettingValue<bool>("mirror-players");
        bool use_gradient = Mod::get()->getSettingValue<bool>("use-gradient");
        bool enable = Mod::get()->getSettingValue<bool>("enable");
        bool noRegularTrail = Mod::get()->getSettingValue<bool>("no-reg-trail");

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
            if (!use_gradient) {
                if (m_player1->m_waveTrail) {
                    m_player1->m_waveTrail
                            ->setColor(rainbowColor);
                }

                if (m_player2->m_waveTrail) {
                    m_player2->m_waveTrail
                            ->setColor(!mirror_players
                                       ? rainbowColor2
                                       : rainbowColor);
                }
            } else {
                if (m_player1->m_waveTrail) {
                    m_player1->m_waveTrail
                            ->setColor(gradientColor);
                }

                if (m_player2->m_waveTrail) {
                    m_player2->m_waveTrail
                            ->setColor(!mirror_players
                                       ? gradientColor
                                       : gradientColor2);
                }
            }
        }
    }
};
