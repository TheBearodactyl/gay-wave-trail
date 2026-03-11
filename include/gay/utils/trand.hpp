#pragma once

#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define TRUERNG_ARCH_X86_64
#elif defined(_M_ARM64) || defined(__aarch64__)
#define TRUERNG_ARCH_ARM64
#endif

#if defined(_WIN32)
#define TRUERNG_OS_WINDOWS
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#define TRUERNG_OS_APPLE
#elif defined(__ANDROID__)
#define TRUERNG_OS_ANDROID
#endif

#if defined(TRUERNG_OS_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <bcrypt.h>
#include <windows.h>
#pragma comment(lib, "bcrypt.lib")
#elif defined(TRUERNG_OS_APPLE)
#include <stdlib.h>
#elif defined(TRUERNG_OS_ANDROID)
#include <sys/random.h>
#endif

#if defined(TRUERNG_ARCH_X86_64)
#if defined(TRUERNG_OS_WINDOWS)
#include <intrin.h>
#else
#include <cpuid.h>
#include <immintrin.h>
#endif
#endif

#if defined(TRUERNG_ARCH_ARM64) && defined(TRUERNG_OS_APPLE)
#include <sys/sysctl.h>
#endif

#if defined(TRUERNG_ARCH_ARM64) && defined(TRUERNG_OS_ANDROID)
#include <asm/hwcap.h>
#include <sys/auxv.h>
#ifndef HWCAP2_RNG
#define HWCAP2_RNG (1UL << 16)
#endif
#endif

namespace gay::util::trand {
	namespace detail {
#if defined(TRUERNG_ARCH_X86_64)
		inline bool cpu_has_rdrand() noexcept {
#if defined(TRUERNG_OS_WINDOWS)
			int info[4] = {};
			__cpuid(info, 1);
			return (info[2] & (1 << 30)) != 0;
#else
			unsigned eax = 0;
			unsigned ebx = 0;
			unsigned ecx = 0;
			unsigned edx = 0;

			if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
				return false;
			}

			return (ecx & (1u << 30)) != 0;
#endif
		}

#if defined(__GNUC__) || defined(__clang__)
		__attribute__((target("rdrnd")))
#endif
		inline bool rdrand64(uint64_t& out) noexcept {
			for (int retry = 0; retry < 10; ++retry) {
				unsigned long long val = 0;
				if (_rdrand64_step(&val)) {
					out = static_cast<uint64_t>(val);
					return true;
				}
			}
			return false;
		}
#endif

#if defined(TRUERNG_ARCH_ARM64)
		inline bool cpu_has_rndr() noexcept {
#if defined(TRUERNG_OS_APPLE)
			int val = 0;
			size_t size = sizeof(val);
			return (sysctlbyname("hw.optional.arm.FEAT_RNG", &val, &size, nullptr, 0) == 0) && (val != 0);
#elif defined(TRUERNG_OS_ANDROID)
			return (getauxval(AT_HWCAP2) & HWCAP2_RNG) != 0;
#elif defined(__ARM_FEATURE_RNG)
			return true;
#else
			return false;
#endif
		}

		inline bool rndr64(uint64_t& out) noexcept {
			uint64_t val = 0;
			uint32_t ok = 0;
			__asm__ __volatile__(
				"mrs   %0, s3_3_c2_c4_0  \n\t"
				"cset  %w1, cs            \n\t"
				: "=r"(val), "=r"(ok)
				:
				: "cc"
			);
			if (ok) {
				out = val;
				return true;
			}
			return false;
		}

#endif

		inline uint64_t os_rand64() {
#if defined(TRUERNG_OS_WINDOWS)
			uint64_t val = 0;
			BCryptGenRandom(
				nullptr,
				reinterpret_cast<PUCHAR>(&val),
				static_cast<ULONG>(sizeof(val)),
				BCRYPT_USE_SYSTEM_PREFERRED_RNG
			);
			return val;
#elif defined(TRUERNG_OS_APPLE)
			uint64_t val = 0;
			arc4random_buf(&val, sizeof(val));
			return val;
#elif defined(TRUERNG_OS_ANDROID)
			uint64_t val = 0;
			getrandom(&val, sizeof(val), 0);
			return val;
#else
			static_assert(false, "truerng: unsupported platform");
			return 0;
#endif
		}

		inline bool hw_available() noexcept {
#if defined(TRUERNG_ARCH_X86_64)
			static const bool ok = cpu_has_rdrand();
			return ok;
#elif defined(TRUERNG_ARCH_ARM64)
			static const bool ok = cpu_has_rndr();
			return ok;
#else
			return false;
#endif
		}
	} // namespace detail

	inline uint64_t next_u64() {
		uint64_t val = 0;

#if defined(TRUERNG_ARCH_X86_64)
		if (detail::hw_available() && detail::rdrand64(val)) {
			return val;
		}
#elif defined(TRUERNG_ARCH_ARM64)
		if (detail::hw_available() && detail::rndr64(val)) {
			return val;
		}
#endif

		return detail::os_rand64();
	}

	template<typename T>
	inline T range(T lo, T hi) {
		static_assert(std::is_integral<T>::value, "trng::range requires an integral type");

		if (lo > hi) {
			throw std::invalid_argument("trng::range: lo must be <= hi");
		}
		if (lo == hi) {
			return lo;
		}

		const uint64_t span = static_cast<uint64_t>(hi - lo) + 1u;
		const uint64_t limit = (UINT64_MAX - span + 1u) / span * span;

		uint64_t raw;
		do {
			raw = next_u64();
		} while (raw >= limit);

		return static_cast<T>(lo + static_cast<T>(raw % span));
	}

	template<typename T = double>
	inline T range_f(T lo = T(0), T hi = T(1)) {
		static_assert(std::is_floating_point<T>::value, "trng::range_f requires a floating-point type");
		const T unit = static_cast<T>(next_u64()) / static_cast<T>(UINT64_MAX);
		return lo + unit * (hi - lo);
	}

	template<typename Container>
	inline auto& pick(Container& c) {
		if (c.empty()) {
			throw std::invalid_argument("trng::pick: container is empty");
		}

		return c[range<std::size_t>(0, c.size() - 1)];
	}

	template<typename Container>
	inline const auto& pick(const Container& c) {
		if (c.empty()) {
			throw std::invalid_argument("trng::pick: container is empty");
		}

		return c[range<std::size_t>(0, c.size() - 1)];
	}

	template<typename Iterator>
	inline Iterator pick(Iterator begin, Iterator end) {
		const auto count = static_cast<std::size_t>(end - begin);

		if (count == 0) {
			throw std::invalid_argument("trng::pick: empty range");
		}

		return begin + range<std::size_t>(0, count - 1);
	}

	inline bool hw_rng_available() noexcept {
		return detail::hw_available();
	}
} // namespace gay::util::trand
