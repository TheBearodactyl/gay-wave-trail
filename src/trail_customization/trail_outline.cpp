// legit just taken from eclipse I take no credit for this code

#include "../settings/gay_settings.hpp"
#include "Geode/binding/GameManager.hpp"
#include "rainbow_trail.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <cmath>

using namespace geode::prelude;
using namespace gay;

static CCDrawNode *s_currentStreak = nullptr;
static CCDrawNode *s_currentStreak2 = nullptr;

struct TrailOutlinePlayLayer : Modify<TrailOutlinePlayLayer, PlayLayer> {
  void resetLevel() {
    s_currentStreak = nullptr;
    s_currentStreak2 = nullptr;

    PlayLayer::resetLevel();
  }
};

struct TrailOutlineHardStreak : Modify<TrailOutlineHardStreak, HardStreak> {
  void updateStroke(float dt) {
    if (auto play_layer = GameManager::sharedState()->m_playLayer) {
      if (play_layer->m_player1 && this == play_layer->m_player1->m_waveTrail) {
        s_currentStreak = this;
      } else if (s_currentStreak == this) {
        s_currentStreak = nullptr;
      }

      if (play_layer->m_player2 && this == play_layer->m_player2->m_waveTrail) {
        s_currentStreak2 = this;
      } else if (s_currentStreak2 == this) {
        s_currentStreak2 = nullptr;
      }
    }

    HardStreak::updateStroke(dt);
  }
};


struct TrailOutline : Modify<TrailOutline, cocos2d::CCDrawNode> {
  bool drawPolygon(CCPoint *verts, unsigned int count, const ccColor4F &fillColor, float borderWidth, const ccColor4F &borderColor) {
    if ((fillColor.r == 1.F && fillColor.g == 1.F && fillColor.b == 1.F && fillColor.a != 1.F) || ((s_currentStreak != this) && (s_currentStreak2 != this))) {
      return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
    }

    if (settings::get<bool>("wave-outline")) {
      const auto trail_outline_width = settings::get<double>("wave-outline-width");
      const auto outline_colors = settings::get<ColorList>("outline-colors");
      const auto blur_layers = settings::get<double>("wave-outline-blur");
      const auto outline_opacity = settings::get<int>("wave-outline-opacity");

      this->setBlendFunc(CCSprite::create()->getBlendFunc());
      this->setZOrder(-1);

      static float color_phase = 0.0f;
      color_phase += settings::get<float>("speed") * 0.5;
      if (color_phase >= 360.0f)
        color_phase -= 360.0f;

      CCPoint new_verts[4];

      if (blur_layers == 0.0) {
        for (unsigned int i = 0; i < count && i < 4; i++) {
          new_verts[i] = verts[i];
        }

        const auto offset = static_cast<float>(trail_outline_width + (trail_outline_width / count));
        new_verts[0].y -= offset;
        new_verts[3].y -= offset;
        new_verts[1].y += offset;
        new_verts[2].y += offset;

        const auto gradient_color = RainbowTrail::get_gradient(color_phase, 0.0f, true, outline_colors);
        const ccColor4F trail_outline_color = {static_cast<float>(gradient_color.r) / 255.0f, static_cast<float>(gradient_color.g) / 255.0f, static_cast<float>(gradient_color.b) / 255.0f, static_cast<float>(outline_opacity) / 255.0f};

        this->drawSegment(new_verts[0], new_verts[3], static_cast<float>(trail_outline_width), trail_outline_color);
        this->drawSegment(new_verts[1], new_verts[2], static_cast<float>(trail_outline_width), trail_outline_color);

        return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
      }

      const auto blur_layers_int = static_cast<int>(round(blur_layers));
      const auto count_float = static_cast<float>(count);

      for (int i = 0; i < blur_layers_int; i++) {
        const float layer_width = static_cast<float>(trail_outline_width) * (1.0f + (i * 0.8f));
        const float opacity = std::max(0.05f, 0.8f * static_cast<float>(std::pow(0.7f, i)));
        const float layer_phase_offset = static_cast<float>(i) * 5.0f;// Offset each layer slightly
        const auto gradient_color = RainbowTrail::get_gradient(color_phase, layer_phase_offset, true, outline_colors);

        ccColor4F glow_color = {static_cast<float>(gradient_color.r) / 255.0f, static_cast<float>(gradient_color.g) / 255.0f, static_cast<float>(gradient_color.b) / 255.0f, (static_cast<float>(outline_opacity) / 255.0f) * opacity};

        for (unsigned int j = 0; j < count && j < 4; j++) {
          new_verts[j] = verts[j];
        }

        const float offset = layer_width + (layer_width / count_float);
        new_verts[0].y -= offset;
        new_verts[3].y -= offset;
        new_verts[1].y += offset;
        new_verts[2].y += offset;

        this->drawSegment(new_verts[0], new_verts[3], layer_width, glow_color);
        this->drawSegment(new_verts[1], new_verts[2], layer_width, glow_color);
      }
    }

    return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
  }
};
