#include <gay/color.hpp>
#include <gay/constants.hpp>
#include <gay/settings.hpp>
#include <gay/types.hpp>
#include <gay/utils.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace gay;
namespace settings = gay::settings;
namespace color = gay::color;

struct TrailRewriteHook: Modify<TrailRewriteHook, HardStreak> {
	struct Fields {
		TrailRing ring;
		TrailWorker worker;
		Array<TrailPt, RING_CAP> snap_scratch;
		CCPoint last_vel_pos = CCPointZero;

		uint32_t seg_id = 0;

		float game_time = 0.0f;
		float fade_time = 0.0f;
		float current_speed = 0.0f;
		float simple_last_x = 0.0f;
		float simple_last_y = 0.0f;
		float seg_accumulated = 0.0f;
		float seg_gap_dist = 0.0f;
		float seg_last_x = 0.0f;
		float seg_last_y = 0.0f;

		bool seg_in_gap = false;
		bool was_drawing = false;
	};

	Fields& F() {
		return *m_fields.operator->();
	}

	bool is_wave_trail() const {
		auto* pl = PlayLayer::get();

		if (!pl) {
			return false;
		}

		return (pl->m_player1 && pl->m_player1->m_waveTrail == this) ||
			(pl->m_player2 && pl->m_player2->m_waveTrail == this);
	}

	bool should_rewrite() const {
		return settings::is_enabled() && settings::get<bool>("rewrite-trail") && is_wave_trail();
	}

	PlayerObject* get_owner() const {
		auto* pl = PlayLayer::get();

		if (!pl) {
			return nullptr;
		}

		if (pl->m_player1 && pl->m_player1->m_waveTrail == this) {
			return pl->m_player1;
		}

		if (pl->m_player2 && pl->m_player2->m_waveTrail == this) {
			return pl->m_player2;
		}

		return nullptr;
	}

	void addPoint(CCPoint point) {
		if (!should_rewrite()) {
			HardStreak::addPoint(point);
			return;
		}

		this->m_currentPoint = point;
		this->m_drawStreak = true;
	}

	void stopStroke() {
		if (!should_rewrite()) {
			HardStreak::stopStroke();
			return;
		}

		if (settings::check<settings::CheckMode::Any>("persist-trail", "segment-persist")) {
			return;
		}

		this->m_drawStreak = false;
	}

	void updateStroke(float dt) {
		if (!should_rewrite()) {
			HardStreak::updateStroke(dt);
			return;
		}

		HardStreak::updateStroke(dt);
		this->clear();

		dt = std::min(dt, gay::MAX_DT);

		auto& f = F();
		f.game_time += dt;

		if (settings::check<settings::CheckMode::Any>("persist-trail", "segment-persist")) {
			this->m_drawStreak = true;
		}

		const bool drawing = this->m_drawStreak;

		if (drawing && !f.was_drawing) {
			f.seg_id++;
			f.seg_accumulated = 0.0f;
			f.seg_gap_dist = 0.0f;
			f.seg_in_gap = false;
			f.seg_last_x = 0.0f;
			f.seg_last_y = 0.0f;
			f.simple_last_x = 0.0f;
			f.simple_last_y = 0.0f;
			f.fade_time = 0.0f;
		}

		f.was_drawing = drawing;

		if (drawing) {
			PlayerObject* player = get_owner();

			if (player && player->m_isDart) {
				CCPoint pos = this->m_currentPoint;

				if (settings::get<bool>("tilt-enabled")) {
					pos.x += settings::get_float("tilt-x-offset");
					pos.y += settings::get_float("tilt-y-offset");
				}

				if (dt > 0.0f) {
					float dist = ccpDistance(pos, f.last_vel_pos);
					const float ALPHA = settings::get_float("speed-smoothing");
					f.current_speed = f.current_speed * (1.0f - ALPHA) + (dist / dt) * ALPHA;
				}

				f.last_vel_pos = pos;

				if (settings::get<bool>("trail-segments")) {
					push_segmented(pos);
				} else {
					push_simple(pos);
				}
			}
		}

		float ttl = static_cast<float>(settings::get<double>("trail-length"));

		if (ttl <= 0.0f) {
			ttl = 2.5f;
		}

		f.ring.cull_before(f.game_time - ttl);

		const float opacity = calc_opacity(dt);
		const float width = calc_base_width();
		const float p_add = calc_pulse_add();
		const bool do_ol = settings::get<bool>("wave-outline");
		const float ol_w = do_ol ? static_cast<float>(settings::get<double>("wave-outline-width")) : 0.0f;
		const size_t cnt = f.ring.snapshot(f.snap_scratch.data(), RING_CAP);

		WorkerParams wp;
		wp.trail_width = width;
		wp.outline_width = ol_w;
		wp.pulse_add = p_add;
		wp.miter_limit = settings::get_float("miter-limit");
		wp.do_outline = do_ol;
		wp.join_style = to(gay::JoinStyle, settings::get<int64_t>("join-style"));
		f.worker.submit(f.snap_scratch.data(), cnt, wp);

		this->clear();

		if (settings::get<bool>("solid-trail")) {
			this->m_isSolid = true;
			this->setBlendFunc({GL_ONE, GL_ONE_MINUS_SRC_ALPHA});
			this->setOpacity(255);
		}

		this->setZOrder(static_cast<int>(settings::get<int64_t>("trail-z-order")));

		draw_frame(opacity, do_ol);
	}

