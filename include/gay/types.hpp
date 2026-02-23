#pragma once

#include <string>
#include <utility>
#include <vector>

namespace gay {
	struct ColorEntry final {
		std::string hex;
		bool user_added;
		bool enabled;

		bool operator==(const ColorEntry& other) const = default;

		ColorEntry(std::string hex, bool user_added, bool enabled) : hex(std::move(hex)), user_added(user_added), enabled(enabled) {}

		ColorEntry(const ColorEntry&) = default;
		ColorEntry& operator=(const ColorEntry&) = default;
		ColorEntry(ColorEntry&&) noexcept = default;
		ColorEntry& operator=(ColorEntry&&) noexcept = default;
	};

	struct ColorList final {
		std::vector<ColorEntry> colors;
		bool operator==(const ColorList& other) const = default;
	};
} // namespace gay
