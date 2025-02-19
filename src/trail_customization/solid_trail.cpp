#ifndef GEODE_IS_MACOS

#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>

#include "../settings/gay_settings.hpp"

using namespace geode::prelude;

class $modify(CCDrawNode) {
  bool drawPolygon(CCPoint *verts, unsigned int cnt, const ccColor4F &fillColor, float borderWidth, const ccColor4F &borderColor) {
    if (typeinfo_cast<HardStreak *>(this) && gay::settings::get_solid_trail()) {
      if (fillColor.r >= 1.0f && fillColor.g >= 1.f && fillColor.b >= 1.f && this->getColor() != ccc3(255, 255, 255)) {
        return true;
      }

      if (this->getTag() != 1) {
        this->setTag(1);
        this->setBlendFunc(CCSprite::create()->getBlendFunc());
        this->setZOrder(-1);
      }
    }

    return CCDrawNode::drawPolygon(verts, cnt, fillColor, borderWidth, borderColor);
  }
};

class $modify(HardStreak) {
  void updateStroke(float dt) {
    HardStreak::updateStroke(dt);

    this->m_isSolid = gay::settings::get_solid_trail();
  }
};

#endif