	void push_simple(CCPoint pos) {
		auto& f = F();
		const float dx = pos.x - f.simple_last_x;
		const float dy = pos.y - f.simple_last_y;

		const float min_dist = settings::get_float("min-point-distance");
		if ((f.simple_last_x != 0.0f || f.simple_last_y != 0.0f) && std::sqrt(dx * dx + dy * dy) < min_dist) {
			return;
		}

		f.simple_last_x = pos.x;
		f.simple_last_y = pos.y;
		f.ring.push({pos.x, pos.y, f.game_time, calc_base_width(), f.seg_id, 0});
	}

	void push_segmented(CCPoint pos) {
		auto& f = F();
		const float seg_length = settings::get_float("segment-length");
		const float seg_gap = settings::get_float("segment-gap");

		if (f.seg_last_x == 0.0f && f.seg_last_y == 0.0f) {
			f.seg_last_x = pos.x;
			f.seg_last_y = pos.y;
			f.ring.push({pos.x, pos.y, f.game_time, calc_base_width(), f.seg_id, 0});
			return;
		}

		const float dx = pos.x - f.seg_last_x;
		const float dy = pos.y - f.seg_last_y;
		float remaining = std::sqrt(dx * dx + dy * dy);
		if (remaining < 1e-6f) {
			return;
		}

		f.seg_last_x = pos.x;
		f.seg_last_y = pos.y;

		const float w = calc_base_width();

		while (remaining > 0.0f) {
			if (f.seg_in_gap) {
				float space = seg_gap - f.seg_gap_dist;
				if (remaining < space) {
					f.seg_gap_dist += remaining;
					remaining = 0.0f;
				} else {
					remaining -= space;
					f.seg_in_gap = false;
					f.seg_gap_dist = 0.0f;
					f.seg_accumulated = 0.0f;
					f.seg_id++;
					f.ring.push({pos.x, pos.y, f.game_time, w, f.seg_id, 0});
				}
			} else {
				float space = seg_length - f.seg_accumulated;
				if (remaining < space) {
					f.seg_accumulated += remaining;
					f.ring.push({pos.x, pos.y, f.game_time, w, f.seg_id, 0});
					remaining = 0.0f;
				} else {
					remaining -= space;
					f.seg_accumulated = 0.0f;
					f.seg_in_gap = true;
					f.seg_gap_dist = 0.0f;
					f.ring.push({pos.x, pos.y, f.game_time, w, f.seg_id, 0});
				}
			}
		}
	}

	float calc_opacity(float dt) {
		const bool solid = settings::get<bool>("solid-trail");
		const int32_t base = static_cast<int32_t>(settings::get<int64_t>("trail-opacity"));
		auto& f = F();

		if (solid) {
			this->setOpacity(255);
			return 1.0f;
		}

		if (!settings::get<bool>("enable-trail-fade")) {
			this->setOpacity(to(GLubyte, base));
			return to(float, base) / 255.0f;
		}

		const float speed = settings::get_float("trail-fade-speed");
		const int32_t min_op = static_cast<int32_t>(settings::get<int64_t>("trail-min-opacity"));
		f.fade_time += dt * speed;

		const float fade_curve = settings::get_float("fade-curve-factor");
		const int32_t faded = to(int32_t, base / (1.0f + f.fade_time * fade_curve));
		const int32_t target = std::max(min_op, faded);
		this->setOpacity(to(GLubyte, target));
		return to(float, target) / 255.0f;
	}

	float calc_base_width() {
		float base = this->m_waveSize * settings::get_float("trail-size");

		if (settings::get<bool>("add-min-width")) {
			base = std::max(base, 0.5f);
		}

		if (settings::get<bool>("trail-thickness-variation") && !settings::get<bool>("disable-pulse")) {
			const float mul = settings::get_float("thickness-multiplier");
			const float speed_ref = settings::get_float("speed-reference");
			const float clamp_min = settings::get_float("speed-clamp-min");
			const float clamp_max = settings::get_float("speed-clamp-max");
			const float factor = std::clamp(F().current_speed / speed_ref, clamp_min, clamp_max);
			base *= (1.0f + (factor - 1.0f) * mul);
		}

		return base;
	}

	float calc_pulse_add() {
		if (settings::get<bool>("disable-pulse")) {
			return 0.0f;
		}

		return this->m_pulseSize * settings::get_float("pulse-size");
	}

