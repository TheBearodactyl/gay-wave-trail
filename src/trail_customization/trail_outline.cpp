// legit just taken from eclipse i take no credit for this code

#include "../constants.hpp"
#include "../settings/gay_settings.hpp"
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
    if (s_currentStreak == nullptr || s_currentStreak2 == nullptr) {
      if (auto play_layer = GameManager::sharedState()->m_playLayer) {
        if (play_layer->m_player1 && s_currentStreak == nullptr) {
          if (this == play_layer->m_player1->m_waveTrail) {
            s_currentStreak = this;
            play_layer->m_player1->m_waveTrail->updateStroke(dt);
          }
        }

        if (play_layer->m_player2 && s_currentStreak2 == nullptr) {
          if (this == play_layer->m_player2->m_waveTrail) {
            s_currentStreak2 = this;
            play_layer->m_player2->m_waveTrail->updateStroke(dt);
          }
        }
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

    if (Mod::get()->getSettingValue<bool>("wave-outline")) {
      auto trail_outline_width = settings::get<double>("wave-outline-width");
      auto trail_outline_col_3b = settings::get<ccColor3B>("wave-outline-color");

      this->setBlendFunc(CCSprite::create()->getBlendFunc());
      this->setZOrder(-1);

      const auto blur_layers = settings::get<double>("wave-outline-blur");
      auto trail_outline_color = ccColor4F{
          .r = static_cast<float>(trail_outline_col_3b.r) / 255.0f,
          .g = static_cast<float>(trail_outline_col_3b.g) / 255.0f,
          .b = static_cast<float>(trail_outline_col_3b.b) / 255.0f,
          .a = static_cast<float>(settings::get<int>("wave-outline-opacity")),
      };

      if (blur_layers == 0.0) {
        std::vector<CCPoint> new_verts(count);
        for (unsigned int i = 0; i < count; i++) {
          new_verts[i] = verts[i];
        }

        float offset = trail_outline_width + (trail_outline_width / count);
        new_verts[0].y -= offset;
        new_verts[3].y -= offset;
        new_verts[1].y += offset;
        new_verts[2].y += offset;

        this->drawSegment(new_verts[0], new_verts[3], static_cast<float>(trail_outline_width), trail_outline_color);
        this->drawSegment(new_verts[1], new_verts[2], static_cast<float>(trail_outline_width), trail_outline_color);

        return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
      }

      for (int i = 0; i < static_cast<int>(round(blur_layers)); i++) {
        float layer_width = trail_outline_width * (1.0f + (i * 0.8f));
        float opacity = std::max({0.05f, 0.8f * static_cast<float>(std::pow(0.7f, i))});
        auto glow_color = trail_outline_color;
        glow_color.a *= opacity;

        std::vector<CCPoint> new_verts(count);
        for (unsigned int j = 0; j < count; j++) {
          new_verts[j] = verts[j];
        }

        float offset = layer_width + (layer_width / static_cast<float>(count));
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
