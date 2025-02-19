#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/HardStreak.hpp>

#include "../settings/gay_settings.hpp"

using namespace geode::prelude;

class $modify(PlayerObject) {
    void createFadeOutDartStreak() {
        if (!gay::settings::get_persist_trail()) {
            PlayerObject::createFadeOutDartStreak();
        }
    }

    void fadeOutStreak2(float dt) {
        if (!gay::settings::get_persist_trail()) {
            PlayerObject::fadeOutStreak2(dt);
        }
    }
};

class $modify(HardStreak) {
    void updateStroke(float dt) {
        HardStreak::updateStroke(dt);

        if (gay::settings::get_persist_trail()) {
            this->m_drawStreak = true;
        }
    }

    void stopStroke() {
        if (!gay::settings::get_persist_trail()) {
            HardStreak::stopStroke();
        }
    }
};