#include <cmath>
#include <deque>
#include <vector>

#include <gay/color.hpp>
#include <gay/constants.hpp>
#include <gay/settings.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;
namespace color = gay::color;

static std::vector<CCPoint> compute_smooth_normals(const std::vector<CCPoint>& pts) {
	size_t n = pts.size();
	std::vector<CCPoint> normals(n, CCPointZero);

	if (n < 2) {
		return normals;
	}

	std::vector<CCPoint> edge_normals(n - 1);

	for (size_t i = 0; i < n - 1; i++) {
		CCPoint d = ccpSub(pts[i + 1], pts[i]);
		float len = ccpLength(d);
		edge_normals[i] = (len < 0.0001f) ? CCPointZero : ccp(-d.y / len, d.x / len);
	}

	normals[0] = edge_normals[0];

	for (size_t i = 1; i < n - 1; i++) {
		CCPoint avg = ccpAdd(edge_normals[i - 1], edge_normals[i]);
		float len = ccpLength(avg);
		normals[i] = (len < 0.0001f) ? edge_normals[i] : ccp(avg.x / len, avg.y / len);
	}

	normals[n - 1] = edge_normals[n - 2];

	for (size_t i = 1; i < n; i++) {
		if (ccpDot(normals[i], normals[i - 1]) < 0.0f) {
			normals[i] = ccp(-normals[i].x, -normals[i].y);
		}
	}

	return normals;
}

struct TrailRewriteHook: Modify<TrailRewriteHook, HardStreak> {
	struct Fields {
		std::vector<std::deque<CCPoint>> segments;
		CCPoint last_pos = CCPointZero;
		CCPoint seg_last_point = CCPointZero;
		float fade_time = 0.0f;
		float seg_accumulated = 0.0f;
		float seg_gap_distance = 0.0f;
		float current_speed = 0.0f;
		bool seg_in_gap = false;
		bool was_drawing = false;
	};

	bool is_wave_trail() {
		auto* pl = PlayLayer::get();

		if (!pl) {
			return false;
		}

		return (pl->m_player1 && pl->m_player1->m_waveTrail == this) ||
			(pl->m_player2 && pl->m_player2->m_waveTrail == this);
	}

	bool should_rewrite() {
		return settings::is_enabled() && settings::get<bool>("rewrite-trail") && is_wave_trail();
	}

	PlayerObject* get_owner_player() {
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

		this->m_currentPoint = CCPointApplyAffineTransform(
			point,
			CCAffineTransformInvert(this->nodeToParentTransform())
		);

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

		dt = std::min(dt, gay::MAX_DT);

		if (settings::check<settings::CheckMode::Any>("persist-trail", "segment-persist")) {
			this->m_drawStreak = true;
		}

		bool drawing = this->m_drawStreak;
		if (drawing && !m_fields->was_drawing) {
			m_fields->segments.push_back({});
			m_fields->seg_accumulated = 0.0f;
			m_fields->seg_in_gap = false;
			m_fields->seg_gap_distance = 0.0f;
			m_fields->seg_last_point = CCPointZero;
		}
		m_fields->was_drawing = drawing;

		if (drawing) {
			PlayerObject* player = get_owner_player();
			if (player && player->m_isDart) {
				CCPoint pos = this->m_currentPoint;

				if (settings::get<bool>("tilt-enabled")) {
					pos.x += settings::get_float("tilt-x-offset");
					pos.y += settings::get_float("tilt-y-offset");
				}

				if (dt > 0.0f) {
					float dist = ccpDistance(pos, m_fields->last_pos);
					float instant_speed = dist / dt;
					constexpr float ALPHA = 0.12f;
					m_fields->current_speed *= (1.0f - ALPHA) + instant_speed * ALPHA;
				}

				m_fields->last_pos = pos;

				if (settings::get<bool>("trail-segments")) {
					add_point_segmented(pos);
				} else {
					add_point_simple(pos);
				}
			}
		}

		cull_points();

		float opacity = calc_opacity(dt);
		float width = calc_width();

		this->clear();

		bool solid = settings::get<bool>("solid-trail");

		if (solid) {
			this->m_isSolid = true;
			this->setBlendFunc({GL_ONE, GL_ONE_MINUS_SRC_ALPHA});
		}

		this->setZOrder(-1);

		render_trail(width, opacity, solid);
	}

	void add_point_simple(CCPoint pos) {
		auto& segs = m_fields->segments;

		if (segs.empty()) {
			segs.push_back({});
		}

		auto& current = segs.back();

		if (current.empty() || ccpDistance(current.back(), pos) > 0.1f) {
			current.push_back(pos);
		}
	}

