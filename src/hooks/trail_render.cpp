#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/color.hpp>
#include <gay/settings.hpp>

#include <cmath>

using namespace geode::prelude;
namespace settings = gay::settings;
namespace color = gay::color;

#ifndef GEODE_IS_MACOS

struct SolidDrawHook: Modify<SolidDrawHook, CCDrawNode> {
	bool drawPolygon(CCPoint* verts, unsigned int cnt, const ccColor4F& fill, float border_width, const ccColor4F& border) {
		if (typeinfo_cast<HardStreak*>(this) && settings::get<bool>("solid-trail")) {
			if (fill.r >= 1.0f && fill.g >= 1.0f && fill.b >= 1.0f && this->getColor() != ccc3(255, 255, 255)) {
				return true;
			}

			if (this->getTag() != 1) {
				this->setTag(1);
				this->setBlendFunc(CCSprite::create()->getBlendFunc());
				this->setZOrder(-1);
			}

			ccColor4F solid_fill = fill;
			solid_fill.a = 1.0f;
			return CCDrawNode::drawPolygon(verts, cnt, solid_fill, border_width, border);
		}

		return CCDrawNode::drawPolygon(verts, cnt, fill, border_width, border);
	}
};

struct SolidStreakHook: Modify<SolidStreakHook, HardStreak> {
	void updateStroke(float dt) {
		HardStreak::updateStroke(dt);
		this->m_isSolid = settings::get<bool>("solid-trail");
	}
};

#endif

struct DisableStreakHook: Modify<DisableStreakHook, HardStreak> {
	void updateStroke(float dt) {
		if (settings::is_enabled() && settings::get<bool>("disable-pulse") && PlayLayer::get()) {
			double min_width = settings::get<bool>("add-min-width") ? 0.5 : 0.0;
			this->m_pulseSize = static_cast<float>(min_width + settings::get_float("pulse-size"));
		}

		HardStreak::updateStroke(dt);
	}
};

struct DisablePlayHook: Modify<DisablePlayHook, PlayLayer> {
	void postUpdate(float dt) {
		if (settings::is_enabled()) {
			bool disable = settings::get<bool>("disable-trail");
			if (this->m_player1 && this->m_player1->m_waveTrail) {
				this->m_player1->m_waveTrail->setVisible(!disable);
			}
			if (this->m_player2 && this->m_player2->m_waveTrail) {
				this->m_player2->m_waveTrail->setVisible(!disable);
			}
		}

		PlayLayer::postUpdate(dt);
	}
};

struct TrailFadeHook: Modify<TrailFadeHook, HardStreak> {
	struct Fields {
		float accumulated_time = 0.0f;
	};

	void updateStroke(float dt) {
		HardStreak::updateStroke(dt);

		if (!settings::is_enabled()) {
			return;
		}

		int trail_opacity = settings::get<int>("trail-opacity");

		if (!settings::get<bool>("enable-trail-fade")) {
			this->setOpacity(static_cast<GLubyte>(trail_opacity));
			return;
		}

		float fade_speed = settings::get_float("trail-fade-speed");
		int min_opacity = settings::get<int>("trail-min-opacity");

		m_fields->accumulated_time += dt * fade_speed;

		int faded = static_cast<int>(static_cast<float>(trail_opacity) / (1.0f + m_fields->accumulated_time * 0.1f));
		auto target = static_cast<GLubyte>(std::max(min_opacity, faded));

		this->setOpacity(target);
	}
};

struct OutlineStreakState {
	CCDrawNode* node = nullptr;
	bool is_p1 = false;

	void reset() {
		node = nullptr;
		is_p1 = false;
	}
};

static OutlineStreakState s_outline_p1;
static OutlineStreakState s_outline_p2;

struct OutlinePlayHook: Modify<OutlinePlayHook, PlayLayer> {
	void resetLevel() {
		s_outline_p1.reset();
		s_outline_p2.reset();
		PlayLayer::resetLevel();
	}

	void onQuit() {
		s_outline_p1.reset();
		s_outline_p2.reset();
		PlayLayer::onQuit();
	}
};

