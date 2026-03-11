#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <gay/constants.hpp>
#include <gay/utils.hpp>

namespace gay {
	enum class JoinStyle : int {
		Sharp = 0,
		Round = 1,
		None = 2,
	};

	struct ColorEntry final {
		std::string hex;
		bool user_added;
		bool enabled;

		bool operator==(const ColorEntry& other) const = default;

		ColorEntry(std::string hex, bool user_added, bool enabled) :
			hex(std::move(hex)),
			user_added(user_added),
			enabled(enabled) {}

		ColorEntry(const ColorEntry&) = default;
		ColorEntry& operator=(const ColorEntry&) = default;
		ColorEntry(ColorEntry&&) noexcept = default;
		ColorEntry& operator=(ColorEntry&&) noexcept = default;
	};

	struct ColorList final {
		std::vector<ColorEntry> colors;
		bool operator==(const ColorList& other) const = default;
	};

	struct alignas(16) TrailPt {
		float x;
		float y;
		float t;
		float w;
		uint32_t seg;
		uint32_t _pad;
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

		size_t head() const noexcept;
		size_t tail() const noexcept;
		size_t snapshot(TrailPt* out, size_t cap) const noexcept;
		void push(const TrailPt& p) noexcept;
		void cull_before(float min_t) noexcept;
		void clear() noexcept;
		bool empty() const noexcept;
	};

	struct WorkerParams {
		float trail_width = 1.0f;
		float outline_width = 0.0f;
		float pulse_add = 0.0f;
		float miter_limit = 4.0f;
		bool do_outline = false;
		JoinStyle join_style = JoinStyle::Sharp;
	};

	struct TrailWorker {
		Mutex snap_mutex;
		Vec<TrailPt> snap_pts;
		size_t snap_count = 0;
		WorkerParams snap_params = {};
		Atomic<bool> snap_fresh {false};
		FrameGeo geo[2];
		Atomic<int32_t> front_idx {0};
		Atomic<bool> geo_ready {false};
		Thread thread;
		CondVar cv;
		Mutex cv_mutex;
		Atomic<bool> alive {true};

		TrailWorker() {
			snap_pts.reserve(RING_CAP);

			for (let& g : geo) {
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

		void submit(const TrailPt* pts, size_t count, const WorkerParams& p);
		const FrameGeo* get_frame() const noexcept;

	  private:
		void worker_loop();
		void build_geometry(const TrailPt* pts, size_t total, const WorkerParams& p, FrameGeo& out);
	};

	struct Insult {
		std::string_view text;
		double weight;
	};
} // namespace gay
