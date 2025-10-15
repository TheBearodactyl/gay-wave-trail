#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../settings/gay_settings.hpp"

using namespace geode::prelude;

class $modify(HardStreak) {
	void updateStroke(float dt) {
		if (gay::settings::get_mod_enabled() and gay::settings::get<bool>("disable-pulse") and PlayLayer::get()) {
			double min = gay::settings::get<bool>("add-min-width") ? 0.5 : 0.0;

			this->m_pulseSize = static_cast<float>(min + gay::settings::get_float("pulse-size"));
		}

		HardStreak::updateStroke(dt);
	}
};

class $modify(PlayLayer) {
	void postUpdate(float dt) {
		if (gay::settings::get_mod_enabled() and gay::settings::get<bool>("disable-trail")) {
			this->m_player1->m_waveTrail->setVisible(false);
			this->m_player2->m_waveTrail->setVisible(false);
		}

		PlayLayer::postUpdate(dt);
	}
};