	void add_point_segmented(CCPoint pos) {
		auto& segs = m_fields->segments;
		float seg_length = settings::get_float("segment-length");
		float seg_gap = settings::get_float("segment-gap");

		if (segs.empty()) {
			segs.push_back({});
		}

		if (m_fields->seg_last_point.equals(CCPointZero)) {
			m_fields->seg_last_point = pos;
			segs.back().push_back(pos);
			return;
		}

		float remaining = ccpDistance(pos, m_fields->seg_last_point);
		m_fields->seg_last_point = pos;

		while (remaining > 0.0f) {
			if (m_fields->seg_in_gap) {
				float space = seg_gap - m_fields->seg_gap_distance;
				if (remaining < space) {
					m_fields->seg_gap_distance += remaining;
					remaining = 0.0f;
				} else {
					remaining -= space;
					m_fields->seg_in_gap = false;
					m_fields->seg_gap_distance = 0.0f;
					m_fields->seg_accumulated = 0.0f;
					segs.push_back({});
					segs.back().push_back(pos);
				}
			} else {
				float space = seg_length - m_fields->seg_accumulated;
				if (remaining < space) {
					m_fields->seg_accumulated += remaining;
					auto& current = segs.back();
					if (current.empty() || ccpDistance(current.back(), pos) > 0.1f) {
						current.push_back(pos);
					}
					remaining = 0.0f;
				} else {
					remaining -= space;
					m_fields->seg_accumulated = 0.0f;
					m_fields->seg_in_gap = true;
					m_fields->seg_gap_distance = 0.0f;
				}
			}
		}
	}

	void cull_points() {
		auto& segs = m_fields->segments;
		size_t total = 0;
		for (auto& seg : segs) {
			total += seg.size();
		}

		while (total > gay::MAX_TRAIL_POINTS && !segs.empty()) {
			if (segs.front().empty()) {
				segs.erase(segs.begin());
				continue;
			}
			segs.front().pop_front();
			total--;
			if (segs.front().empty()) {
				segs.erase(segs.begin());
			}
		}
	}

	float calc_opacity(float dt) {
		bool solid = settings::get<bool>("solid-trail");
		int base_opacity = settings::get<int>("trail-opacity");

		if (solid) {
			this->setOpacity(255);
			return 1.0f;
		}

		if (!settings::get<bool>("enable-trail-fade")) {
			this->setOpacity(static_cast<GLubyte>(base_opacity));
			return static_cast<float>(base_opacity) / 255.0f;
		}

		float fade_speed = settings::get_float("trail-fade-speed");
		int min_opacity = settings::get<int>("trail-min-opacity");

		m_fields->fade_time += dt * fade_speed;
		int faded = static_cast<int>(
			static_cast<float>(base_opacity) / (1.0f + m_fields->fade_time * 0.1f)
		);
		int target = std::max(min_opacity, faded);

		this->setOpacity(static_cast<GLubyte>(target));
		return static_cast<float>(target) / 255.0f;
	}

	float calc_width() {
		float wave_size = settings::get_float("trail-size");
		float pulse_size =
			settings::get<bool>("disable-pulse") ? 0.0f : settings::get_float("pulse-size");
		float base = wave_size + pulse_size;

		if (settings::get<bool>("add-min-width")) {
			base = std::max(base, 0.5f);
		}

		if (settings::get<bool>("trail-thickness-variation") &&
			!settings::get<bool>("disable-pulse")) {
			float multiplier = settings::get_float("thickness-multiplier");
			float speed_factor = std::clamp(m_fields->current_speed / 500.0f, 0.2f, 2.0f);
			base *= (1.0f + (speed_factor - 1.0f) * multiplier);
		}

		return base;
	}

	void render_trail(float width, float opacity, bool solid) {
		auto& segs = m_fields->segments;

		ccColor3B c = this->getColor();
		float brightness = settings::get_float("trail-brightness");
		if (brightness != 1.0f) {
			c = color::apply_brightness(c, brightness);
		}

		ccColor4F fill = {c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, opacity};

		if (settings::get<bool>("wave-outline")) {
			render_outline(width);
		}

		for (auto& seg : segs) {
			render_segment(seg, width, fill, solid);
		}
	}

	void render_segment(std::deque<CCPoint>& pts, float width, ccColor4F fill, bool solid) {
		size_t count = pts.size();
		if (count < 2) {
			return;
		}

		CCAffineTransform to_local = CCAffineTransformInvert(this->nodeToParentTransform());
		std::vector<CCPoint> local(count);

		for (size_t i = 0; i < count; i++) {
			local[i] = CCPointApplyAffineTransform(pts[i], to_local);
		}

		auto normals = compute_smooth_normals(local);

		for (size_t i = 0; i + 1 < count; i++) {
			float t0 = static_cast<float>(i) / static_cast<float>(count - 1);
			float t1 = static_cast<float>(i + 1) / static_cast<float>(count - 1);
			float w0 = width * t0, w1 = width * t1;

			CCPoint p0 = local[i], p1 = local[i + 1];
			CCPoint n0 = normals[i], n1 = normals[i + 1];
			CCPoint upper0 = ccp(p0.x + n0.x * w0, p0.y + n0.y * w0);
			CCPoint lower0 = ccp(p0.x - n0.x * w0, p0.y - n0.y * w0);
			CCPoint upper1 = ccp(p1.x + n1.x * w1, p1.y + n1.y * w1);
			CCPoint lower1 = ccp(p1.x - n1.x * w1, p1.y - n1.y * w1);

			ccColor4F quad_fill = fill;
			if (!solid) {
				quad_fill.a = fill.a * (t0 + t1) * 0.5f;
			}

			CCPoint verts[4] = {upper0, lower0, lower1, upper1};
			this->drawPolygon(verts, 4, quad_fill, 0.0f, {0, 0, 0, 0});
		}
	}

