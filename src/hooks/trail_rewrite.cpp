#include <atomic>
#include <cstring>

#include <gay/color.hpp>
#include <gay/constants.hpp>
#include <gay/settings.hpp>
#include <gay/utils.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;
namespace color = gay::color;

static constexpr size_t RING_CAP = 1u << 12;
static constexpr size_t RING_MASK = RING_CAP - 1;
static constexpr float MITER_LIMIT = 4.0f;
static constexpr float MIN_DIST = 0.1f;

struct alignas(16) TrailPt {
	float x;
	float y;
	float t;
	uint32_t seg;
};

struct Triangle {
	CCPoint v[3];
};

struct FrameGeo {
	Vec<Triangle> fill;
	Vec<Triangle> outline_upper;
	Vec<Triangle> outline_lower;
};

struct alignas(64) TrailRing {
	struct alignas(64) {
		Atomic<size_t> v {0};
	} _head;

	struct alignas(64) {
		Atomic<size_t> v {0};
	} _tail;

	TrailPt buf[RING_CAP];

	size_t head() const noexcept {
		return _head.v.load(std::memory_order_acquire);
	}

	size_t tail() const noexcept {
		return _tail.v.load(std::memory_order_acquire);
	}

	void push(const TrailPt& p) noexcept {
		size_t t = _tail.v.load(std::memory_order_relaxed);
		buf[t & RING_MASK] = p;
		_tail.v.store(t + 1, std::memory_order_release);

		size_t h = _head.v.load(std::memory_order_relaxed);

		if ((t + 1) - h > RING_CAP) {
			_head.v.store(h + 1, std::memory_order_relaxed);
		}
	}

	void cull_before(float min_t) noexcept {
		size_t h = _head.v.load(std::memory_order_relaxed);
		size_t t = _tail.v.load(std::memory_order_acquire);

		while (h < t && buf[h & RING_MASK].t < min_t) {
			++h;
		}

		_head.v.store(h, std::memory_order_release);
	}

	void clear() noexcept {
		_head.v.store(_tail.v.load(std::memory_order_relaxed), std::memory_order_release);
	}

	bool empty() const noexcept {
		return _head.v.load(std::memory_order_relaxed) == _tail.v.load(std::memory_order_relaxed);
	}

	size_t snapshot(TrailPt* out, size_t cap) const noexcept {
		size_t h = _head.v.load(std::memory_order_acquire);
		size_t t = _tail.v.load(std::memory_order_acquire);
		size_t n = std::min(t - h, cap);

		for (size_t i = 0; i < n; ++i) {
			out[i] = buf[(h + i) & RING_MASK];
		}

		return n;
	}
};

namespace geom {
	static inline float len(float x, float y) noexcept {
		return std::sqrt(x * x + y * y);
	}

	static void miter(
		const float* xs,
		const float* ys,
		size_t i,
		size_t n,
		float half_w,
		float& ox,
		float& oy
	) noexcept {
		float nx1 = 0;
		float ny1 = 0;

		if (i > 0) {
			float dx = xs[i] - xs[i - 1];
			float dy = ys[i] - ys[i - 1];
			float ln = len(dx, dy);

			if (ln > 1e-6f) {
				nx1 = -dy / ln;
				ny1 = dx / ln;
			}
		}

		float nx2 = 0;
		float ny2 = 0;

		if (i + 1 < n) {
			float dx = xs[i + 1] - xs[i];
			float dy = ys[i + 1] - ys[i];
			float ln = len(dx, dy);
		}

		if (i == 0) {
			ox = nx2 * half_w;
			oy = ny2 * half_w;
			return;
		}

		if (i + 1 == n) {
			ox = nx1 * half_w;
			oy = ny1 * half_w;
			return;
		}

		float bx = nx1 + nx2;
		float by = ny1 + ny2;
		float bl = len(bx, by);

		if (bl < 1e-4f) {
			ox = nx1 * half_w;
			oy = ny1 * half_w;
			return;
		}

		bx /= bl;
		by /= bl;

		float cs = bx * nx1 + by * ny1;

		if (cs < 1.0f / MITER_LIMIT) {
			ox = nx1 * half_w;
			oy = ny1 * half_w;
			return;
		}

		float scale = half_w / cs;

		ox = bx * scale;
		oy = by * scale;
	}

