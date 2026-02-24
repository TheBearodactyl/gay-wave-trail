#include <unordered_set>

#include <gay/settings.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;
using settings::CheckMode;

extern std::unordered_set<HardStreak*> g_streaks_in_gap;

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
		bool in_gap = g_streaks_in_gap.count(this) > 0;
		if (!in_gap && settings::is_enabled() && settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			this->m_drawStreak = true;
		}
		HardStreak::updateStroke(dt);
	}

	void stopStroke() {
		bool in_gap = g_streaks_in_gap.count(this) > 0;
		if (!settings::is_enabled() || settings::check<CheckMode::None>("persist-trail", "segment-persist") || in_gap) {
			HardStreak::stopStroke();
		}
	}
};

struct EditorStreakHook: Modify<EditorStreakHook, HardStreak> {
	void updateStroke(float dt) {
		if (LevelEditorLayer::get() &&
			(!settings::is_enabled() || settings::check<CheckMode::None>("persist-trail", "segment-persist") ||
				g_streaks_in_gap.find(this) != g_streaks_in_gap.end())) {
			HardStreak::stopStroke();
			return;
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