	void reserve_buff(size_t needed) {
		if (needed <= m_uBufferCapacity) {
			return;
		}

		size_t cap = std::max(needed, size_t {64});
		cap--;
		cap |= cap >> 1;
		cap |= cap >> 2;
		cap |= cap >> 4;
		cap |= cap >> 8;
		cap |= cap >> 16;
		cap |= cap >> 32;
		cap++;

		m_pBuffer = static_cast<ccV2F_C4B_T2F*>(malloc(cap * sizeof(ccV2F_C4B_T2F)));
		m_uBufferCapacity = cap;
	}

	void draw_frame(float opacity, bool do_outline) {
		const FrameGeo* geo = F().worker.get_frame();
		if (!geo) {
			return;
		}

		int32_t layers = 1;
		if (do_outline && (!geo->outline_upper.empty() || !geo->outline_lower.empty())) {
			const auto blur = settings::get<int64_t>("wave-outline-blur");
			layers = (blur == 0) ? 1 : to(int32_t, blur);
		}

		const size_t outline_tris =
			do_outline ? size_t(layers) * (geo->outline_upper.size() + geo->outline_lower.size()) : 0u;

		reserve_buff((geo->fill.size() + outline_tris) * 3);

		const auto inv = CCAffineTransformInvert(this->nodeToParentTransform());
		const float ma = inv.a, mb = inv.b, mc = inv.c, md = inv.d, mtx = inv.tx, mty = inv.ty;
		ccColor3B c = this->getColor();
		const float br = settings::get_float("trail-brightness");

		if (br != 1.0f) {
			c = color::apply_brightness(c, br);
		}

		const ccColor4B fill_col = {c.r, c.g, c.b, static_cast<GLubyte>(opacity * 255.0f)};
		ccV2F_C4B_T2F* dst = m_pBuffer;

		auto write_tri = [&](const Triangle& tri, ccColor4B color) {
			for (int32_t k = 0; k < 3; ++k) {
				const float px = tri.v[k].x, py = tri.v[k].y;
				dst->vertices = {ma * px + mc * py + mtx, mb * px + md * py + mty};
				dst->colors = color;
				dst->texCoords = {0.f, 0.f};
				++dst;
			}
		};

		for (const auto& tri : geo->fill) {
			write_tri(tri, fill_col);
		}

		if (outline_tris > 0) {
			const float phase = color::g_phase;
			const float oa = to(float, settings::get<int64_t>("wave-outline-opacity")) / 255.0f;
			const auto colors = settings::get<gay::ColorList>("outline-colors");

			const float blur_decay = settings::get_float("outline-blur-decay");
			const float color_spread = settings::get_float("outline-color-spread");

			for (int32_t layer = 0; layer < layers; ++layer) {
				const auto gc = color::get_gradient(phase, static_cast<float>(layer) * color_spread, true, colors);
				float layer_a = oa;

				if (layers > 1) {
					layer_a *= std::max(0.05f, 0.8f * static_cast<float>(std::pow(blur_decay, layer)));
				}

				const ccColor4B oc = {gc.r, gc.g, gc.b, static_cast<GLubyte>(layer_a * 255.0f)};

				for (const auto& tri : geo->outline_upper) {
					write_tri(tri, oc);
				}

				for (const auto& tri : geo->outline_lower) {
					write_tri(tri, oc);
				}
			}
		}

		m_nBufferCount = static_cast<unsigned int>(dst - m_pBuffer);
		m_bDirty = true;
	}

	void reset_state() {
		auto& f = F();
		f.ring.clear();
		f.game_time = 0.0f;
		f.fade_time = 0.0f;
		f.current_speed = 0.0f;
		f.last_vel_pos = CCPointZero;
		f.simple_last_x = 0.0f;
		f.simple_last_y = 0.0f;
		f.seg_id = 0;
		f.seg_accumulated = 0.0f;
		f.seg_gap_dist = 0.0f;
		f.seg_in_gap = false;
		f.seg_last_x = 0.0f;
		f.seg_last_y = 0.0f;
		f.was_drawing = false;
	}
};

class $modify(TrailRewriteResetHook, PlayLayer) {
	void resetLevel() {
		PlayLayer::resetLevel();

		if (!settings::is_enabled() || !settings::get<bool>("rewrite-trail")) {
			return;
		}

		for_each_trail([](TrailRewriteHook* h) { h->reset_state(); });
	}

	void onQuit() {
		if (settings::is_enabled() && settings::get<bool>("rewrite-trail")) {
			for_each_trail([](TrailRewriteHook* h) { h->m_fields->ring.clear(); });
		}

		PlayLayer::onQuit();
	}

  private:
	template<typename F>
	void for_each_trail(F fn) {
		auto visit = [&](PlayerObject* p) {
			if (!p || !p->m_waveTrail) {
				return;
			}

			fn(to(TrailRewriteHook*, p->m_waveTrail));
		};

		visit(this->m_player1);
		visit(this->m_player2);
	}
};