	static void build_offset(
		const float* xs,
		const float* ys,
		size_t n,
		float half_w,
		float* ux,
		float* uy,
		float* lx,
		float* ly
	) noexcept {
		for (size_t i = 0; i < n; ++i) {
			float ox;
			float oy;

			miter(xs, ys, i, n, half_w, ox, oy);

			ux[i] = xs[i] + ox;
			uy[i] = ys[i] + oy;
			lx[i] = xs[i] - ox;
			ly[i] = ys[i] - oy;
		}
	}

	static void emit_strip(
		Vec<Triangle>& out,
		const float* ax,
		const float* ay,
		const float* bx,
		const float* by,
		size_t n
	) noexcept {
		if (n < 2) {
			return;
		}

		out.reserve(out.size() + (n - 1) * 2);

		for (size_t i = 0; i + 1 < n; ++i) {
			if (ax[i] == ax[i + 1] && ay[i] == ay[i + 1]) {
				continue;
			}

			Triangle t0;
			Triangle t1;

			t0.v[0] = CCPoint {ax[i], ay[i]};
			t0.v[1] = CCPoint {ax[i + 1], ay[i + 1]};
			t0.v[2] = CCPoint {bx[i], by[i]};
			t1.v[0] = CCPoint {bx[i], by[i]};
			t1.v[1] = CCPoint {ax[i + 1], ay[i + 1]};
			t1.v[2] = CCPoint {bx[i + 1], by[i + 1]};

			out.push_back(t0);
			out.push_back(t1);
		}
	}
} // namespace geom

struct WorkerParams {
	float trail_width = 1.0f;
	float outline_width = 0.0f;
	bool do_outline = false;
};

struct TrailWorker {
	Mutex snap_mutex;
	Vec<TrailPt> snap_pts;
	size_t snap_count = 0;
	WorkerParams snap_params {};
	Atomic<bool> snap_fresh {false};
	FrameGeo geo[2];
	Atomic<int> front_idx {0};
	Atomic<bool> geo_ready {false};
	Thread thread;
	CondVar cv;
	Mutex cv_mu;
	Atomic<bool> alive {true};

	TrailWorker() {
		snap_pts.reserve(RING_CAP);

		for (auto& g : geo) {
			g.fill.reserve(RING_CAP * 2);
			g.outline_upper.reserve(RING_CAP);
			g.outline_lower.reserve(RING_CAP);
		}

		thread = Thread([this] { worker_loop(); });
	}

	~TrailWorker() {
		alive.store(false, std::memory_order_relaxed);
		cv.notify_all();

		if (thread.joinable()) {
			thread.join();
		}
	}

	void submit(const TrailPt* pts, size_t count, const WorkerParams& p) {
		{
			LockGuard<Mutex> lk(snap_mutex);
			snap_pts.resize(count);

			if (count) {
				std::memcpy(snap_pts.data(), pts, count * sizeof(TrailPt));
			}

			snap_count = count;
			snap_params = p;
			snap_fresh.store(true, std::memory_order_release);
		}

		cv.notify_one();
	}

	const FrameGeo* get_frame() const noexcept {
		if (!geo_ready.load(std::memory_order_acquire)) {
			return nullptr;
		}

		return &geo[front_idx.load(std::memory_order_relaxed)];
	}

  private:
	void worker_loop() {
		Vec<TrailPt> local_pts;
		local_pts.reserve(RING_CAP);

		WorkerParams local_p {};

		while (alive.load(std::memory_order_relaxed)) {
			{
				UniqueLock<Mutex> lk(cv_mu);

				cv.wait(lk, [this] {
					return !alive.load(std::memory_order_relaxed) ||
						snap_fresh.load(std::memory_order_relaxed);
				});
			}

			if (!alive.load(std::memory_order_relaxed)) {
				break;
			}

			size_t count;

			{
				LockGuard<Mutex> lk(snap_mutex);
				count = snap_count;
				local_pts.resize(count);

				if (count) {
					std::memcpy(local_pts.data(), snap_pts.data(), count * sizeof(TrailPt));
				}

				local_p = snap_params;
				snap_fresh.store(false, std::memory_order_relaxed);
			}

			int back = 1 - front_idx.load(std::memory_order_relaxed);

			build_geometry(local_pts.data(), count, local_p, geo[back]);

			front_idx.store(back, std::memory_order_release);
			geo_ready.store(true, std::memory_order_release);
		}
	}

