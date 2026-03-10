#include <cmath>
#include <cstddef>

#include <gay/geometry.hpp>
#include <gay/types.hpp>
#include <gay/utils.hpp>

namespace gay::geom {
	static inline void seg_normal(float x0, float y0, float x1, float y1, float& nx, float& ny) noexcept {
		const float dx = x1 - x0;
		const float dy = y1 - y0;
		const float ln = std::sqrt(dx * dx + dy * dy);
		if (ln > 1e-6f) {
			nx = -dy / ln;
			ny = dx / ln;
		} else {
			nx = 0.0f;
			ny = 0.0f;
		}
	}

	void emit_fill_quads(
		Vec<gay::Triangle>& out,
		const float* xs,
		const float* ys,
		const float* ws,
		size_t n,
		gay::JoinStyle join_style,
		float /*miter_limit*/
	) noexcept {
		if (n < 2) {
			return;
		}

		const bool fill_joins = join_style != gay::JoinStyle::None;
		const size_t join_reserve = fill_joins ? (n >= 2 ? n - 2 : 0) * 2 : size_t {0};

		out.reserve(out.size() + (n - 1) * 2 + join_reserve);

		float prev_nx = 0.0f;
		float prev_ny = 0.0f;

		for (size_t i = 0; i + 1 < n; ++i) {
			float nx;
			float ny;

			seg_normal(xs[i], ys[i], xs[i + 1], ys[i + 1], nx, ny);

			if (nx == 0.0f && ny == 0.0f) {
				prev_nx = nx;
				prev_ny = ny;
				continue;
			}

			const float hw0 = ws[i];
			const float hw1 = ws[i + 1];
			const CCPoint ul {xs[i] + nx * hw0, ys[i] + ny * hw0};
			const CCPoint ur {xs[i + 1] + nx * hw1, ys[i + 1] + ny * hw1};
			const CCPoint ll {xs[i] - nx * hw0, ys[i] - ny * hw0};
			const CCPoint lr {xs[i + 1] - nx * hw1, ys[i + 1] - ny * hw1};

			gay::Triangle t0, t1;
			t0.v[0] = ul;
			t0.v[1] = ur;
			t0.v[2] = ll;
			t1.v[0] = ll;
			t1.v[1] = ur;
			t1.v[2] = lr;
			out.push_back(t0);
			out.push_back(t1);

			if (fill_joins && i > 0 && (prev_nx != 0.0f || prev_ny != 0.0f)) {
				const float hw = ws[i];
				const CCPoint center {xs[i], ys[i]};
				const CCPoint prev_u {xs[i] + prev_nx * hw, ys[i] + prev_ny * hw};
				const CCPoint curr_u {xs[i] + nx * hw, ys[i] + ny * hw};
				const CCPoint prev_l {xs[i] - prev_nx * hw, ys[i] - prev_ny * hw};
				const CCPoint curr_l {xs[i] - nx * hw, ys[i] - ny * hw};

				gay::Triangle fu, fl;
				fu.v[0] = center;
				fu.v[1] = prev_u;
				fu.v[2] = curr_u;
				fl.v[0] = center;
				fl.v[1] = curr_l;
				fl.v[2] = prev_l;
				out.push_back(fu);
				out.push_back(fl);
			}

			prev_nx = nx;
			prev_ny = ny;
		}
	}

