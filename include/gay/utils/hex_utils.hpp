#ifndef HEX_UTILS_HPP
#define HEX_UTILS_HPP

#pragma once

#include <cocos2d.h>

using namespace cocos2d;

namespace gay::utils::hex_code {
	class HexUtils {
	  public:
		static ccColor3B hex_to_rgb(const std::string& hex_code);
		static std::string rgb_to_hex(const _ccColor3B& color);
		static std::string rgb_to_hex(const _ccColor4B& color);
	};
} // namespace gay::utils::hex_code

#endif
