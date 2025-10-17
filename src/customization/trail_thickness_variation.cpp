#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct ThicknessVariationPlayLayer: Modify<ThicknessVariationPlayLayer, PlayLayer> {
	struct Fields {
		CCPoint p1_last_pos;
		CCPoint p2_last_pos;
		float p1_speed = 0.0f;
		float p2_speed = 0.0f;
	};

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!gay::settings::get_mod_enabled() || !gay::settings::get<bool>("trail-thickness-variation")) {
			return;
		}

		updateThicknessForPlayer(this->m_player1, m_fields->p1_last_pos, m_fields->p1_speed, dt);

		if (this->m_player2) {
			updateThicknessForPlayer(this->m_player2, m_fields->p2_last_pos, m_fields->p2_speed, dt);
		}
	}

	void updateThicknessForPlayer(PlayerObject* player, CCPoint& last_pos, float& speed, float dt) {
		if (!player || !player->m_isDart || dt <= 0.0f)
			return;

		CCPoint current_pos = player->getPosition();
		float distance = ccpDistance(current_pos, last_pos);
		speed = distance / dt;
		last_pos = current_pos;

		float base_size = gay::settings::get_float("trail-size");
		float multiplier = gay::settings::get_float("thickness-multiplier");

		float speed_factor = std::clamp(speed / 500.0f, 0.2f, 2.0f);
		float new_size = base_size * (1.0f + (speed_factor - 1.0f) * multiplier);

		player->m_waveTrail->m_pulseSize = new_size;
	}

	void resetLevel() {
		PlayLayer::resetLevel();

		m_fields->p1_last_pos = CCPointZero;
		m_fields->p2_last_pos = CCPointZero;
		m_fields->p1_speed = 0.0f;
		m_fields->p2_speed = 0.0f;
	}
};