	void emit_outline_quads(
		Vec<gay::Triangle>& upper,
		Vec<gay::Triangle>& lower,
		const float* xs,
		const float* ys,
		const float* inner_ws,
		const float* outer_ws,
		size_t n,
		gay::JoinStyle join_style,
		float /*miter_limit*/
	) noexcept {
		if (n < 2) {
			return;
		}

		const bool fill_joins = join_style != gay::JoinStyle::None;
		const size_t join_reserve = fill_joins ? (n >= 2 ? n - 2 : 0) * 4 : size_t {0};
		upper.reserve(upper.size() + (n - 1) * 2 + join_reserve);
		lower.reserve(lower.size() + (n - 1) * 2 + join_reserve);

		float prev_nx = 0.0f, prev_ny = 0.0f;

		for (size_t i = 0; i + 1 < n; ++i) {
			float nx, ny;
			seg_normal(xs[i], ys[i], xs[i + 1], ys[i + 1], nx, ny);
			if (nx == 0.0f && ny == 0.0f) {
				prev_nx = nx;
				prev_ny = ny;
				continue;
			}

			const float iw0 = inner_ws[i], iw1 = inner_ws[i + 1];
			const float ow0 = outer_ws[i], ow1 = outer_ws[i + 1];
			const CCPoint iu0 {xs[i] + nx * iw0, ys[i] + ny * iw0};
			const CCPoint iu1 {xs[i + 1] + nx * iw1, ys[i + 1] + ny * iw1};
			const CCPoint il0 {xs[i] - nx * iw0, ys[i] - ny * iw0};
			const CCPoint il1 {xs[i + 1] - nx * iw1, ys[i + 1] - ny * iw1};
			const CCPoint ou0 {xs[i] + nx * ow0, ys[i] + ny * ow0};
			const CCPoint ou1 {xs[i + 1] + nx * ow1, ys[i + 1] + ny * ow1};
			const CCPoint ol0 {xs[i] - nx * ow0, ys[i] - ny * ow0};
			const CCPoint ol1 {xs[i + 1] - nx * ow1, ys[i + 1] - ny * ow1};

			gay::Triangle t0, t1;
			t0.v[0] = ou0;
			t0.v[1] = ou1;
			t0.v[2] = iu0;
			t1.v[0] = iu0;
			t1.v[1] = ou1;
			t1.v[2] = iu1;

			upper.push_back(t0);
			upper.push_back(t1);

			t0.v[0] = il0;
			t0.v[1] = il1;
			t0.v[2] = ol0;
			t1.v[0] = ol0;
			t1.v[1] = il1;
			t1.v[2] = ol1;

			lower.push_back(t0);
			lower.push_back(t1);

			if (fill_joins && i > 0 && (prev_nx != 0.0f || prev_ny != 0.0f)) {
				const float iw = inner_ws[i];
				const float ow = outer_ws[i];
				const CCPoint center {xs[i], ys[i]};
				const CCPoint prev_ou {xs[i] + prev_nx * ow, ys[i] + prev_ny * ow};
				const CCPoint curr_ou {xs[i] + nx * ow, ys[i] + ny * ow};
				const CCPoint prev_iu {xs[i] + prev_nx * iw, ys[i] + prev_ny * iw};
				const CCPoint curr_iu {xs[i] + nx * iw, ys[i] + ny * iw};

				gay::Triangle fo;
				fo.v[0] = center;
				fo.v[1] = prev_ou;
				fo.v[2] = curr_ou;

				upper.push_back(fo);

				fo.v[0] = center;
				fo.v[1] = prev_iu;
				fo.v[2] = curr_iu;

				upper.push_back(fo);

				const CCPoint prev_ol {xs[i] - prev_nx * ow, ys[i] - prev_ny * ow};
				const CCPoint curr_ol {xs[i] - nx * ow, ys[i] - ny * ow};
				const CCPoint prev_il {xs[i] - prev_nx * iw, ys[i] - prev_ny * iw};
				const CCPoint curr_il {xs[i] - nx * iw, ys[i] - ny * iw};

				gay::Triangle fl;

				fl.v[0] = center;
				fl.v[1] = curr_ol;
				fl.v[2] = prev_ol;

				lower.push_back(fl);
				fl.v[0] = center;
				fl.v[1] = curr_il;
				fl.v[2] = prev_il;

				lower.push_back(fl);
			}

			prev_nx = nx;
			prev_ny = ny;
		}
	}
} // namespace gay::geom
