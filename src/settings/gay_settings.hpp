#ifndef GAY_SETTINGS_HPP
#define GAY_SETTINGS_HPP

#pragma once

#include "../types/color.hpp"
#include "../settings/color_list/color_list_setting.hpp"

using namespace geode::prelude;

namespace gay::settings {
    inline bool get_mod_enabled() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }

    inline float get_speed() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("speed"));
    }

    inline float get_saturation() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("speed"));
    }

    inline bool get_solid_trail() {
        return Mod::get()->getSettingValue<bool>("solid-trail");
    }

    inline bool get_gaydient_enabled() {
        return Mod::get()->getSettingValue<bool>("enable-gaydient");
    }

    inline float get_trail_size() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("trail-size"));
    }

    inline float get_pulse_size() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("pulse-size"));
    }

    inline bool get_editor_trail() {
        return Mod::get()->getSettingValue<bool>("editor-trail");
    }

    inline bool get_persist_trail() {
        return Mod::get()->getSettingValue<bool>("persist-trail");
    }

    inline bool get_disable_pulse() {
        return Mod::get()->getSettingValue<bool>("disable-pulse");
    }

    inline bool get_enable_tilt() {
        return Mod::get()->getSettingValue<bool>("tilt-enabled");
    }

    inline float get_tilt_x_offset() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("tilt-x-offset"));
    }

    inline float get_tilt_y_offset() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("tilt-y-offset"));
    }

    inline ColorList get_gaydient_colors() {
        return Mod::get()->getSettingValue<ColorList>("gaydient-colors");
    }
}

#endif