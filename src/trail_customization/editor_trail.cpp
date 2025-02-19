#include "../settings/gay_settings.hpp"

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(HardStreak) {
    void updateStroke(float p0) {
        if (LevelEditorLayer::get() && gay::settings::get_editor_trail()) {
            m_drawStreak = true;
        }

        HardStreak::updateStroke(p0);
    }
};

class $modify(PlayerObject) {
    void placeStreakPoint() {
        if (LevelEditorLayer::get() && this->m_isDart && gay::settings::get_editor_trail()) {
            this->m_waveTrail->addPoint(this->getPosition());
        } else {
            PlayerObject::placeStreakPoint();
        }
    }

    void update(float dt) {
        PlayerObject::update(dt);

        if (LevelEditorLayer::get() && gay::settings::get_editor_trail()) {
            this->m_waveTrail->m_currentPoint = this->getPosition();
        }
    }
};