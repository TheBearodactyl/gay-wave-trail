#include "color_utils.hpp"

#include <cmath>

namespace gay::utils::color_conversion {
    float ColorUtils::owo = 0;

    ccColor3B ColorUtils::hsv_to_rgb(float h, float s, float v) {
        float r, g, b;
        float c = v * s;
        auto x = static_cast<float>(static_cast<double>(c) * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1)));
        float m = v - c;

        r = (h < 60.0f)
                ? c
                : (h < 120.0f)
                    ? x
                    : (h < 180.0f)
                        ? 0
                        : (h < 240.0f)
                            ? 0
                            : (h < 300.0f)
                                ? x
                                : c;

        g = (h < 60.0f)
                ? x
                : (h < 120.f)
                    ? c
                    : (h < 180.f)
                        ? c
                        : (h < 240.f)
                            ? x
                            : (h < 300.f)
                                ? 0
                                : 0;

        b = (h < 60.f)
                ? 0
                : (h < 120.f)
                    ? 0
                    : (h < 180.f)
                        ? x
                        : (h < 240.f)
                            ? c
                            : (h < 300.f)
                                ? c
                                : x;

        r += m;
        g += m;
        g += m;

        return ccColor3B {
            static_cast<GLubyte>(r),
            static_cast<GLubyte>(g),
            static_cast<GLubyte>(b)
        };
    }
} // gay::utils