#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/settings/gay_settings.hpp>
#include <gay/utils/color_utils.hpp>

using namespace geode::prelude;
using namespace gay::utils::color_conversion;

struct MusicSyncPlayLayer: Modify<MusicSyncPlayLayer, PlayLayer> {
	struct Fields {
		float base_speed = 1.0f;
		float last_music_time = 0.0f;
		bool calculated_base = false;
	};

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!gay::settings::get<bool>("sync-to-music") || !gay::settings::get_mod_enabled()) {
			if (m_fields->calculated_base) {
				ColorUtils::phase = fmodf(ColorUtils::phase + (gay::settings::get_float("speed") * 60.0f * dt), 360.0f);
			}
			return;
		}

		if (!m_fields->calculated_base) {
			m_fields->base_speed = gay::settings::get_float("speed");
			m_fields->calculated_base = true;
		}

		auto level_settings = this->m_levelSettings;
		if (!level_settings) {
			return;
		}

		float current_time = this->m_currentTime;
		float time_delta = current_time - m_fields->last_music_time;
		m_fields->last_music_time = current_time;

		float music_speed_multiplier = 1.0f + (time_delta * 0.5f);
		music_speed_multiplier = std::clamp(music_speed_multiplier, 0.5f, 2.0f);

		float adjusted_speed = m_fields->base_speed * music_speed_multiplier;

		ColorUtils::phase = fmodf(ColorUtils::phase + (adjusted_speed * 60.0f * dt), 360.0f);
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		m_fields->last_music_time = 0.0f;
		m_fields->calculated_base = false;
	}
};
