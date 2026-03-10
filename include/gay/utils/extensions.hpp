#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <climits>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <fmt/base.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace gay::ext {
	class RustStr {
	  public:
		RustStr() = default;

		RustStr(const char* s) : data_(s ? s : "") {}

		RustStr(std::string s) : data_(std::move(s)) {}

		RustStr(std::string_view sv) : data_(sv) {}

		RustStr(const RustStr&) = default;
		RustStr(RustStr&&) = default;
		RustStr& operator=(const RustStr&) = default;
		RustStr& operator=(RustStr&&) = default;

		operator std::string() const& {
			return data_;
		}

		operator std::string_view() const {
			return data_;
		}

		const std::string& str() const {
			return data_;
		}

		const char* as_ptr() const {
			return data_.data();
		}

		std::size_t len() const {
			return data_.size();
		}

		bool is_empty() const {
			return data_.empty();
		}

		RustStr trim() const {
			auto start = data_.find_first_not_of(" \t\n\r\f\v");
			if (start == std::string::npos) {
				return RustStr {};
			}
			auto end = data_.find_last_not_of(" \t\n\r\f\v");
			return data_.substr(start, end - start + 1);
		}

		RustStr trim_start() const {
			auto start = data_.find_first_not_of(" \t\n\r\f\v");
			if (start == std::string::npos) {
				return RustStr {};
			}
			return data_.substr(start);
		}

		RustStr trim_end() const {
			auto end = data_.find_last_not_of(" \t\n\r\f\v");
			if (end == std::string::npos) {
				return RustStr {};
			}
			return data_.substr(0, end + 1);
		}

		RustStr to_uppercase() const {
			std::string out = data_;
			std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::toupper(c); });
			return out;
		}

		RustStr to_lowercase() const {
			std::string out = data_;
			std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
			return out;
		}

		bool contains(std::string_view needle) const {
			return data_.find(needle) != std::string::npos;
		}

		bool starts_with(std::string_view prefix) const {
			return data_.size() >= prefix.size() && data_.compare(0, prefix.size(), prefix) == 0;
		}

		bool ends_with(std::string_view suffix) const {
			if (suffix.size() > data_.size()) {
				return false;
			}
			return data_.compare(data_.size() - suffix.size(), suffix.size(), suffix) == 0;
		}

		std::optional<std::size_t> find(std::string_view needle) const {
			auto pos = data_.find(needle);
			if (pos == std::string::npos) {
				return std::nullopt;
			}
			return pos;
		}

		std::optional<std::size_t> rfind(std::string_view needle) const {
			auto pos = data_.rfind(needle);
			if (pos == std::string::npos) {
				return std::nullopt;
			}
			return pos;
		}

		std::vector<RustStr> split(std::string_view delimiter) const {
			std::vector<RustStr> result;
			if (delimiter.empty()) {
				for (char c : data_) {
					result.push_back(std::string(1, c));
				}
				return result;
			}
			std::size_t start = 0;
			std::size_t pos;
			while ((pos = data_.find(delimiter, start)) != std::string::npos) {
				result.push_back(data_.substr(start, pos - start));
				start = pos + delimiter.size();
			}
			result.push_back(data_.substr(start));
			return result;
		}

		std::vector<RustStr> split_whitespace() const {
			std::vector<RustStr> result;
			std::istringstream ss(data_);
			std::string token;
			while (ss >> token) {
				result.push_back(std::move(token));
			}
			return result;
		}

		std::vector<RustStr> splitn(std::size_t n, std::string_view delimiter) const {
			std::vector<RustStr> result;
			if (n == 0) {
				return result;
			}
			if (n == 1 || delimiter.empty()) {
				result.push_back(data_);
				return result;
			}
			std::size_t start = 0, pos;
			while (result.size() < n - 1 && (pos = data_.find(delimiter, start)) != std::string::npos) {
				result.push_back(data_.substr(start, pos - start));
				start = pos + delimiter.size();
			}
			result.push_back(data_.substr(start));
			return result;
		}

		std::vector<RustStr> lines() const {
			std::vector<RustStr> result;
			std::istringstream ss(data_);
			std::string line;
			while (std::getline(ss, line)) {
				if (!line.empty() && line.back() == '\r') {
					line.pop_back();
				}
				result.push_back(std::move(line));
			}
			return result;
		}

		RustStr replace(std::string_view from, std::string_view to) const {
			std::string result = data_;
			std::size_t pos = 0;
			while ((pos = result.find(from, pos)) != std::string::npos) {
				result.replace(pos, from.size(), to);
				pos += to.size();
			}
			return result;
		}

		RustStr replacen(std::string_view from, std::string_view to, std::size_t count) const {
			std::string result = data_;
			std::size_t pos = 0, replaced = 0;
			while (replaced < count && (pos = result.find(from, pos)) != std::string::npos) {
				result.replace(pos, from.size(), to);
				pos += to.size();
				++replaced;
			}
			return result;
		}

		RustStr repeat(std::size_t n) const {
			std::string result;
			result.reserve(data_.size() * n);
			for (std::size_t i = 0; i < n; ++i) {
				result += data_;
			}
			return result;
		}

		std::vector<char> chars() const {
			return std::vector<char>(data_.begin(), data_.end());
		}

		template<typename T>
		std::optional<T> parse() const {
			T value {};
			auto [ptr, ec] = std::from_chars(data_.data(), data_.data() + data_.size(), value);
			if (ec != std::errc {} || ptr != data_.data() + data_.size()) {
				return std::nullopt;
			}
			return value;
		}

		bool operator==(const RustStr& o) const {
			return data_ == o.data_;
		}

		bool operator!=(const RustStr& o) const {
			return data_ != o.data_;
		}

		bool operator<(const RustStr& o) const {
			return data_ < o.data_;
		}

		RustStr operator+(const RustStr& o) const {
			return data_ + o.data_;
		}

		friend std::ostream& operator<<(std::ostream& os, const RustStr& s) {
			return os << s.data_;
		}

	  private:
		std::string data_;
	};

	template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	class RustInt {
	  public:
		using value_type = T;

		constexpr RustInt() noexcept : val_(0) {}

		constexpr RustInt(T v) noexcept : val_(v) {}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U> && !std::is_same_v<U, T>>>
		constexpr RustInt(U v) noexcept : val_(static_cast<T>(v)) {}

		constexpr operator T() const noexcept {
			return val_;
		}

		constexpr T value() const noexcept {
			return val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U> && !std::is_same_v<U, T>>>
		constexpr explicit operator U() const noexcept {
			return static_cast<U>(val_);
		}

		static constexpr T MIN = std::numeric_limits<T>::min();
		static constexpr T MAX = std::numeric_limits<T>::max();
		static constexpr int BITS = static_cast<int>(sizeof(T) * CHAR_BIT);

		constexpr RustInt abs() const noexcept {
			if constexpr (std::is_signed_v<T>) {
				return val_ < 0 ? -val_ : val_;
			} else {
				return val_;
			}
		}

		constexpr RustInt pow(uint32_t exp) const noexcept {
			T base = val_, result = 1;
			while (exp > 0) {
				if (exp & 1) {
					result *= base;
				}
				base *= base;
				exp >>= 1;
			}
			return result;
		}

		constexpr RustInt min_(RustInt other) const noexcept {
			return val_ < other.val_ ? val_ : other.val_;
		}

		constexpr RustInt max_(RustInt other) const noexcept {
			return val_ > other.val_ ? val_ : other.val_;
		}

		constexpr RustInt clamp(RustInt lo, RustInt hi) const noexcept {
			assert(lo.val_ <= hi.val_);
			if (val_ < lo.val_) {
				return lo;
			}
			if (val_ > hi.val_) {
				return hi;
			}
			return val_;
		}

		constexpr RustInt signum() const noexcept {
			if constexpr (std::is_signed_v<T>) {
				return (val_ > 0) - (val_ < 0);
			} else {
				return val_ > 0 ? T(1) : T(0);
			}
		}

		constexpr std::optional<T> checked_add(RustInt rhs) const noexcept {
			if constexpr (std::is_signed_v<T>) {
				if (rhs.val_ > 0 && val_ > MAX - rhs.val_) {
					return std::nullopt;
				}
				if (rhs.val_ < 0 && val_ < MIN - rhs.val_) {
					return std::nullopt;
				}
			} else {
				if (val_ > MAX - rhs.val_) {
					return std::nullopt;
				}
			}
			return static_cast<T>(val_ + rhs.val_);
		}

		constexpr std::optional<T> checked_sub(RustInt rhs) const noexcept {
			if constexpr (std::is_signed_v<T>) {
				if (rhs.val_ < 0 && val_ > MAX + rhs.val_) {
					return std::nullopt;
				}
				if (rhs.val_ > 0 && val_ < MIN + rhs.val_) {
					return std::nullopt;
				}
			} else {
				if (val_ < rhs.val_) {
					return std::nullopt;
				}
			}
			return static_cast<T>(val_ - rhs.val_);
		}

		constexpr std::optional<T> checked_mul(RustInt rhs) const noexcept {
			if (rhs.val_ != 0 && val_ != 0) {
				if constexpr (std::is_signed_v<T>) {
					if (val_ > 0 && rhs.val_ > 0 && val_ > MAX / rhs.val_) {
						return std::nullopt;
					}
					if (val_ < 0 && rhs.val_ < 0 && val_ < MAX / rhs.val_) {
						return std::nullopt;
					}
					if (val_ > 0 && rhs.val_ < 0 && rhs.val_ < MIN / val_) {
						return std::nullopt;
					}
					if (val_ < 0 && rhs.val_ > 0 && val_ < MIN / rhs.val_) {
						return std::nullopt;
					}
				} else {
					if (val_ > MAX / rhs.val_) {
						return std::nullopt;
					}
				}
			}
			return static_cast<T>(val_ * rhs.val_);
		}

		constexpr std::optional<T> checked_div(RustInt rhs) const noexcept {
			if (rhs.val_ == 0) {
				return std::nullopt;
			}
			if constexpr (std::is_signed_v<T>) {
				if (val_ == MIN && rhs.val_ == T(-1)) {
					return std::nullopt;
				}
			}
			return static_cast<T>(val_ / rhs.val_);
		}

		constexpr RustInt saturating_add(RustInt rhs) const noexcept {
			auto r = checked_add(rhs);
			if (r) {
				return *r;
			}
			return (rhs.val_ > 0) ? MAX : MIN;
		}

		constexpr RustInt saturating_sub(RustInt rhs) const noexcept {
			auto r = checked_sub(rhs);
			if (r) {
				return *r;
			}
			return (rhs.val_ > 0) ? MIN : MAX;
		}

		constexpr RustInt saturating_mul(RustInt rhs) const noexcept {
			auto r = checked_mul(rhs);
			if (r) {
				return *r;
			}
			if constexpr (std::is_signed_v<T>) {
				return ((val_ ^ rhs.val_) >= 0) ? MAX : MIN;
			} else {
				return MAX;
			}
		}

		constexpr RustInt wrapping_add(RustInt rhs) const noexcept {
			using U = std::make_unsigned_t<T>;
			return static_cast<T>(static_cast<U>(val_) + static_cast<U>(rhs.val_));
		}

		constexpr RustInt wrapping_sub(RustInt rhs) const noexcept {
			using U = std::make_unsigned_t<T>;
			return static_cast<T>(static_cast<U>(val_) - static_cast<U>(rhs.val_));
		}

		constexpr RustInt wrapping_mul(RustInt rhs) const noexcept {
			using U = std::make_unsigned_t<T>;
			return static_cast<T>(static_cast<U>(val_) * static_cast<U>(rhs.val_));
		}

		constexpr uint32_t leading_zeros() const noexcept {
			using U = std::make_unsigned_t<T>;
			U u = static_cast<U>(val_);
			if (u == 0) {
				return static_cast<uint32_t>(BITS);
			}
			uint32_t n = 0;
			U mask = U(1) << (BITS - 1);
			while ((u & mask) == 0) {
				++n;
				mask >>= 1;
			}
			return n;
		}

		constexpr uint32_t trailing_zeros() const noexcept {
			using U = std::make_unsigned_t<T>;
			U u = static_cast<U>(val_);
			if (u == 0) {
				return static_cast<uint32_t>(BITS);
			}
			uint32_t n = 0;
			while ((u & 1) == 0) {
				++n;
				u >>= 1;
			}
			return n;
		}

		constexpr uint32_t count_ones() const noexcept {
			using U = std::make_unsigned_t<T>;
			U u = static_cast<U>(val_);
			uint32_t n = 0;
			while (u) {
				n += u & 1;
				u >>= 1;
			}
			return n;
		}

		constexpr uint32_t count_zeros() const noexcept {
			return static_cast<uint32_t>(BITS) - count_ones();
		}

		constexpr RustInt rotate_left(uint32_t n) const noexcept {
			using U = std::make_unsigned_t<T>;
			n %= BITS;
			U u = static_cast<U>(val_);
			return static_cast<T>((u << n) | (u >> (BITS - n)));
		}

		constexpr RustInt rotate_right(uint32_t n) const noexcept {
			using U = std::make_unsigned_t<T>;
			n %= BITS;
			U u = static_cast<U>(val_);
			return static_cast<T>((u >> n) | (u << (BITS - n)));
		}

		constexpr RustInt swap_bytes() const noexcept {
			using U = std::make_unsigned_t<T>;
			U u = static_cast<U>(val_);
			U result = 0;
			for (int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
				result = (result << 8) | (u & 0xFF);
				u >>= 8;
			}
			return static_cast<T>(result);
		}

		constexpr bool is_zero() const noexcept {
			return val_ == 0;
		}

		constexpr bool is_positive() const noexcept {
			return val_ > 0;
		}

		constexpr bool is_negative() const noexcept {
			if constexpr (std::is_signed_v<T>) {
				return val_ < 0;
			}
			return false;
		}

		std::string to_string() const {
			return std::to_string(val_);
		}

		static std::optional<RustInt> from_str(std::string_view s) {
			T value {};
			auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
			if (ec != std::errc {} || ptr != s.data() + s.size()) {
				return std::nullopt;
			}
			return RustInt(value);
		}

		constexpr RustInt operator+(RustInt o) const noexcept {
			return val_ + o.val_;
		}

		constexpr RustInt operator-(RustInt o) const noexcept {
			return val_ - o.val_;
		}

		constexpr RustInt operator*(RustInt o) const noexcept {
			return val_ * o.val_;
		}

		constexpr RustInt operator/(RustInt o) const noexcept {
			return val_ / o.val_;
		}

		constexpr RustInt operator%(RustInt o) const noexcept {
			return val_ % o.val_;
		}

		constexpr RustInt operator-() const noexcept {
			return -val_;
		}

		constexpr RustInt& operator++() noexcept {
			++val_;
			return *this;
		}

		constexpr RustInt operator++(int) noexcept {
			auto t = *this;
			++val_;
			return t;
		}

		constexpr RustInt& operator--() noexcept {
			--val_;
			return *this;
		}

		constexpr RustInt operator--(int) noexcept {
			auto t = *this;
			--val_;
			return t;
		}

		constexpr RustInt& operator+=(RustInt o) noexcept {
			val_ += o.val_;
			return *this;
		}

		constexpr RustInt& operator-=(RustInt o) noexcept {
			val_ -= o.val_;
			return *this;
		}

		constexpr RustInt& operator*=(RustInt o) noexcept {
			val_ *= o.val_;
			return *this;
		}

		constexpr RustInt& operator/=(RustInt o) noexcept {
			val_ /= o.val_;
			return *this;
		}

		constexpr bool operator==(RustInt o) const noexcept {
			return val_ == o.val_;
		}

		constexpr bool operator!=(RustInt o) const noexcept {
			return val_ != o.val_;
		}

		constexpr bool operator<(RustInt o) const noexcept {
			return val_ < o.val_;
		}

		constexpr bool operator<=(RustInt o) const noexcept {
			return val_ <= o.val_;
		}

		constexpr bool operator>(RustInt o) const noexcept {
			return val_ > o.val_;
		}

		constexpr bool operator>=(RustInt o) const noexcept {
			return val_ >= o.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator+(RustInt a, U b) noexcept {
			return a.val_ + static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator+(U a, RustInt b) noexcept {
			return static_cast<T>(a) + b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator-(RustInt a, U b) noexcept {
			return a.val_ - static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator-(U a, RustInt b) noexcept {
			return static_cast<T>(a) - b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator*(RustInt a, U b) noexcept {
			return a.val_ * static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator*(U a, RustInt b) noexcept {
			return static_cast<T>(a) * b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator/(RustInt a, U b) noexcept {
			return a.val_ / static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr RustInt operator/(U a, RustInt b) noexcept {
			return static_cast<T>(a) / b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator==(RustInt a, U b) noexcept {
			return a.val_ == static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator==(U a, RustInt b) noexcept {
			return static_cast<T>(a) == b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator!=(RustInt a, U b) noexcept {
			return a.val_ != static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator!=(U a, RustInt b) noexcept {
			return static_cast<T>(a) != b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator<(RustInt a, U b) noexcept {
			return a.val_ < static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator<(U a, RustInt b) noexcept {
			return static_cast<T>(a) < b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator<=(RustInt a, U b) noexcept {
			return a.val_ <= static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator<=(U a, RustInt b) noexcept {
			return static_cast<T>(a) <= b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator>(RustInt a, U b) noexcept {
			return a.val_ > static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator>(U a, RustInt b) noexcept {
			return static_cast<T>(a) > b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator>=(RustInt a, U b) noexcept {
			return a.val_ >= static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend constexpr bool operator>=(U a, RustInt b) noexcept {
			return static_cast<T>(a) >= b.val_;
		}

		friend std::ostream& operator<<(std::ostream& os, const RustInt& i) {
			return os << i.val_;
		}

	  private:
		T val_;
	};

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	class RustFloat {
	  public:
		using value_type = T;

		constexpr RustFloat() noexcept : val_(0) {}

		constexpr RustFloat(T v) noexcept : val_(v) {}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U> && !std::is_same_v<U, T>>>
		constexpr RustFloat(U v) noexcept : val_(static_cast<T>(v)) {}

		constexpr operator T() const noexcept {
			return val_;
		}

		constexpr T value() const noexcept {
			return val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U> && !std::is_same_v<U, T>>>
		constexpr explicit operator U() const noexcept {
			return static_cast<U>(val_);
		}

		static constexpr T nan() noexcept {
			return std::numeric_limits<T>::quiet_NaN();
		}

		static constexpr T infinity() noexcept {
			return std::numeric_limits<T>::infinity();
		}

		static constexpr T neg_infinity() noexcept {
			return -std::numeric_limits<T>::infinity();
		}

		static constexpr T min_positive() noexcept {
			return std::numeric_limits<T>::min();
		}

		static constexpr T max_val() noexcept {
			return std::numeric_limits<T>::max();
		}

		static constexpr T min_val() noexcept {
			return -std::numeric_limits<T>::max();
		}

		static constexpr T epsilon() noexcept {
			return std::numeric_limits<T>::epsilon();
		}

		bool is_nan() const noexcept {
			return std::isnan(val_);
		}

		bool is_infinite() const noexcept {
			return std::isinf(val_);
		}

		bool is_zero() const noexcept {
			return val_ == 0.0f;
		}

		bool is_finite() const noexcept {
			return std::isfinite(val_);
		}

		bool is_sign_positive() const noexcept {
			return !std::signbit(val_);
		}

		bool is_sign_negative() const noexcept {
			return std::signbit(val_);
		}

		RustFloat abs() const noexcept {
			return std::abs(val_);
		}

		RustFloat sqrt() const noexcept {
			return std::sqrt(val_);
		}

		RustFloat cbrt() const noexcept {
			return std::cbrt(val_);
		}

		RustFloat powi(int exp) const noexcept {
			return std::pow(val_, static_cast<T>(exp));
		}

		RustFloat powf(T exp) const noexcept {
			return std::pow(val_, exp);
		}

		RustFloat ln() const noexcept {
			return std::log(val_);
		}

		RustFloat log2() const noexcept {
			return std::log2(val_);
		}

		RustFloat log10() const noexcept {
			return std::log10(val_);
		}

		RustFloat log(T base) const noexcept {
			return std::log(val_) / std::log(base);
		}

		RustFloat exp() const noexcept {
			return std::exp(val_);
		}

		RustFloat exp2() const noexcept {
			return std::exp2(val_);
		}

		RustFloat floor() const noexcept {
			return std::floor(val_);
		}

		RustFloat ceil() const noexcept {
			return std::ceil(val_);
		}

		RustFloat round() const noexcept {
			return std::round(val_);
		}

		RustFloat trunc() const noexcept {
			return std::trunc(val_);
		}

		RustFloat fract() const noexcept {
			return val_ - std::trunc(val_);
		}

		RustFloat min(RustFloat other) const noexcept {
			return std::fmin(val_, other.val_);
		}

		RustFloat max(RustFloat other) const noexcept {
			return std::fmax(val_, other.val_);
		}

		RustFloat clamp(RustFloat lo, RustFloat hi) const noexcept {
			if (val_ < lo.val_) {
				return lo;
			}
			if (val_ > hi.val_) {
				return hi;
			}
			return val_;
		}

		RustFloat signum() const noexcept {
			if (is_nan()) {
				return val_;
			}
			if (val_ > T(0)) {
				return T(1);
			}
			if (val_ < T(0)) {
				return T(-1);
			}
			return T(0);
		}

		RustFloat recip() const noexcept {
			return T(1) / val_;
		}

		RustFloat hypot(RustFloat other) const noexcept {
			return std::hypot(val_, other.val_);
		}

		RustFloat mul_add(RustFloat a, RustFloat b) const noexcept {
			return std::fma(val_, a.val_, b.val_);
		}

		RustFloat sin() const noexcept {
			return std::sin(val_);
		}

		RustFloat cos() const noexcept {
			return std::cos(val_);
		}

		RustFloat tan() const noexcept {
			return std::tan(val_);
		}

		RustFloat asin() const noexcept {
			return std::asin(val_);
		}

		RustFloat acos() const noexcept {
			return std::acos(val_);
		}

		RustFloat atan() const noexcept {
			return std::atan(val_);
		}

		RustFloat atan2(RustFloat other) const noexcept {
			return std::atan2(val_, other.val_);
		}

		RustFloat sinh() const noexcept {
			return std::sinh(val_);
		}

		RustFloat cosh() const noexcept {
			return std::cosh(val_);
		}

		RustFloat tanh() const noexcept {
			return std::tanh(val_);
		}

		std::pair<RustFloat, RustFloat> sin_cos() const noexcept {
			return {std::sin(val_), std::cos(val_)};
		}

		RustFloat to_radians() const noexcept {
			constexpr T PI = T(3.14159265358979323846L);
			return val_ * (PI / T(180));
		}

		RustFloat to_degrees() const noexcept {
			constexpr T PI = T(3.14159265358979323846L);
			return val_ * (T(180) / PI);
		}

		std::string to_string() const {
			return std::to_string(val_);
		}

		RustFloat operator+(RustFloat o) const noexcept {
			return val_ + o.val_;
		}

		RustFloat operator-(RustFloat o) const noexcept {
			return val_ - o.val_;
		}

		RustFloat operator*(RustFloat o) const noexcept {
			return val_ * o.val_;
		}

		RustFloat operator/(RustFloat o) const noexcept {
			return val_ / o.val_;
		}

		RustFloat operator-() const noexcept {
			return -val_;
		}

		RustFloat& operator+=(RustFloat o) noexcept {
			val_ += o.val_;
			return *this;
		}

		RustFloat& operator-=(RustFloat o) noexcept {
			val_ -= o.val_;
			return *this;
		}

		RustFloat& operator*=(RustFloat o) noexcept {
			val_ *= o.val_;
			return *this;
		}

		RustFloat& operator/=(RustFloat o) noexcept {
			val_ /= o.val_;
			return *this;
		}

		bool operator==(RustFloat o) const noexcept {
			return val_ == o.val_;
		}

		bool operator!=(RustFloat o) const noexcept {
			return val_ != o.val_;
		}

		bool operator<(RustFloat o) const noexcept {
			return val_ < o.val_;
		}

		bool operator<=(RustFloat o) const noexcept {
			return val_ <= o.val_;
		}

		bool operator>(RustFloat o) const noexcept {
			return val_ > o.val_;
		}

		bool operator>=(RustFloat o) const noexcept {
			return val_ >= o.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator+(RustFloat a, U b) noexcept {
			return a.val_ + static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator+(U a, RustFloat b) noexcept {
			return static_cast<T>(a) + b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator-(RustFloat a, U b) noexcept {
			return a.val_ - static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator-(U a, RustFloat b) noexcept {
			return static_cast<T>(a) - b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator*(RustFloat a, U b) noexcept {
			return a.val_ * static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator*(U a, RustFloat b) noexcept {
			return static_cast<T>(a) * b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator/(RustFloat a, U b) noexcept {
			return a.val_ / static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend RustFloat operator/(U a, RustFloat b) noexcept {
			return static_cast<T>(a) / b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator==(RustFloat a, U b) noexcept {
			return a.val_ == static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator==(U a, RustFloat b) noexcept {
			return static_cast<T>(a) == b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator!=(RustFloat a, U b) noexcept {
			return a.val_ != static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator!=(U a, RustFloat b) noexcept {
			return static_cast<T>(a) != b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator<(RustFloat a, U b) noexcept {
			return a.val_ < static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator<(U a, RustFloat b) noexcept {
			return static_cast<T>(a) < b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator<=(RustFloat a, U b) noexcept {
			return a.val_ <= static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator<=(U a, RustFloat b) noexcept {
			return static_cast<T>(a) <= b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator>(RustFloat a, U b) noexcept {
			return a.val_ > static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator>(U a, RustFloat b) noexcept {
			return static_cast<T>(a) > b.val_;
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator>=(RustFloat a, U b) noexcept {
			return a.val_ >= static_cast<T>(b);
		}

		template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
		friend bool operator>=(U a, RustFloat b) noexcept {
			return static_cast<T>(a) >= b.val_;
		}

		friend std::ostream& operator<<(std::ostream& os, const RustFloat& f) {
			return os << f.val_;
		}

	  private:
		T val_;
	};

	//

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator+(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) + b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator+(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() + static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator-(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) - b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator-(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() - static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator*(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) * b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator*(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() * static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator/(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) / b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr RustFloat<F> operator/(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() / static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator==(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) == b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator==(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() == static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator!=(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) != b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator!=(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() != static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator<(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) < b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator<(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() < static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator<=(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) <= b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator<=(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() <= static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator>(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) > b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator>(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() > static_cast<F>(b.value());
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator>=(RustInt<I> a, RustFloat<F> b) noexcept {
		return static_cast<F>(a.value()) >= b.value();
	}

	template<
		typename I,
		typename F,
		typename = std::enable_if_t<std::is_integral_v<I>>,
		typename = std::enable_if_t<std::is_floating_point_v<F>>>
	constexpr bool operator>=(RustFloat<F> a, RustInt<I> b) noexcept {
		return a.value() >= static_cast<F>(b.value());
	}

	using i8 = RustInt<int8_t>;
	using i16 = RustInt<int16_t>;
	using i32 = RustInt<int32_t>;
	using i64 = RustInt<int64_t>;
	using u8 = RustInt<uint8_t>;
	using u16 = RustInt<uint16_t>;
	using u32 = RustInt<uint32_t>;
	using u64 = RustInt<uint64_t>;
	using isize = RustInt<std::ptrdiff_t>;
	using usize = RustInt<std::size_t>;
	using f32 = RustFloat<float>;
	using f64 = RustFloat<double>;

} // namespace gay::ext

template<typename T, typename E>
struct fmt::formatter<gay::ext::RustInt<T, E>>: fmt::formatter<T> {
	template<typename FormatContext>
	auto format(const gay::ext::RustInt<T, E>& v, FormatContext& ctx) const {
		return fmt::formatter<T>::format(v.value(), ctx);
	}
};

template<typename T, typename E>
struct fmt::formatter<gay::ext::RustFloat<T, E>>: fmt::formatter<T> {
	template<typename FormatContext>
	auto format(const gay::ext::RustFloat<T, E>& v, FormatContext& ctx) const {
		return fmt::formatter<T>::format(v.value(), ctx);
	}
};

template<>
struct fmt::formatter<gay::ext::RustStr>: fmt::formatter<std::string_view> {
	template<typename FormatContext>
	auto format(const gay::ext::RustStr& v, FormatContext& ctx) const {
		return fmt::formatter<std::string_view>::format(std::string_view(v), ctx);
	}
};
