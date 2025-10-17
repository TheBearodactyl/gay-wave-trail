#ifndef RAINBOW_HPP
#define RAINBOW_HPP

#include <gay/types/color.hpp>

#include <cocos2d.h>

using namespace cocos2d;

class RainbowTrail {
  public:
	static _ccColor3B get_rainbow(float offset, float saturation);
	static _ccColor3B get_gradient(float phase, float offset, bool smooth, const ColorList& colors);
};

#endif
