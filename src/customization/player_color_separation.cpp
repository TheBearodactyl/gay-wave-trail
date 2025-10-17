#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/customization/rainbow.hpp>
#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct PlayerColorSeparation: Modify<PlayerColorSeparation, PlayLayer> {
	struct Fields {
		float color_phase_p1 = 0.0f;
		float color_phase_p2 = 0.0f;
	};

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!gay::settings::get_mod_enabled()) {
			return;
		}

		bool separate_colors = gay::settings::get<bool>("separate-player-colors");
		float speed = gay::settings::get_float("speed");
		float p2_offset = gay::settings::get_float("rainbow-offset-p2");

		if (separate_colors && gay::settings::get<bool>("enable-gaydient")) {
			auto p1_gradient = gay::settings::get<ColorList>("gaydient-colors");
			auto p2_gradient = gay::settings::get<ColorList>("p2-gaydient-colors");

			m_fields->color_phase_p1 = fmodf(m_fields->color_phase_p1 + (speed * 60.0f * dt), 360.0f);
			m_fields->color_phase_p2 = fmodf(m_fields->color_phase_p2 + (speed * 60.0f * dt), 360.0f);

			if (this->m_player1 && this->m_player1->m_isDart) {
				auto gradient_color =
					RainbowTrail::get_gradient(m_fields->color_phase_p1, 0.0f, gay::settings::get<bool>("smooth-colors"), p1_gradient);
				this->m_player1->m_waveTrail->setColor(gradient_color);
			}

			if (this->m_player2 && this->m_player2->m_isDart) {
				auto gradient_color =
					RainbowTrail::get_gradient(m_fields->color_phase_p2, 0.0f, gay::settings::get<bool>("smooth-colors"), p2_gradient);
				this->m_player2->m_waveTrail->setColor(gradient_color);
			}
		} else if (!separate_colors && p2_offset != 180.0f) {
			if (this->m_player2 && this->m_player2->m_isDart && !gay::settings::get<bool>("enable-gaydient")) {
				auto rainbow_color = RainbowTrail::get_rainbow(p2_offset, gay::settings::get_float("saturation"));
				this->m_player2->m_waveTrail->setColor(rainbow_color);
			}
		}
	}
};
