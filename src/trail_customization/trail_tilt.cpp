#include "../settings/gay_settings.hpp"
#include <Geode/modify/HardStreak.hpp>

using namespace geode::prelude;

class $modify(HardStreak) {
    void addPoint(cocos2d::CCPoint point_coords) { // NOLINT(*-unnecessary-value-param)
        auto out_point = CCPoint{point_coords.x, point_coords.y};

        if (gay::settings::get_enable_tilt()) {
            out_point.x += gay::settings::get_tilt_x_offset();
            out_point.y += gay::settings::get_tilt_y_offset();

            HardStreak::addPoint(out_point);
        }

        HardStreak::addPoint(out_point);
    }
};