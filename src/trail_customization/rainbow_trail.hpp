#ifndef RAINBOW_TRAIL_HPP
#define RAINBOW_TRAIL_HPP

#include <cocos2d.h>
#include "../types/color.hpp"

using namespace cocos2d;

class RainbowTrail {
public:
    static _ccColor3B get_rainbow(float offset, float saturation);
    static _ccColor3B get_gradient(float phase, float offset, bool smooth, const ColorList &colors);
};

#endif