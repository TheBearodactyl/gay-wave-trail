#pragma once

#include <gay/types.hpp>
#include <gay/utils.hpp>

namespace gay::geom {
	void emit_fill_quads(
		Vec<gay::Triangle>& out,
		const float* xs,
		const float* ys,
		const float* ws,
		size_t n,
		gay::JoinStyle join_style = gay::JoinStyle::Sharp,
		float miter_limit = 4.0f
	) noexcept;

	void emit_outline_quads(
		Vec<gay::Triangle>& upper,
		Vec<gay::Triangle>& lower,
		const float* xs,
		const float* ys,
		const float* inner_ws,
		const float* outer_ws,
		size_t n,
		gay::JoinStyle join_style = gay::JoinStyle::Sharp,
		float miter_limit = 4.0f
	) noexcept;
} // namespace gay::geom
