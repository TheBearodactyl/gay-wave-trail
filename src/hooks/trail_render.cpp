#include <cmath>

#include <gay/color.hpp>
#include <gay/settings.hpp>

#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;
namespace color = gay::color;

#ifndef GEODE_IS_MACOS

struct SolidDrawHook: Modify<SolidDrawHook, CCDrawNode> {
	void draw() override {
		if (typeinfo_cast<HardStreak*>(this) && settings::get<bool>("solid-trail")) {
			for (GLsizei i = 0; i < m_nBufferCount; i++) {
				m_pBuffer[i].colors.a = 255;
			}

			m_bDirty = true;

			this->setBlendFunc({GL_ONE, GL_ONE_MINUS_SRC_ALPHA});
			this->setZOrder(-1);
		}

		CCDrawNode::draw();
	}
};

#endif

struct SolidStreakHook: Modify<SolidStreakHook, HardStreak> {
	void updateStroke(float dt) {
		this->m_isSolid = settings::get<bool>("solid-trail");
		HardStreak::updateStroke(dt);
	}
};

struct DisableStreakHook: Modify<DisableStreakHook, HardStreak> {
	void updateStroke(float dt) {
		if (settings::is_enabled() && settings::get<bool>("disable-pulse") && PlayLayer::get()) {
			double min_width = settings::get<bool>("add-min-width") ? 0.5 : 0.0;
			this->m_pulseSize = static_cast<float>(min_width + settings::get_float("pulse-size"));
		}

		HardStreak::updateStroke(dt);
	}
};

struct TrailFadeHook: Modify<TrailFadeHook, HardStreak> {
	struct Fields {
		float accumulated_time = 0.0f;
	};

	void updateStroke(float dt) {
		if (settings::get<bool>("solid-trail") || this->m_isSolid) {
			this->setOpacity(255);
			HardStreak::updateStroke(dt);
			return;
		}

		if (!settings::is_enabled()) {
			HardStreak::updateStroke(dt);
			return;
		}

		int trail_opacity = settings::get<int>("trail-opacity");

		if (!settings::get<bool>("enable-trail-fade")) {
			this->setOpacity(static_cast<GLubyte>(trail_opacity));
			HardStreak::updateStroke(dt);
			return;
		}

		float fade_speed = settings::get_float("trail-fade-speed");
		int min_opacity = settings::get<int>("trail-min-opacity");

		m_fields->accumulated_time += dt * fade_speed;

		int faded = static_cast<int>(static_cast<float>(trail_opacity) / (1.0f + m_fields->accumulated_time * 0.1f));
		auto target = static_cast<GLubyte>(std::max(min_opacity, faded));

		this->setOpacity(target);
		HardStreak::updateStroke(dt);
	}
};

struct RegularStreakHook: Modify<RegularStreakHook, PlayerObject> {
	void update(float dt) {
		bool disable_regular_streak = settings::get<bool>("disable-behind-streak");
		auto* pl = PlayLayer::get();

		if (!pl) {
			PlayerObject::update(dt);
			return;
		}

		bool is_wave_trail = (pl->m_player1 && pl->m_player1->m_isDart) || (pl->m_player2 && pl->m_player2->m_isDart);

		if (disable_regular_streak && is_wave_trail) {
			m_disableStreakTint = true;
			m_regularTrail->setVisible(false);
		}

		PlayerObject::update(dt);
	}
};

struct OutlineDrawHook: Modify<OutlineDrawHook, CCDrawNode> {
	bool drawPolygon(CCPoint* verts, unsigned int count, const ccColor4F& fill, float border_width, const ccColor4F& border) {
		HardStreak* streak = typeinfo_cast<HardStreak*>(this);

		if (!streak) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		if (!settings::get<bool>("wave-outline") || settings::get<bool>("rewrite-trail")) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		auto* pl = PlayLayer::get();

		if (!pl) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		bool is_wave_trail =
			(pl->m_player1 && pl->m_player1->m_waveTrail == streak) || (pl->m_player2 && pl->m_player2->m_waveTrail == streak);

		if (!is_wave_trail) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		if (fill.a < 0.05f) {
			return CCDrawNode::drawPolygon(verts, count, fill, border_width, border);
		}

		auto outline_width = settings::get<double>("wave-outline-width");
		auto outline_colors = settings::get<gay::ColorList>("outline-colors");
		auto blur_layers = settings::get<double>("wave-outline-blur");
		auto outline_opacity = settings::get<int>("wave-outline-opacity");

		this->setBlendFunc(CCSprite::create()->getBlendFunc());
		this->setZOrder(-1);

		float phase = color::g_phase;

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