	void build_geometry(const TrailPt* pts, size_t total, const WorkerParams& p, FrameGeo& out) {
		out.fill.clear();
		out.outline_upper.clear();
		out.outline_lower.clear();

		if (total < 2) {
			return;
		}

		thread_local Vec<float> xs;
		thread_local Vec<float> ys;
		thread_local Vec<float> ux;
		thread_local Vec<float> uy;
		thread_local Vec<float> lx;
		thread_local Vec<float> ly;
		thread_local Vec<float> ox;
		thread_local Vec<float> oy;
		thread_local Vec<float> ix;
		thread_local Vec<float> iy;

		const float hw = p.trail_width;
		const float ohw = p.trail_width + p.outline_width;

		size_t seg_start = 0;
		uint32_t curr_seg = pts[0].seg;

		auto flush = [&](size_t start, size_t end) {
			size_t n = end - start;

			if (n < 2) {
				return;
			}

			xs.resize(n);
			ys.resize(n);

			for (size_t k = 0; k < n; ++k) {
				xs[k] = pts[start + k].x;
				ys[k] = pts[start + k].y;
			}

			ux.resize(n);
			uy.resize(n);
			lx.resize(n);
			ly.resize(n);

			geom::build_offset(
				xs.data(),
				ys.data(),
				n,
				hw,
				ux.data(),
				uy.data(),
				lx.data(),
				ly.data()
			);

			geom::emit_strip(out.fill, ux.data(), uy.data(), lx.data(), ly.data(), n);

			if (p.do_outline && p.outline_width > 0.0f) {
				ox.resize(n);
				oy.resize(n);
				ix.resize(n);
				iy.resize(n);

				geom::build_offset(
					xs.data(),
					ys.data(),
					n,
					ohw,
					ox.data(),
					oy.data(),
					ix.data(),
					iy.data()
				);

				geom::emit_strip(out.outline_upper, ox.data(), oy.data(), ux.data(), uy.data(), n);
				geom::emit_strip(out.outline_lower, lx.data(), ly.data(), ix.data(), iy.data(), n);
			}
		};

		for (size_t i = 0; i <= total; ++i) {
			bool boundary = (i == total) || (pts[i].seg != curr_seg);

			if (!boundary) {
				continue;
			}

			flush(seg_start, i);

			if (i < total) {
				seg_start = i;
				curr_seg = pts[i].seg;
			}
		}
	}
};

class $modify(TrailRewriteHook, HardStreak) {
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
					constexpr float ALPHA = 0.12f;
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

		float ttl = to(float, settings::get<double>("trail-length"));

		if (ttl <= 0.0f) {
			ttl = 2.5f;
		}

		f.ring.cull_before(f.game_time - ttl);

		const float opacity = calc_opacity(dt);
		const float width = calc_width();
		const bool do_ol = settings::get<bool>("wave-outline");
		const float ol_w = do_ol ? to(float, settings::get<double>("wave-outline-width")) : 0.0f;
		const size_t cnt = f.ring.snapshot(f.snap_scratch.data(), RING_CAP);

		WorkerParams wp;
		wp.trail_width = width;
		wp.outline_width = ol_w;
		wp.do_outline = do_ol;
		f.worker.submit(f.snap_scratch.data(), cnt, wp);

		this->clear();

		if (settings::get<bool>("solid-trail")) {
			this->m_isSolid = true;
			this->setBlendFunc({GL_ONE, GL_ONE_MINUS_SRC_ALPHA});
			this->setOpacity(255);
		}

		this->setZOrder(-1);

