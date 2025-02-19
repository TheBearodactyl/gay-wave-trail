#include "../settings/gay_settings.hpp"
#include <Geode/modify/HardStreak.hpp>

using namespace geode::prelude;

class $modify(HardStreak) {
    void addPoint(cocos2d::CCPoint point_coords) {
        auto out_point = CCPoint{point_coords.x, point_coords.y};

        if (gay::settings::get<bool>("enabled")) {
            out_point.x += gay::settings::get_float("tilt-x-offset");
            out_point.y += gay::settings::get_float("tilt-y-offset");

            HardStreak::addPoint(out_point);
        }

        HardStreak::addPoint(out_point);
    }
};