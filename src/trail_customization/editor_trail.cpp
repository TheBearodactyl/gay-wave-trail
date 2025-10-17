#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

class $modify(HardStreak) {
	void updateStroke(float p0) {
		if (LevelEditorLayer::get() and gay::settings::get<bool>("editor-trail")) {
			m_drawStreak = true;
		}

		HardStreak::updateStroke(p0);
	}
};

class $modify(PlayerObject) {
	void placeStreakPoint() {
		if (LevelEditorLayer::get() and gay::settings::get<bool>("editor-trail") and this->m_isDart) {
			this->m_waveTrail->addPoint(this->getPosition());
		} else {
			PlayerObject::placeStreakPoint();
		}
	}

	void update(float dt) {
		PlayerObject::update(dt);

		if (LevelEditorLayer::get() and gay::settings::get<bool>("editor-trail")) {
			this->m_waveTrail->m_currentPoint = this->getPosition();
		}
	}
};