		draw_frame(opacity, do_ol);
	}

	void push_simple(CCPoint pos) {
		auto& f = F();
		const float dx = pos.x - f.simple_last_x;
		const float dy = pos.y - f.simple_last_y;

		if ((f.simple_last_x != 0.0f || f.simple_last_y != 0.0f) &&
			std::sqrt(dx * dx + dy * dy) < MIN_DIST) {
			return;
		}

		f.simple_last_x = pos.x;
		f.simple_last_y = pos.y;
		f.ring.push({pos.x, pos.y, f.game_time, f.seg_id});
	}

	void push_segmented(CCPoint pos) {
		auto& f = F();
		const float seg_length = settings::get_float("segment-length");
		const float seg_gap = settings::get_float("segment-gap");

		if (f.seg_last_x == 0.0f && f.seg_last_y == 0.0f) {
			f.seg_last_x = pos.x;
			f.seg_last_y = pos.y;
			f.ring.push({pos.x, pos.y, f.game_time, f.seg_id});
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
					f.ring.push({pos.x, pos.y, f.game_time, f.seg_id});
				}
			} else {
				float space = seg_length - f.seg_accumulated;
				if (remaining < space) {
					f.seg_accumulated += remaining;
					f.ring.push({pos.x, pos.y, f.game_time, f.seg_id});
					remaining = 0.0f;
				} else {
					remaining -= space;
					f.seg_accumulated = 0.0f;
					f.seg_in_gap = true;
					f.seg_gap_dist = 0.0f;
					f.ring.push({pos.x, pos.y, f.game_time, f.seg_id});
				}
			}
		}
	}

	float calc_opacity(float dt) {
		const bool solid = settings::get<bool>("solid-trail");
		const int base = settings::get<int>("trail-opacity");
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
		const int min_op = settings::get<int>("trail-min-opacity");
		f.fade_time += dt * speed;

		const int faded = to(int, base / (1.0f + f.fade_time * 0.1f));
		const int target = std::max(min_op, faded);
		this->setOpacity(to(GLubyte, target));
		return to(float, target) / 255.0f;
	}

	float calc_width() {
		float base = settings::get_float("trail-size") +
			(settings::get<bool>("disable-pulse") ? 0.0f : settings::get_float("pulse-size"));

		if (settings::get<bool>("add-min-width")) {
			base = std::max(base, 0.5f);
		}

		if (settings::get<bool>("trail-thickness-variation") &&
			!settings::get<bool>("disable-pulse")) {
			const float mul = settings::get_float("thickness-multiplier");
			const float factor = std::clamp(F().current_speed / 500.0f, 0.2f, 2.0f);
			base *= (1.0f + (factor - 1.0f) * mul);
		}
		return base;
	}

	void draw_frame(float opacity, bool do_outline) {
		const FrameGeo* geo = F().worker.get_frame();
		if (!geo) {
			return;
		}

		ccColor3B c = this->getColor();
		const float br = settings::get_float("trail-brightness");
		if (br != 1.0f) {
			c = color::apply_brightness(c, br);
		}

		const ccColor4F fill_col = {c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, opacity};

		for (const auto& tri : geo->fill) {
			CCPoint pts[3] = {tri.v[0], tri.v[1], tri.v[2]};
			this->drawPolygon(pts, 3, fill_col, 0.0f, {0, 0, 0, 0});
		}

		if (do_outline && (!geo->outline_upper.empty() || !geo->outline_lower.empty())) {
			const float phase = color::g_phase;
			const auto blur = settings::get<double>("wave-outline-blur");
			const float oa = to(float, settings::get<int>("wave-outline-opacity")) / 255.0f;
			const auto colors = settings::get<gay::ColorList>("outline-colors");
			int layers = (blur == 0.0) ? 1 : to(int, std::round(blur));

			for (int layer = 0; layer < layers; ++layer) {
				const float phase_off = to(float, layer) * 5.0f;
				const auto gc = color::get_gradient(phase, phase_off, true, colors);
				float layer_a = oa;

				if (layers > 1) {
					layer_a *= std::max(0.05f, 0.8f * to(float, std::pow(0.7, layer)));
				}

				const ccColor4F oc = {gc.r / 255.0f, gc.g / 255.0f, gc.b / 255.0f, layer_a};

				for (const auto& tri : geo->outline_upper) {
					CCPoint pts[3] = {tri.v[0], tri.v[1], tri.v[2]};
					this->drawPolygon(pts, 3, oc, 0.0f, {0, 0, 0, 0});
				}

				for (const auto& tri : geo->outline_lower) {
					CCPoint pts[3] = {tri.v[0], tri.v[1], tri.v[2]};
					this->drawPolygon(pts, 3, oc, 0.0f, {0, 0, 0, 0});
				}
			}
		}
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
