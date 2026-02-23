#include <algorithm>
#include <cmath>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include <gay/color.hpp>

#include <Geode/Geode.hpp>

#include <fmt/core.h>

using namespace cocos2d;

namespace gay::color {
	float g_phase = 0.0f;

	ccColor3B hsv_to_rgb(float h, float s, float v) {
		float c = v * s;
		float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
		float m = v - c;

		float r = 0.0f, g = 0.0f, b = 0.0f;

		if (h < 60.0f) {
			r = c;
			g = x;
		} else if (h < 120.0f) {
			r = x;
			g = c;
		} else if (h < 180.0f) {
			g = c;
			b = x;
		} else if (h < 240.0f) {
			g = x;
			b = c;
		} else if (h < 300.0f) {
			r = x;
			b = c;
		} else {
			r = c;
			b = x;
		}

		return {static_cast<GLubyte>((r + m) * 255.0f), static_cast<GLubyte>((g + m) * 255.0f), static_cast<GLubyte>((b + m) * 255.0f)};
	}

	ccColor3B hex_to_rgb(std::string_view hex_code) {
		if (hex_code.empty()) {
			return {255, 255, 255};
		}

		auto clean = hex_code;
		if (clean.front() == '#') {
			clean.remove_prefix(1);
		}

		if (clean.size() < 6) {
			return {255, 255, 255};
		}

		int value = geode::utils::numFromString<int>(std::string(clean), 16).unwrapOr(0);

		return {static_cast<GLubyte>((value >> 16) & 0xFF), static_cast<GLubyte>((value >> 8) & 0xFF), static_cast<GLubyte>(value & 0xFF)};
	}

	std::string rgb_to_hex(const ccColor3B& color) {
		return fmt::format("#{:02X}{:02X}{:02X}", static_cast<int>(color.r), static_cast<int>(color.g), static_cast<int>(color.b));
	}

	std::string rgb_to_hex(const ccColor4B& color) {
		return rgb_to_hex(ccColor3B {color.r, color.g, color.b});
	}

	ccColor3B get_rainbow(float offset, float saturation) {
		constexpr float DEGREES_PER_SECTOR = 1.0f / 60.0f;
		constexpr float PERCENT_TO_FRACT = 1.0f / 100.0f;

		float hue = std::fmod(g_phase + offset, 360.0f);
		if (hue < 0.0f) {
			hue += 360.0f;
		}
		float chroma = saturation * PERCENT_TO_FRACT;
		float match = 1.0f - chroma;
		float sector_pos = hue * DEGREES_PER_SECTOR;
		int sector_idx = static_cast<int>(sector_pos);
		float intermediate = chroma * (1.0f - std::abs(std::fmod(sector_pos, 2.0f) - 1.0f));

		float r = 0.0f, g = 0.0f, b = 0.0f;

		switch (sector_idx) {
			case 0:
				r = chroma;
				g = intermediate;
				break;
			case 1:
				r = intermediate;
				g = chroma;
				break;
			case 2:
				g = chroma;
				b = intermediate;
				break;
			case 3:
				g = intermediate;
				b = chroma;
				break;
			case 4:
				r = intermediate;
				b = chroma;
				break;
			case 5:
				r = chroma;
				b = intermediate;
				break;
			default:
				std::unreachable();
		}

		return {
			static_cast<GLubyte>((r + match) * 255.0f),
			static_cast<GLubyte>((g + match) * 255.0f),
			static_cast<GLubyte>((b + match) * 255.0f)
		};
	}

	ccColor3B get_gradient(float phase, float offset, bool smooth, const ColorList& colors) {
		auto clrs = colors.colors | std::views::filter([](const ColorEntry& e) { return e.enabled; }) | std::ranges::to<std::vector>();

		if (clrs.empty()) {
			return {255, 255, 255};
		}

		if (clrs.size() == 1) {
			return hex_to_rgb(clrs[0].hex);
		}

		float normalized = std::fmod(phase + offset, 360.0f);
		if (normalized < 0.0f) {
			normalized += 360.0f;
		}

		float segment_size = 360.0f / static_cast<float>(clrs.size());
		if (segment_size <= 0.0f) {
			return hex_to_rgb(clrs[0].hex);
		}

		float segment_float = normalized / segment_size;
		auto count = static_cast<int>(clrs.size());
		int current = static_cast<int>(segment_float) % count;
		float t = segment_float - std::floor(segment_float);

		auto current_color = hex_to_rgb(clrs[current].hex);

		if (!smooth) {
			return current_color;
		}

		int next = (current + 1) % count;
		auto next_color = hex_to_rgb(clrs[next].hex);

		return {
			static_cast<GLubyte>(current_color.r + (next_color.r - current_color.r) * t),
			static_cast<GLubyte>(current_color.g + (next_color.g - current_color.g) * t),
			static_cast<GLubyte>(current_color.b + (next_color.b - current_color.b) * t)
		};
	}

	ccColor3B apply_brightness(const ccColor3B& color, float brightness) {
		return {
			static_cast<GLubyte>(std::min(255.0f, color.r * brightness)),
			static_cast<GLubyte>(std::min(255.0f, color.g * brightness)),
			static_cast<GLubyte>(std::min(255.0f, color.b * brightness))
		};
	}

} // namespace gay::color
