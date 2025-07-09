#include "rainbow_trail.hpp"
#include "../constants.hpp"
#include "../settings/gay_settings.hpp"
#include "../utils/color_utils.hpp"
#include "../utils/hex_utils.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

using namespace gay::utils::color_conversion;
using namespace gay::utils::hex_code;

_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
    constexpr float inv60 = 1.0f / 60.0f;
    constexpr float inv100 = 1.0f / 100.0f;

    float hue = fmodf(ColorUtils::owo + offset, 360.0f);
    float c = saturation * inv100;
    float m = 1.0f - c;
    float h = hue * inv60;
    int sector = static_cast<int>(h);
    float x = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
    float r = ((sector == 0) | (sector == 5)) ? c : ((sector == 1) | (sector == 4)) ? x : 0.0f;
    float g = ((sector == 1) | (sector == 2)) ? c : (sector == 0) | (sector == 3) ? x : 0.0f;
    float b = ((sector == 3) | (sector == 4)) ? c : (sector == 2) | (sector == 5) ? x : 0.0f;

    return {
        static_cast<GLubyte>((r + m) * 255.0f),
        static_cast<GLubyte>((g + m) * 255.0f),
        static_cast<GLubyte>((b + m) * 255.0f)
    };
}

_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const ColorList &colors) {
    const auto& clrs = colors.colors;
    if (clrs.empty()) [[unlikely]] {
        return {255, 255, 255};
    }

    const float t = fmodf(phase + offset, 360.0f);
    const float inv_size = 1.0f / static_cast<float>(clrs.size());
    const float seg_size = 360.0f / inv_size;
    const float seg_float = t * inv_size * (1.0f / seg_size) * (float)clrs.size();
    const int seg = static_cast<int>(seg_float);
    const float y = seg_float - seg;
    const int nx_seg = (seg + 1) % clrs.size();
    const auto col = HexUtils::hex_to_rgb(clrs[seg].m_hex);

    if (!smooth) [[likely]] {
        return col;
    }

    const auto nx_col = HexUtils::hex_to_rgb(clrs[nx_seg].m_hex);
    const float y_smth = smooth ? y : 0.0f;

    return {
            static_cast<GLubyte>(col.r + (nx_col.r - col.r) * y_smth),
            static_cast<GLubyte>(col.g + (nx_col.g - col.g) * y_smth),
            static_cast<GLubyte>(col.b + (nx_col.b - col.b) * y_smth)
    };

}

float phase;

class $modify(PlayLayer) {
    void postUpdate(float dt) override {
        PlayLayer::postUpdate(dt);

        auto gradient_colors = gay::settings::get<ColorList>("gaydient-colors");
        float speed = gay::settings::get_float("speed");

        ColorUtils::owo = fmodf(ColorUtils::owo + (speed * 60.0f * dt), 360.0f);

        phase = fmodf(phase + (speed * 60.0f * dt), 360.0f);

        auto rainbow_one = RainbowTrail::get_rainbow(0, gay::settings::get_float("saturation"));
        auto rainbow_two = RainbowTrail::get_rainbow(180, gay::settings::get_float("saturation"));
        auto gradient_one = RainbowTrail::get_gradient(phase, 0.0f, gay::settings::get<bool>("smooth-colors"),
                                                       gradient_colors);
        auto gradient_two = RainbowTrail::get_gradient(phase + 180.0f, 0.0f, gay::settings::get<bool>("smooth-colors"),
                                                       gradient_colors);

        if (this->m_player1 && gay::settings::get_mod_enabled() && this->m_player1->m_isDart) {
            this->m_player1->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient")
                                                       ? gradient_one
                                                       : rainbow_one);
        }

        if (this->m_player2 && gay::settings::get_mod_enabled() && this->m_player2->m_isDart) {
            this->m_player2->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient")
                                                       ? gradient_two
                                                       : rainbow_two);
        }
    }
};