struct OutlineStreakHook: Modify<OutlineStreakHook, HardStreak> {
	void updateStroke(float delta) {
		auto* play_layer = GameManager::sharedState()->m_playLayer;

		if (play_layer) {
			if (play_layer->m_player1 && play_layer->m_player1->m_waveTrail && this == play_layer->m_player1->m_waveTrail) {
				s_outline_p1.node = this;
				s_outline_p1.is_p1 = true;
			} else if (s_outline_p1.node == this) {
				s_outline_p1.reset();
			}

			if (play_layer->m_player2 && play_layer->m_player2->m_waveTrail && this == play_layer->m_player2->m_waveTrail) {
				s_outline_p2.node = this;
				s_outline_p2.is_p1 = false;
			} else if (s_outline_p2.node == this) {
				s_outline_p2.reset();
			}
		}

		HardStreak::updateStroke(delta);
	}
};

struct OutlineDrawHook: Modify<OutlineDrawHook, CCDrawNode> {
	struct Fields {
		float phase_p1 = 0.0f;
		float phase_p2 = 0.0f;
	};

	bool drawPolygon(CCPoint* verts, unsigned int count, const ccColor4F& fill, float border_width, const ccColor4F& border) {
		if (fill.r == 1.0f && fill.g == 1.0f && fill.b == 1.0f && fill.a != 1.0f) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		bool is_tracked = (this == s_outline_p1.node) || (this == s_outline_p2.node);
		if (!is_tracked || !settings::get<bool>("wave-outline")) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		bool is_p1 = (this == s_outline_p1.node);

		auto outline_width = settings::get<double>("wave-outline-width");
		auto outline_colors = settings::get<gay::ColorList>("outline-colors");
		auto blur_layers = settings::get<double>("wave-outline-blur");
		auto outline_opacity = settings::get<int>("wave-outline-opacity");

		this->setBlendFunc(CCSprite::create()->getBlendFunc());
		this->setZOrder(-1);

		float& phase = is_p1 ? m_fields->phase_p1 : m_fields->phase_p2;

		float dt = CCDirector::sharedDirector()->getDeltaTime();
		phase = std::fmod(phase + settings::get_float("speed") * dt, 360.0f);

		CCPoint expanded[4];
		auto copy_verts = [&]() {
			for (unsigned int i = 0; i < count && i < 4; i++) {
				expanded[i] = verts[i];
			}
		};

		if (blur_layers == 0.0) {
			copy_verts();

			float offset = static_cast<float>(outline_width) + (static_cast<float>(outline_width) / static_cast<float>(count));
			expanded[0].y -= offset;
			expanded[3].y -= offset;
			expanded[1].y += offset;
			expanded[2].y += offset;

			auto gc = color::get_gradient(phase, 0.0f, true, outline_colors);
			ccColor4F outline_color = {gc.r / 255.0f, gc.g / 255.0f, gc.b / 255.0f, static_cast<float>(outline_opacity) / 255.0f};

			this->drawSegment(expanded[0], expanded[3], static_cast<float>(outline_width), outline_color);
			this->drawSegment(expanded[1], expanded[2], static_cast<float>(outline_width), outline_color);

			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		int blur_count = static_cast<int>(std::round(blur_layers));
		float count_f = static_cast<float>(count);

		for (int i = 0; i < blur_count; i++) {
			float layer_width = static_cast<float>(outline_width) * (1.0f + static_cast<float>(i) * 0.8f);
			float opacity = std::max(0.05f, 0.8f * static_cast<float>(std::pow(0.7, i)));
			float phase_offset = static_cast<float>(i) * 5.0f;

			auto gc = color::get_gradient(phase, phase_offset, true, outline_colors);
			ccColor4F glow = {gc.r / 255.0f, gc.g / 255.0f, gc.b / 255.0f, (static_cast<float>(outline_opacity) / 255.0f) * opacity};

			copy_verts();

			float offset = layer_width + (layer_width / count_f);
			expanded[0].y -= offset;
			expanded[3].y -= offset;
			expanded[1].y += offset;
			expanded[2].y += offset;

			this->drawSegment(expanded[0], expanded[3], layer_width, glow);
			this->drawSegment(expanded[1], expanded[2], layer_width, glow);
		}

		return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
	}
};
