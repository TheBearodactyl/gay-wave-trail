#include <algorithm>
#include <cmath>
#include <ranges>
#include <string>
#include <string_view>

#include <gay/color.hpp>
#include <gay/utils.hpp>

#include <Geode/Geode.hpp>

#include <fmt/core.h>

using namespace cocos2d;
using gay::util::LogLevel;

namespace gay::color {
	float g_phase = 0.0f;

	GLubyte lerp_channel(GLubyte a, GLubyte b, float t) {
		auto result = static_cast<GLubyte>(
			static_cast<int>(a) + static_cast<int>(static_cast<int>(b) - static_cast<int>(a)) * t
		);
		gay::util::log(
			LogLevel::Debug,
			"[lerp_channel] a={} b={} t={:.4f} -> result={}",
			(int)a,
			(int)b,
			t,
			(int)result
		);
		return result;
	}

	ccColor3B hsv_to_rgb(float h, float s, float v) {
		gay::util::log(LogLevel::Debug, "[hsv_to_rgb] input h={:.2f} s={:.4f} v={:.4f}", h, s, v);

		float c = v * s;
		float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
		float m = v - c;

		gay::util::log(
			LogLevel::Debug,
			"[hsv_to_rgb] computed c={:.4f} x={:.4f} m={:.4f}",
			c,
			x,
			m
		);

		float r = 0.0f, g = 0.0f, b = 0.0f;

		if (h < 60.0f) {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 0 (red-yellow), h={:.2f}", h);
			r = c;
			g = x;
		} else if (h < 120.0f) {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 1 (yellow-green), h={:.2f}", h);
			r = x;
			g = c;
		} else if (h < 180.0f) {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 2 (green-cyan), h={:.2f}", h);
			g = c;
			b = x;
		} else if (h < 240.0f) {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 3 (cyan-blue), h={:.2f}", h);
			g = x;
			b = c;
		} else if (h < 300.0f) {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 4 (blue-magenta), h={:.2f}", h);
			r = x;
			b = c;
		} else {
			gay::util::log(LogLevel::Debug, "[hsv_to_rgb] sector 5 (magenta-red), h={:.2f}", h);
			r = c;
			b = x;
		}

		auto result = ccColor3B {static_cast<GLubyte>((r + m) * 255.0f),
			static_cast<GLubyte>((g + m) * 255.0f),
			static_cast<GLubyte>((b + m) * 255.0f)};
		gay::util::log(
			LogLevel::Debug,
			"[hsv_to_rgb] result rgb=({},{},{})",
			(int)result.r,
			(int)result.g,
			(int)result.b
		);
		return result;
	}

	ccColor3B hex_to_rgb(std::string_view hex_code) {
		gay::util::log(LogLevel::Debug, "[hex_to_rgb] input hex='{}'", hex_code);

		if (hex_code.empty()) {
			gay::util::log(LogLevel::Warn, "[hex_to_rgb] empty hex code, returning white");
			return {255, 255, 255};
		}

		auto clean = hex_code;
		if (clean.front() == '#') {
			clean.remove_prefix(1);
			gay::util::log(LogLevel::Debug, "[hex_to_rgb] stripped '#', clean='{}'", clean);
		}

		if (clean.size() < 6) {
			gay::util::log(
				LogLevel::Warn,
				"[hex_to_rgb] hex too short (size={}), returning white",
				clean.size()
			);
			return {255, 255, 255};
		}

		int value = geode::utils::numFromString<int>(std::string(clean), 16).unwrapOr(0);
		gay::util::log(LogLevel::Debug, "[hex_to_rgb] parsed int value=0x{:06X}", value);

		auto result = ccColor3B {static_cast<GLubyte>((value >> 16) & 0xFF),
			static_cast<GLubyte>((value >> 8) & 0xFF),
			static_cast<GLubyte>(value & 0xFF)};
		gay::util::log(
			LogLevel::Debug,
			"[hex_to_rgb] result rgb=({},{},{})",
			(int)result.r,
			(int)result.g,
			(int)result.b
		);
		return result;
	}

	std::string rgb_to_hex(const ccColor3B& color) {
		auto result = fmt::format(
			"#{:02X}{:02X}{:02X}",
			static_cast<int>(color.r),
			static_cast<int>(color.g),
			static_cast<int>(color.b)
		);
		gay::util::log(
			LogLevel::Debug,
			"[rgb_to_hex] rgb=({},{},{}) -> '{}'",
			(int)color.r,
			(int)color.g,
			(int)color.b,
			result
		);
		return result;
	}

	std::string rgb_to_hex(const ccColor4B& color) {
		gay::util::log(
			LogLevel::Debug,
			"[rgb_to_hex (4B)] rgba=({},{},{},{})",
			(int)color.r,
			(int)color.g,
			(int)color.b,
			(int)color.a
		);
		return rgb_to_hex(ccColor3B {color.r, color.g, color.b});
	}

