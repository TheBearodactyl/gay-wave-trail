#ifndef GEODE_IS_MACOS

#include <Geode/modify/CCDrawNode.hpp>

using namespace geode::prelude;

class $modify(CCDrawNode) {
  bool drawPolygon(CCPoint* verts, unsigned int cnt, const ccColor4F& fillColor, float borderWidth, const ccColor4F& borderColor) {
    if (typeinfo_cast<HardStreak*>(this) && Mod::get()->getSettingValue<bool>("solid-wave-trail")) {
      if (fillColor.r >= 1.0f && fillColor.g >= 1.0f && fillColor.b >= 1.0f && this->getColor() != ccc3(255, 255, 255)) {
        return true; // Likely a shortcut under specific conditions 
      }

      // Optimized blending if not already set
      if (this->getTag() != 1) { 
        this->setTag(1);
        this->setBlendFunc(CCSprite::create()->getBlendFunc()); 
        this->setZOrder(-1); 
      }
    }

    return CCDrawNode::drawPolygon(verts, cnt, fillColor, borderWidth, borderColor);
  }
};

#endif 