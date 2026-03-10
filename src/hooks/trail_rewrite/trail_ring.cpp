#include <atomic>

#include <gay/constants.hpp>
#include <gay/types.hpp>
#include <gay/utils.hpp>

namespace gay {
	size_t TrailRing::head() const noexcept {
		return _head.v.load(std::memory_order_acquire);
	}

	size_t TrailRing::tail() const noexcept {
		return _tail.v.load(std::memory_order_acquire);
	}

	void TrailRing::push(const TrailPt& p) noexcept {
		size_t t = _tail.v.load(std::memory_order_relaxed);
		buf[t & RING_MASK] = p;
		_tail.v.store(t + 1, std::memory_order_release);

		size_t h = _head.v.load(std::memory_order_relaxed);

		if ((t + 1) - h > RING_CAP) {
			_head.v.store(h + 1, std::memory_order_relaxed);
		}
	}

	void TrailRing::cull_before(float min_t) noexcept {
		size_t h = _head.v.load(std::memory_order_relaxed);
		size_t t = _tail.v.load(std::memory_order_acquire);

		while (h < t && buf[h & RING_MASK].t < min_t) {
			++h;
		}

		_head.v.store(h, std::memory_order_release);
	}

	void TrailRing::clear() noexcept {
		_head.v.store(_tail.v.load(std::memory_order_relaxed), std::memory_order_release);
	}

	bool TrailRing::empty() const noexcept {
		return _head.v.load(std::memory_order_relaxed) == _tail.v.load(std::memory_order_relaxed);
	}

	size_t TrailRing::snapshot(TrailPt* out, size_t cap) const noexcept {
		size_t h = _head.v.load(std::memory_order_acquire);
		size_t t = _tail.v.load(std::memory_order_acquire);
		size_t n = std::min(t - h, cap);

		for (size_t i = 0; i < n; ++i) {
			out[i] = buf[(h + i) & RING_MASK];
		}

		return n;
	}
} // namespace gay
