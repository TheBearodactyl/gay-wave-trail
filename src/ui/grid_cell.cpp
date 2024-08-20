#include <Geode/binding/StatsCell.hpp>

#include "grid_cell.hpp"

bool ColorGridCell::init(CCSize const &size) {
  m_width = size.width;
  m_height = size.height;
  this->setContentSize(size);
  this->setID("nong-list-cell");
  return true;
}

void ColorGridCell::draw() { reinterpret_cast<StatsCell *>(this)->StatsCell::draw(); }
