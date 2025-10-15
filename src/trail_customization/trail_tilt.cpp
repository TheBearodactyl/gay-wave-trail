#include <Geode/modify/HardStreak.hpp>

#include "../settings/gay_settings.hpp"

using namespace geode::prelude;

struct TrailTiltHardStreak: Modify<TrailTiltHardStreak, HardStreak> {
	void addPoint(cocos2d::CCPoint point_coords) {
		auto out_point = CCPoint {point_coords.x, point_coords.y};

		if (gay::settings::get<bool>("tilt-enabled") and gay::settings::get_mod_enabled()) {
			out_point.x += gay::settings::get_float("tilt-x-offset");
			out_point.y += gay::settings::get_float("tilt-y-offset");

			HardStreak::addPoint(out_point);
		}

		HardStreak::addPoint(out_point);
	}
};
