#ifndef GAY_SETTINGS_HPP
#define GAY_SETTINGS_HPP

#pragma once

#include "../types/color.hpp"
#include "../settings/color_list/color_list_setting.hpp"

using namespace geode::prelude;

namespace gay::settings {
    template<typename T>
    inline T get(const std::string &name) {
        return Mod::get()->getSettingValue<T>(name);
    }

    inline float get_float(const std::string &name) {
        return static_cast<float>(Mod::get()->getSettingValue<double>(name));
    }

    inline bool get_mod_enabled() { return get<bool>("enabled"); }
}

#endif