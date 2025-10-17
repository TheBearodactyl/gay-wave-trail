#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct TrailFadeHardStreak: Modify<TrailFadeHardStreak, HardStreak> {
	struct Fields {
		float accumulated_time = 0.0f;
	};

	void updateStroke(float dt) {
		HardStreak::updateStroke(dt);

		if (!gay::settings::get_mod_enabled()) {
			return;
		}

		auto fade_speed = gay::settings::get_float("trail-fade-speed");
		auto min_opacity = gay::settings::get<int>("trail-min-opacity");

		m_fields->accumulated_time += dt * fade_speed;

		auto base_opacity = this->getOpacity();
		auto target_opacity = static_cast<GLubyte>(std::max(min_opacity, static_cast<int>(base_opacity)));

		this->setOpacity(target_opacity);
	}
};
