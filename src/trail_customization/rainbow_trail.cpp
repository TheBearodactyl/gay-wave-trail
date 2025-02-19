#include "rainbow_trail.hpp"
#include "../utils/color_utils.hpp"
#include "../utils/hex_utils.hpp"
#include "../settings/gay_settings.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

using namespace gay::utils::color_conversion;
using namespace gay::utils::hex_code;

_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
    float r;
    float g;
    float b;

    auto hue = static_cast<float>(fmod(ColorUtils::owo + offset, 360));
    auto sat = static_cast<float>(static_cast<double>(saturation) / 100.0);
    float vc = 1;

    auto rgb = ColorUtils::hsv_to_rgb(hue, sat, vc);

    r = rgb.r;
    g = rgb.g;
    b = rgb.b;

    cocos2d::_ccColor3B out{};

    out.r = static_cast<GLubyte>(r * 255);
    out.g = static_cast<GLubyte>(g * 255);
    out.b = static_cast<GLubyte>(b * 255);

    return out;
}

_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const ColorList &colors) {
    auto clrs = colors.colors;
    auto t = fmodf(phase + offset, 360.f);
    auto segment_size = 360.0f / clrs.size();
    auto segment = static_cast<int>(t / segment_size);
    auto y = (t - segment * segment_size) / segment_size;
    auto next_segment = (segment + 1) % clrs.size();

    auto color = HexUtils::hex_to_rgb(clrs[segment].m_hex);
    auto next_color = HexUtils::hex_to_rgb(clrs[next_segment].m_hex);

    _ccColor3B out{};

    out.r = static_cast<GLubyte>(color.r + (next_color.r - color.r) * y);
    out.g = static_cast<GLubyte>(color.g + (next_color.g - color.g) * y);
    out.b = static_cast<GLubyte>(color.b + (next_color.b - color.b) * y);

    return out;
}

float phase;

class $modify(PlayLayer) {
    void postUpdate(float p0) override {
        PlayLayer::postUpdate(p0);

        auto gradient_colors = gay::settings::get_gaydient_colors();

        float delta = p0;
        if (ColorUtils::owo >= 360) {
            ColorUtils::owo = 0;
        } else {
            ColorUtils::owo += (gay::settings::get_speed() + delta);
        }

        phase = fmod(phase + ((gay::settings::get_speed() * 10.f) * p0), 360.f);

        auto rainbow_one = RainbowTrail::get_rainbow(0, gay::settings::get_saturation());
        auto rainbow_two = RainbowTrail::get_rainbow(180, gay::settings::get_saturation());
        auto gradient_one = RainbowTrail::get_gradient(phase, 0.0f, true, gradient_colors);
        auto gradient_two = RainbowTrail::get_gradient(phase + 180.0f, 0.0f, true, gradient_colors);

        if (this->m_player1 && gay::settings::get_mod_enabled() && this->m_player1->m_isDart) {
            this->m_player1->m_waveTrail->setColor(gay::settings::get_gaydient_enabled() ? gradient_one : rainbow_one);
        }

        if (this->m_player2 && gay::settings::get_mod_enabled() && this->m_player2->m_isDart) {
            this->m_player2->m_waveTrail->setColor(gay::settings::get_gaydient_enabled() ? gradient_two : rainbow_two);
        }
    }
};