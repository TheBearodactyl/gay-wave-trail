#pragma once

#include <string>
#include <string_view>

#include <gay/types.hpp>

#include <cocos2d.h>

#define color(name, r, g, b) static constexpr cocos2d::ccColor3B name = {r, g, b}

namespace gay::color {
	extern float g_phase;
	cocos2d::ccColor3B hsv_to_rgb(float h, float s, float v);
	cocos2d::ccColor3B hex_to_rgb(std::string_view hex_code);
	std::string rgb_to_hex(const cocos2d::ccColor3B& color);
	std::string rgb_to_hex(const cocos2d::ccColor4B& color);
	cocos2d::ccColor3B get_rainbow(float offset, float saturation);
	cocos2d::ccColor3B get_gradient(float phase, float offset, bool smooth, const ColorList& colors);
	cocos2d::ccColor3B apply_brightness(const cocos2d::ccColor3B& color, float brightness);

	namespace rose_pine {
		color(base, 25, 23, 36);
		color(surface, 31, 29, 46);
		color(overlay, 38, 35, 58);
		color(muted, 110, 106, 134);
		color(subtle, 144, 140, 170);
		color(text, 224, 222, 244);
		color(love, 235, 111, 146);
		color(gold, 246, 193, 119);
		color(rose, 235, 188, 186);
		color(pine, 49, 116, 143);
		color(foam, 156, 207, 216);
		color(iris, 196, 167, 231);
		color(h_low, 33, 32, 46);
		color(h_med, 64, 61, 82);
		color(h_high, 82, 79, 103);
	} // namespace rose_pine
} // namespace gay::color
