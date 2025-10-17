#include <Geode/Geode.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/customization/rainbow.hpp>
#include <gay/settings/gay_settings.hpp>

#include <cmath>

using namespace geode::prelude;
using namespace gay;

struct StreakState {
	CCDrawNode* node = nullptr;
	bool is_pl1 = false;

	void reset() {
		node = nullptr;
		is_pl1 = false;
	}
};

static StreakState s_streak_pl1;
static StreakState s_streak_pl2;

struct TrailOutlinePlayLayer: Modify<TrailOutlinePlayLayer, PlayLayer> {
	void resetLevel() {
		s_streak_pl1.reset();
		s_streak_pl2.reset();

		PlayLayer::resetLevel();
	}

	void onQuit() {
		s_streak_pl1.reset();
		s_streak_pl2.reset();

		PlayLayer::onQuit();
	}
};

struct TrailOutlineHardStreak: Modify<TrailOutlineHardStreak, HardStreak> {
	void updateStroke(float delta) {
		auto* play_layer = GameManager::sharedState()->m_playLayer;

		if (play_layer) {
			if (play_layer->m_player1 and this == play_layer->m_player1->m_waveTrail) {
				s_streak_pl1.node = this;
				s_streak_pl1.is_pl1 = true;
			} else if (s_streak_pl1.node == this) {
				s_streak_pl1.reset();
			}

			if (play_layer->m_player2 and this == play_layer->m_player2->m_waveTrail) {
				s_streak_pl2.node = this;
				s_streak_pl2.is_pl1 = false;
			} else if (s_streak_pl2.node == this) {
				s_streak_pl2.reset();
			}
		}

		HardStreak::updateStroke(delta);
	}
};

struct TrailOutline: Modify<TrailOutline, cocos2d::CCDrawNode> {
	bool drawPolygon(CCPoint* verts, unsigned int count, const ccColor4F& fill_col, float border_width, const ccColor4F& border_col) {
		if ((fill_col.r == 1.F and fill_col.g == 1.F and fill_col.b == 1.F and fill_col.a != 1.F)) {
			return CCDrawNode::drawPolygon(verts, count, fill_col, border_width, border_col);
		}

		bool is_tracked_streak = (this == s_streak_pl1.node) || (this == s_streak_pl2.node);
		bool is_pl1_trail = (this == s_streak_pl1.node);

		if (!is_tracked_streak) {
			return CCDrawNode::drawPolygon(verts, count, fill_col, border_width, border_col);
		}

		if (!settings::get<bool>("wave-outline")) {
			return CCDrawNode::drawPolygon(verts, count, fill_col, border_width, border_col);
		}

		const auto trail_outline_width = settings::get<double>("wave-outline-width");
		const auto outline_colors = settings::get<ColorList>("outline-colors");
		const auto blur_layers = settings::get<double>("wave-outline-blur");
		const auto outline_opacity = settings::get<int>("wave-outline-opacity");

		this->setBlendFunc(CCSprite::create()->getBlendFunc());
		this->setZOrder(-1);

		static float color_phase_p1 = 0.0f;
		static float color_phase_p2 = 0.0f;

		float& color_phase = is_pl1_trail ? color_phase_p1 : color_phase_p2;

		color_phase += settings::get<float>("speed") * 0.5f;
		if (color_phase >= 360.0f) {
			color_phase -= 360.0f;
		}

		CCPoint new_verts[4];

		if (blur_layers == 0.0) {
			for (unsigned int i = 0; i < count and i < 4; i++) {
				new_verts[i] = verts[i];
			}

			const auto offset = static_cast<float>(trail_outline_width + (trail_outline_width / static_cast<float>(count)));
			new_verts[0].y -= offset;
			new_verts[3].y -= offset;
			new_verts[1].y += offset;
			new_verts[2].y += offset;

			const auto gradient_color = RainbowTrail::get_gradient(color_phase, 0.0f, true, outline_colors);
			const ccColor4F trail_outline_color = {
				static_cast<float>(gradient_color.r) / 255.0f,
				static_cast<float>(gradient_color.g) / 255.0f,
				static_cast<float>(gradient_color.b) / 255.0f,
				static_cast<float>(outline_opacity) / 255.0f
			};

			this->drawSegment(new_verts[0], new_verts[3], static_cast<float>(trail_outline_width), trail_outline_color);
			this->drawSegment(new_verts[1], new_verts[2], static_cast<float>(trail_outline_width), trail_outline_color);

			return CCDrawNode::drawPolygon(verts, count, fill_col, border_width, border_col);
		}

		const auto blur_layers_int = static_cast<int>(round(blur_layers));
		const auto count_float = static_cast<float>(count);

		for (int i = 0; i < blur_layers_int; i++) {
			const float layer_width = static_cast<float>(trail_outline_width) * (1.0f + (static_cast<float>(i) * 0.8f));
			const float opacity = std::max(0.05f, 0.8f * static_cast<float>(std::pow(0.7, i)));
			const float layer_phase_offset = static_cast<float>(i) * 5.0f;
			const auto gradient_color = RainbowTrail::get_gradient(color_phase, layer_phase_offset, true, outline_colors);

			ccColor4F glow_color = {
				static_cast<float>(gradient_color.r) / 255.0f,
				static_cast<float>(gradient_color.g) / 255.0f,
				static_cast<float>(gradient_color.b) / 255.0f,
				(static_cast<float>(outline_opacity) / 255.0f) * opacity
			};

			for (unsigned int j = 0; j < count and j < 4; j++) {
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

		return CCDrawNode::drawPolygon(verts, count, fill_col, border_width, border_col);
	}
};
