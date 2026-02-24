#include <gay/settings.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;
using settings::CheckMode;

struct PersistPlayerHook: Modify<PersistPlayerHook, PlayerObject> {
	void createFadeOutDartStreak() {
		if (settings::is_enabled() && settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			m_alwaysShowStreak = true;
		}

		if (!settings::is_enabled() || !settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			m_alwaysShowStreak = false;
			PlayerObject::createFadeOutDartStreak();
		}
	}

	void fadeOutStreak2(float dt) {
		if (settings::is_enabled() && settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			m_alwaysShowStreak = true;
		}

		if (!settings::is_enabled() || !settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			m_alwaysShowStreak = false;
			PlayerObject::fadeOutStreak2(dt);
		}
	}
};

struct PersistStreakHook: Modify<PersistStreakHook, HardStreak> {
	void updateStroke(float dt) {
		if (settings::is_enabled() && settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			this->m_drawStreak = true;
		}
		HardStreak::updateStroke(dt);
	}

	void stopStroke() {
		if (!settings::is_enabled() || settings::check<CheckMode::None>("persist-trail", "segment-persist")) {
			HardStreak::stopStroke();
		}
	}
};

struct EditorStreakHook: Modify<EditorStreakHook, HardStreak> {
	void updateStroke(float dt) {
		if (LevelEditorLayer::get() && settings::get<bool>("editor-trail")) {
			m_drawStreak = true;
		}
		HardStreak::updateStroke(dt);
	}
};

struct EditorPlayerHook: Modify<EditorPlayerHook, PlayerObject> {
	void placeStreakPoint() {
		if (LevelEditorLayer::get() && settings::get<bool>("editor-trail") && this->m_isDart && this->m_waveTrail) {
			this->m_waveTrail->addPoint(this->getPosition());
		} else {
			PlayerObject::placeStreakPoint();
		}
	}

	void update(float dt) {
		PlayerObject::update(dt);

		if (LevelEditorLayer::get() && settings::get<bool>("editor-trail") && this->m_waveTrail) {
			this->m_waveTrail->m_currentPoint = this->getPosition();
		}
	}
};
