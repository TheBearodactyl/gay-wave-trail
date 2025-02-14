#include "rainbow_trail.hpp"
#include <vector>

typedef const std::vector<cocos2d::ccColor3B> &ColorVec;

cocos2d::_ccColor3B RainbowTrail::get_rainbow_proxy(float offset, float saturation) {
    auto rainbow = get_rainbow(offset, saturation);

    return cocos2d::_ccColor3B{
            rainbow.r,
            rainbow.g,
            rainbow.b
    };
}

cocos2d::_ccColor3B RainbowTrail::get_gradient_proxy(float phase, float offset, bool smooth, ColorVec colors) {
    auto gradient = get_gradient(
            phase,
            offset,
            smooth,
            reinterpret_cast<const CCColor3BR *>(colors.data()),
            colors.size()
    );

    return cocos2d::_ccColor3B{
            gradient.r,
            gradient.g,
            gradient.b
    };
}