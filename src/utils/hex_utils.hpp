#ifndef HEX_UTILS_HPP
#define HEX_UTILS_HPP

#pragma once

#include <cocos2d.h>

using namespace cocos2d;

namespace gay::utils::hex_code {
  class HexUtils {
  public:
    static ccColor3B hex_to_rgb(const std::string &hex_code);
  };
}// namespace gay::utils::hex_code

#endif