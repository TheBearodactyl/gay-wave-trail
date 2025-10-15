#include "hex_utils.hpp"

namespace gay::utils::hex_code {
	ccColor3B HexUtils::hex_to_rgb(const std::string& hex_code) {
		std::string clean_hex = hex_code[0] == '#' ? hex_code.substr(1) : hex_code;

		int color = geode::utils::numFromString<int>(clean_hex, 16).unwrapOr(0);

		GLubyte red = (color >> 16) & 0xFF;
		GLubyte green = (color >> 8) & 0xFF;
		GLubyte blue = color & 0xFF;

		return ccColor3B {.r = red, .g = green, .b = blue};
	}

	std::string HexUtils::rgb_to_hex(const _ccColor3B& color) {
		std::stringstream ss;
		ss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(color.r) << std::setw(2) << static_cast<int>(color.g)
		   << std::setw(2) << static_cast<int>(color.b);

		std::string result = ss.str();
		for (auto& c : result) {
			c = std::toupper(c);
		}
		return result;
	}

	std::string HexUtils::rgb_to_hex(const _ccColor4B& color) {
		std::stringstream ss;
		ss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(color.r) << std::setw(2) << static_cast<int>(color.g)
		   << std::setw(2) << static_cast<int>(color.b);

		std::string result = ss.str();
		for (auto& c : result) {
			c = std::toupper(c);
		}
		return result;
	}
} // namespace gay::utils::hex_code
