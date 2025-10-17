#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
	void createFadeOutDartStreak() {
		if (!gay::settings::get<bool>("persist-trail") || !gay::settings::get<bool>("segment-persist")) {
			PlayerObject::createFadeOutDartStreak();
		}
	}

	void fadeOutStreak2(float dt) {
		if (!gay::settings::get<bool>("persist-trail") || !gay::settings::get<bool>("segment-persist")) {
			PlayerObject::fadeOutStreak2(dt);
		}
	}
};

class $modify(HardStreak) {
	void updateStroke(float dt) {
		if (gay::settings::get<bool>("persist-trail") || gay::settings::get<bool>("segment-persist")) {
			this->m_drawStreak = true;
		}

		HardStreak::updateStroke(dt);
	}

	void stopStroke() {
		if (!gay::settings::get<bool>("persist-trail") || !gay::settings::get<bool>("segment-persist")) {
			HardStreak::stopStroke();
		}
	}
};
