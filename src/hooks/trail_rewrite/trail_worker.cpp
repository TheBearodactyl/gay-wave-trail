#include <atomic>

#include <gay/geometry.hpp>
#include <gay/types.hpp>
#include <gay/utils.hpp>

using namespace gay;

namespace gay {
	void TrailWorker::submit(const TrailPt* pts, size_t count, const WorkerParams& p) {
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

	const FrameGeo* TrailWorker::get_frame() const noexcept {
		if (!geo_ready.load(std::memory_order_acquire)) {
			return nullptr;
		}

		return &geo[front_idx.load(std::memory_order_relaxed)];
	}

	void TrailWorker::worker_loop() {
		Vec<TrailPt> local_pts;
		local_pts.reserve(RING_CAP);

		WorkerParams local_p {};

		while (alive.load(std::memory_order_relaxed)) {
			{
				UniqueLock<Mutex> lk(cv_mutex);

				cv.wait(lk, [this] {
					return !alive.load(std::memory_order_relaxed) || snap_fresh.load(std::memory_order_relaxed);
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

			let back = 1 - front_idx.load(std::memory_order_relaxed);

			build_geometry(local_pts.data(), count, local_p, geo[back]);

			front_idx.store(back, std::memory_order_release);
			geo_ready.store(true, std::memory_order_release);
		}
	}

	void TrailWorker::build_geometry(const TrailPt* pts, size_t total, const WorkerParams& p, FrameGeo& out) {
		out.fill.clear();
		out.outline_upper.clear();
		out.outline_lower.clear();

		if (total < 2) {
			return;
		}

		thread_local Vec<float> xs;
		thread_local Vec<float> ys;
		thread_local Vec<float> ws;
		thread_local Vec<float> ows;

		let seg_start = 0;
		let curr_seg = pts[0].seg;

		auto flush = [&](size_t start, size_t end) {
			let n = end - start;

			if (n < 2) {
				return;
			}

			xs.resize(n);
			ys.resize(n);
			ws.resize(n);

			for (size_t k = 0; k < n; ++k) {
				xs[k] = pts[start + k].x;
				ys[k] = pts[start + k].y;
				ws[k] = pts[start + k].w + p.pulse_add;
			}

			geom::emit_fill_quads(out.fill, xs.data(), ys.data(), ws.data(), n, p.join_style, p.miter_limit);

			if (p.do_outline && p.outline_width > 0.0f) {
				ows.resize(n);
				for (size_t k = 0; k < n; ++k) {
					ows[k] = ws[k] + p.outline_width;
				}

				geom::emit_outline_quads(
					out.outline_upper,
					out.outline_lower,
					xs.data(),
					ys.data(),
					ws.data(),
					ows.data(),
					n,
					p.join_style,
					p.miter_limit
				);
			}
		};

		for (size_t i = 0; i <= total; ++i) {
			let boundary = (i == total) || (pts[i].seg != curr_seg);

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
} // namespace gay
