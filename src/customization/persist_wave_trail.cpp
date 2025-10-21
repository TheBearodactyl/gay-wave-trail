#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

using gay::settings::CheckMode;

struct PersistTrailPlayerObject: Modify<PersistTrailPlayerObject, PlayerObject> {
	void createFadeOutDartStreak() {
		if (!gay::settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			PlayerObject::createFadeOutDartStreak();
		}
	}

	void fadeOutStreak2(float dt) {
		if (!gay::settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			PlayerObject::fadeOutStreak2(dt);
		}
	}
};

struct PersistTrailHardStreak: Modify<PersistTrailHardStreak, HardStreak> {
	void updateStroke(float dt) {
		if (gay::settings::check<CheckMode::Any>("persist-trail", "segment-persist")) {
			this->m_drawStreak = true;
		}

		HardStreak::updateStroke(dt);
	}

	void stopStroke() {
		if (gay::settings::check<CheckMode::None>("persist-trail", "segment-persist")) {
			HardStreak::stopStroke();
		}
	}
};
