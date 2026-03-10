#pragma once

#include <cstddef>
#include <cstdint>

namespace gay {
	constexpr float MAX_HUE = 360.0f;
	constexpr float MAX_RGB = 255.0f;
	constexpr float GRADIENT_SPEED_MULTIPLIER = 10.0f;
	constexpr float HALF_ROTATION = 180.0f;
	constexpr float OUTLINE_LAYER_SCALE = 0.8f;
	constexpr float MIN_LAYER_OPACITY = 0.05f;
	constexpr float OUTER_BORDER = 2.f;
	constexpr float PAD = 2.f;
	constexpr float LIST_BORDER = 1.5f;
	static constexpr float ROW_HEIGHT_NORMAL = 30.0f;
	static constexpr float INFO_BTN_WIDTH = 20.0f;
	inline int32_t s_row_index = 0;
	static constexpr float CTRL_W = 140.0f;
	static constexpr float CTRL_RIGHT_PAD = 6.0f;
	static constexpr float RESET_BTN_WIDTH = 18.0f;
	static constexpr size_t RING_CAP = 1u << 12;
	static constexpr size_t RING_MASK = RING_CAP - 1;
	static constexpr float MITER_LIMIT = 4.0f;
	static constexpr float MIN_DIST = 0.1f;
	static constexpr float MAX_DT = 1.0f / 20.0f;
	static constexpr size_t MAX_TRAIL_POINTS = 200000;
} // namespace gay
