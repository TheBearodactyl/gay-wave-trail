#ifndef COLOR_HPP
#define COLOR_HPP

#pragma once

#include <string>
#include <utility>
#include <vector>

struct ColorEntry final {
public:
  std::string m_hex;
  bool m_userAdded;
  bool m_enabled;

  bool operator==(const ColorEntry &other) const {
    return m_hex == other.m_hex && m_userAdded == other.m_userAdded && m_enabled == other.m_enabled;
  }

  ColorEntry(std::string m_hex, bool m_userAdded, bool m_enabled) : m_hex(std::move(m_hex)), m_enabled(m_enabled), m_userAdded(m_userAdded) {
  }

  ColorEntry(ColorEntry const &) = default;
};

struct ColorList final {
  std::vector<ColorEntry> colors;

  bool operator==(const ColorList &other) const {
    return colors == other.colors;
  }
};

#endif