	void render_outline(float trail_width) {
		auto& segs = m_fields->segments;
		auto outline_width = static_cast<float>(settings::get<double>("wave-outline-width"));
		auto outline_colors = settings::get<gay::ColorList>("outline-colors");
		auto blur_layers = settings::get<double>("wave-outline-blur");
		auto outline_opacity_setting = settings::get<int>("wave-outline-opacity");

		float phase = color::g_phase;

		if (blur_layers == 0.0) {
			auto gc = color::get_gradient(phase, 0.0f, true, outline_colors);
			ccColor4F outline_color = {gc.r / 255.0f,
				gc.g / 255.0f,
				gc.b / 255.0f,
				static_cast<float>(outline_opacity_setting) / 255.0f};

			for (auto& seg : segs) {
				render_outline_edges(seg, trail_width, outline_width, outline_color);
			}
		} else {
			int blur_count = static_cast<int>(std::round(blur_layers));

			for (int layer = 0; layer < blur_count; layer++) {
				float layer_width = outline_width * (1.0f + static_cast<float>(layer) * 0.8f);
				float layer_opacity =
					std::max(0.05f, 0.8f * static_cast<float>(std::pow(0.7, layer)));
				float phase_offset = static_cast<float>(layer) * 5.0f;

				auto gc = color::get_gradient(phase, phase_offset, true, outline_colors);
				ccColor4F glow = {gc.r / 255.0f,
					gc.g / 255.0f,
					gc.b / 255.0f,
					(static_cast<float>(outline_opacity_setting) / 255.0f) * layer_opacity};

				for (auto& seg : segs) {
					render_outline_edges(seg, trail_width, layer_width, glow);
				}
			}
		}
	}

	void render_outline_edges(
		std::deque<CCPoint>& pts,
		float trail_width,
		float outline_width,
		ccColor4F outline_color
	) {
		size_t count = pts.size();
		if (count < 2) {
			return;
		}

		CCAffineTransform to_local = CCAffineTransformInvert(this->nodeToParentTransform());
		std::vector<CCPoint> local(count);
		for (size_t i = 0; i < count; i++) {
			local[i] = CCPointApplyAffineTransform(pts[i], to_local);
		}

		auto normals = compute_smooth_normals(local);

		float outline_offset = outline_width + outline_width / static_cast<float>(count);

		for (size_t i = 0; i + 1 < count; i++) {
			float t0 = static_cast<float>(i) / static_cast<float>(count - 1);
			float t1 = static_cast<float>(i + 1) / static_cast<float>(count - 1);

			float w0 = trail_width * t0 + outline_offset;
			float w1 = trail_width * t1 + outline_offset;

			CCPoint p0 = local[i], p1 = local[i + 1];
			CCPoint n0 = normals[i], n1 = normals[i + 1];

			CCPoint top0 = ccp(p0.x + n0.x * w0, p0.y + n0.y * w0);
			CCPoint top1 = ccp(p1.x + n1.x * w1, p1.y + n1.y * w1);
			this->drawSegment(top0, top1, outline_width, outline_color);

			CCPoint bot0 = ccp(p0.x - n0.x * w0, p0.y - n0.y * w0);
			CCPoint bot1 = ccp(p1.x - n1.x * w1, p1.y - n1.y * w1);
			this->drawSegment(bot0, bot1, outline_width, outline_color);
		}
	}
};

struct TrailRewriteResetHook: Modify<TrailRewriteResetHook, PlayLayer> {
	void resetLevel() {
		PlayLayer::resetLevel();

		if (!settings::is_enabled() || !settings::get<bool>("rewrite-trail")) {
			return;
		}

		auto clear_trail = [](PlayerObject* player) {
			if (!player || !player->m_waveTrail) {
				return;
			}
			auto* hook = static_cast<TrailRewriteHook*>(player->m_waveTrail);
			hook->m_fields->segments.clear();
			hook->m_fields->fade_time = 0.0f;
			hook->m_fields->seg_accumulated = 0.0f;
			hook->m_fields->seg_last_point = CCPointZero;
			hook->m_fields->seg_in_gap = false;
			hook->m_fields->seg_gap_distance = 0.0f;
			hook->m_fields->last_pos = CCPointZero;
			hook->m_fields->current_speed = 0.0f;
			hook->m_fields->was_drawing = false;
		};

		clear_trail(this->m_player1);
		clear_trail(this->m_player2);
	}

	void onQuit() {
		if (settings::is_enabled() && settings::get<bool>("rewrite-trail")) {
			auto clear_trail = [](PlayerObject* player) {
				if (!player || !player->m_waveTrail) {
					return;
				}
				auto* hook = static_cast<TrailRewriteHook*>(player->m_waveTrail);
				hook->m_fields->segments.clear();
			};

			clear_trail(this->m_player1);
			clear_trail(this->m_player2);
		}

		PlayLayer::onQuit();
	}
};
