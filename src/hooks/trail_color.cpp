#include <Geode/modify/PlayLayer.hpp>

#include <gay/color.hpp>
#include <gay/settings.hpp>

#include <cmath>

using namespace geode::prelude;
namespace settings = gay::settings;
namespace color = gay::color;

static float s_gradient_phase = 0.0f;

struct ColorApplyHook: Modify<ColorApplyHook, PlayLayer> {
	float calc_phase(float phase, float speed, float dt) {
		return std::fmod(phase + (speed * 60.0f * dt), 360.0f);
	}

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!settings::is_enabled()) {
			return;
		}

		float speed = settings::get_float("speed");
		bool use_gaydient = settings::get<bool>("enable-gaydient");
		bool separate = settings::get<bool>("separate-player-colors");

		color::g_phase = calc_phase(color::g_phase, speed, dt);
		s_gradient_phase = calc_phase(s_gradient_phase, speed, dt);

		if (separate && use_gaydient) {
			apply_separate_gradient(dt, speed);
		} else {
			apply_shared_colors(use_gaydient);
		}

		apply_trail_brightness();
	}

	void apply_separate_gradient(float dt, float speed) {
		auto p1_colors = settings::get<gay::ColorList>("gaydient-colors");
		auto p2_colors = settings::get<gay::ColorList>("p2-gaydient-colors");
		bool smooth = settings::get<bool>("smooth-colors");

		if (this->m_player1 && this->m_player1->m_isDart && this->m_player1->m_waveTrail) {
			auto c = color::get_gradient(s_gradient_phase, 0.0f, smooth, p1_colors);
			this->m_player1->m_waveTrail->setColor(c);
		}

		if (this->m_player2 && this->m_player2->m_isDart && this->m_player2->m_waveTrail) {
			auto c = color::get_gradient(s_gradient_phase, 0.0f, smooth, p2_colors);
			this->m_player2->m_waveTrail->setColor(c);
		}
	}

	void apply_shared_colors(bool use_gaydient) {
		float saturation = settings::get_float("saturation");
		bool smooth = settings::get<bool>("smooth-colors");
		auto gradient_colors = settings::get<gay::ColorList>("gaydient-colors");

		auto color_p1 =
			use_gaydient ? color::get_gradient(s_gradient_phase, 0.0f, smooth, gradient_colors) : color::get_rainbow(0.0f, saturation);

		auto color_p2 = use_gaydient ? color::get_gradient(s_gradient_phase + 180.0f, 0.0f, smooth, gradient_colors)
									 : color::get_rainbow(180.0f, saturation);

		if (this->m_player1 && this->m_player1->m_isDart && this->m_player1->m_waveTrail) {
			this->m_player1->m_waveTrail->setColor(color_p1);
		}

		if (this->m_player2 && this->m_player2->m_isDart && this->m_player2->m_waveTrail) {
			this->m_player2->m_waveTrail->setColor(color_p2);
		}
	}

	void apply_trail_brightness() {
		float brightness = settings::get_float("trail-brightness");
		if (brightness == 1.0f) {
			return;
		}

		if (this->m_player1 && this->m_player1->m_isDart && this->m_player1->m_waveTrail) {
			auto c = this->m_player1->m_waveTrail->getColor();
			this->m_player1->m_waveTrail->setColor(color::apply_brightness(c, brightness));
		}

		if (this->m_player2 && this->m_player2->m_isDart && this->m_player2->m_waveTrail) {
			auto c = this->m_player2->m_waveTrail->getColor();
			this->m_player2->m_waveTrail->setColor(color::apply_brightness(c, brightness));
		}
	}
};

struct MusicSyncHook: Modify<MusicSyncHook, PlayLayer> {
	struct Fields {
		float last_time = 0.0f;
	};

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!settings::check<settings::CheckMode::All>("sync-to-music", "enabled")) {
			return;
		}

		float speed = settings::get_float("speed");
		float level_time = static_cast<float>(this->m_currentTime);

		color::g_phase = std::fmod(level_time * speed * 60.0f, 360.0f);
		s_gradient_phase = color::g_phase;

		m_fields->last_time = level_time;
	}

	void resetLevel() override {
		PlayLayer::resetLevel();
		m_fields->last_time = 0.0f;
	}
};