	ccColor3B get_rainbow(float offset, float saturation) {
		gay::util::log(
			LogLevel::Debug,
			"[get_rainbow] offset={:.2f} saturation={:.2f} g_phase={:.2f}",
			offset,
			saturation,
			g_phase
		);

		constexpr float DEGREES_PER_SECTOR = 1.0f / 60.0f;
		constexpr float PERCENT_TO_FRACT = 1.0f / 100.0f;

		float hue = std::fmod(g_phase + offset, 360.0f);
		if (hue < 0.0f) {
			gay::util::log(
				LogLevel::Debug,
				"[get_rainbow] hue was negative ({:.2f}), wrapping",
				hue
			);
			hue += 360.0f;
		}
		float chroma = saturation * PERCENT_TO_FRACT;
		float match = 1.0f - chroma;
		float sector_pos = hue * DEGREES_PER_SECTOR;
		int sector_idx = static_cast<int>(sector_pos);
		float intermediate = chroma * (1.0f - std::abs(std::fmod(sector_pos, 2.0f) - 1.0f));

		gay::util::log(
			LogLevel::Debug,
			"[get_rainbow] hue={:.2f} chroma={:.4f} match={:.4f} sector_idx={} intermediate={:.4f}",
			hue,
			chroma,
			match,
			sector_idx,
			intermediate
		);

		float r = 0.0f, g = 0.0f, b = 0.0f;

		switch (sector_idx) {
			case 0:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 0");
				r = chroma;
				g = intermediate;
				break;
			case 1:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 1");
				r = intermediate;
				g = chroma;
				break;
			case 2:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 2");
				g = chroma;
				b = intermediate;
				break;
			case 3:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 3");
				g = intermediate;
				b = chroma;
				break;
			case 4:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 4");
				r = intermediate;
				b = chroma;
				break;
			case 5:
				gay::util::log(LogLevel::Debug, "[get_rainbow] sector 5");
				r = chroma;
				b = intermediate;
				break;
			default:
				gay::util::log(
					LogLevel::Warn,
					"[get_rainbow] unexpected sector_idx={}, falling back to sector 0 behavior",
					sector_idx
				);
				r = chroma;
				g = intermediate;
				break;
		}

		auto result = ccColor3B {static_cast<GLubyte>((r + match) * 255.0f),
			static_cast<GLubyte>((g + match) * 255.0f),
			static_cast<GLubyte>((b + match) * 255.0f)};
		gay::util::log(
			LogLevel::Debug,
			"[get_rainbow] result rgb=({},{},{})",
			(int)result.r,
			(int)result.g,
			(int)result.b
		);
		return result;
	}

	ccColor3B get_gradient(float phase, float offset, bool smooth, const ColorList& colors) {
		gay::util::log(
			LogLevel::Debug,
			"[get_gradient] phase={:.2f} offset={:.2f} smooth={} total_colors={}",
			phase,
			offset,
			smooth,
			colors.colors.size()
		);

		auto clrs = colors.colors |
			std::views::filter([](const ColorEntry& e) { return e.enabled; }) |
			std::ranges::to<std::vector>();

		gay::util::log(LogLevel::Debug, "[get_gradient] enabled colors count={}", clrs.size());

		if (clrs.empty()) {
			gay::util::log(LogLevel::Warn, "[get_gradient] no enabled colors, returning white");
			return {255, 255, 255};
		}

		if (clrs.size() == 1) {
			gay::util::log(
				LogLevel::Debug,
				"[get_gradient] single color '{}', returning it directly",
				clrs[0].hex
			);
			return hex_to_rgb(clrs[0].hex);
		}

		float normalized = std::fmodf(phase + offset, 360.0f);

		if (normalized < 0.0f) {
			gay::util::log(
				LogLevel::Debug,
				"[get_gradient] normalized was negative ({:.2f}), wrapping",
				normalized
			);
			normalized += 360.0f;
		}

		int count = static_cast<int>(clrs.size());
		float segment_size = 360.0f / static_cast<float>(count);
		float segment_float = normalized / segment_size;
		int current = static_cast<int>(segment_float) % count;
		float t = std::clamp(
			segment_float - static_cast<float>(static_cast<int>(segment_float)),
			0.0f,
			1.0f
		);
		ccColor3B current_color = hex_to_rgb(clrs[current].hex);

		gay::util::log(
			LogLevel::Debug,
			"[get_gradient] normalized={:.2f} count={} segment_size={:.2f} segment_float={:.4f} current={} t={:.4f}",
			normalized,
			count,
			segment_size,
			segment_float,
			current,
			t
		);
		gay::util::log(
			LogLevel::Debug,
			"[get_gradient] current_color='{}' rgb=({},{},{})",
			clrs[current].hex,
			(int)current_color.r,
			(int)current_color.g,
			(int)current_color.b
		);

		if (!smooth) {
			gay::util::log(
				LogLevel::Debug,
				"[get_gradient] smooth=false, returning current color directly"
			);
			return current_color;
		}

		int next = (current + 1) % count;
		ccColor3B next_color = hex_to_rgb(clrs[next].hex);
		gay::util::log(
			LogLevel::Debug,
			"[get_gradient] interpolating current({})='{}' -> next({})='{}' at t={:.4f}",
			current,
			clrs[current].hex,
			next,
			clrs[next].hex,
			t
		);

		auto result = ccColor3B {
			lerp_channel(current_color.r, next_color.r, t),
			lerp_channel(current_color.g, next_color.g, t),
			lerp_channel(current_color.b, next_color.b, t),
		};
		gay::util::log(
			LogLevel::Debug,
			"[get_gradient] interpolated result rgb=({},{},{})",
			(int)result.r,
			(int)result.g,
			(int)result.b
		);
		return result;
	}

	ccColor3B apply_brightness(const ccColor3B& color, float brightness) {
		gay::util::log(
			LogLevel::Debug,
			"[apply_brightness] input rgb=({},{},{}) brightness={:.4f}",
			(int)color.r,
			(int)color.g,
			(int)color.b,
			brightness
		);

		auto result = ccColor3B {static_cast<GLubyte>(std::min(255.0f, color.r * brightness)),
			static_cast<GLubyte>(std::min(255.0f, color.g * brightness)),
			static_cast<GLubyte>(std::min(255.0f, color.b * brightness))};
		gay::util::log(
			LogLevel::Debug,
			"[apply_brightness] result rgb=({},{},{})",
			(int)result.r,
			(int)result.g,
			(int)result.b
		);
		return result;
	}
} // namespace gay::color
