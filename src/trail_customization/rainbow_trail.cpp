#include "rainbow_trail.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

#include "../settings/gay_settings.hpp"
#include "../utils/color_utils.hpp"
#include "../utils/hex_utils.hpp"

using namespace gay::utils::color_conversion;
using namespace gay::utils::hex_code;

_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
	constexpr float degrees_per_sector = 1.0f / 60.0f;
	constexpr float percent_to_fract = 1.0f / 100.0f;

	float hue = fmodf(ColorUtils::phase + offset, 360.0f);
	float chroma = saturation * percent_to_fract;
	float match = 1.0f - chroma;
	float sector_pos = hue * degrees_per_sector;
	int sector_idx = static_cast<int>(sector_pos);
	float intermediate = chroma * (1.0f - fabsf(fmodf(sector_pos, 2.0f) - 1.0f));

	float r, g, b = 0.0f;

	switch (sector_idx) {
		case 0:
			r = chroma;
			g = intermediate;
			b = 0.0f;
			break;
		case 1:
			r = intermediate;
			g = chroma;
			b = 0.0f;
			break;
		case 2:
			r = 0.0f;
			g = chroma;
			b = intermediate;
			break;
		case 3:
			r = 0.0f;
			g = intermediate;
			b = chroma;
			break;
		case 4:
			r = intermediate;
			g = 0.0f;
			b = chroma;
			break;
		case 5:
			r = chroma;
			g = 0.0f;
			b = intermediate;
			break;
	}

	return {
		static_cast<GLubyte>((r + match) * 255.0f),
		static_cast<GLubyte>((g + match) * 255.0f),
		static_cast<GLubyte>((b + match) * 255.0f)
	};
}

_ccColor3B RainbowTrail::get_gradient(float phase, float offset, bool smooth, const ColorList& colors) {
	const auto& clrs = colors.colors;
	if (clrs.empty()) {
		return {255, 255, 255};
	}

	if (clrs.size() == 1) {
		return HexUtils::hex_to_rgb(clrs[0].m_hex);
	}

	const float normalized_phase = fmodf(phase + offset, 360.0f);
	const float segment_size = 360.0f / static_cast<float>(clrs.size());
	const float segment_float = normalized_phase / segment_size;
	const int current_segment = static_cast<int>(segment_float) % clrs.size();
	const float interpolation_factor = segment_float - static_cast<int>(segment_float);
	const auto current_color = HexUtils::hex_to_rgb(clrs[current_segment].m_hex);

	if (!smooth) {
		return current_color;
	}

	const int next_segment = (current_segment + 1) % clrs.size();
	const auto next_color = HexUtils::hex_to_rgb(clrs[next_segment].m_hex);

	return {
		static_cast<GLubyte>(current_color.r + (next_color.r - current_color.r) * interpolation_factor),
		static_cast<GLubyte>(current_color.g + (next_color.g - current_color.g) * interpolation_factor),
		static_cast<GLubyte>(current_color.b + (next_color.b - current_color.b) * interpolation_factor)
	};
}

float phase;

struct RainbowTrailPlayLayer: Modify<RainbowTrailPlayLayer, PlayLayer> {
	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		auto gradient_colors = gay::settings::get<ColorList>("gaydient-colors");
		float speed = gay::settings::get_float("speed");

		ColorUtils::phase = fmodf(ColorUtils::phase + (speed * 60.0f * dt), 360.0f);

		phase = fmodf(phase + (speed * 60.0f * dt), 360.0f);

		auto rainbow_one = RainbowTrail::get_rainbow(0, gay::settings::get_float("saturation"));
		auto rainbow_two = RainbowTrail::get_rainbow(180, gay::settings::get_float("saturation"));
		auto gradient_one = RainbowTrail::get_gradient(phase, 0.0f, gay::settings::get<bool>("smooth-colors"), gradient_colors);
		auto gradient_two = RainbowTrail::get_gradient(phase + 180.0f, 0.0f, gay::settings::get<bool>("smooth-colors"), gradient_colors);

		if (this->m_player1 and gay::settings::get_mod_enabled() and this->m_player1->m_isDart) {
			this->m_player1->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient") ? gradient_one : rainbow_one);
		}

		if (this->m_player2 and gay::settings::get_mod_enabled() and this->m_player2->m_isDart) {
			this->m_player2->m_waveTrail->setColor(gay::settings::get<bool>("enable-gaydient") ? gradient_two : rainbow_two);
		}
	}
};
