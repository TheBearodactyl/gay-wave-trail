#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/settings/gay_settings.hpp>
#include <gay/utils/color_utils.hpp>

using namespace geode::prelude;

ccColor3B apply_brightness(const ccColor3B& color, float brightness) {
	return {
		static_cast<GLubyte>(std::min(255.0f, color.r * brightness)),
		static_cast<GLubyte>(std::min(255.0f, color.g * brightness)),
		static_cast<GLubyte>(std::min(255.0f, color.b * brightness))
	};
}

struct TrailBrightnessPlayLayer: Modify<TrailBrightnessPlayLayer, PlayLayer> {
	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);

		if (!gay::settings::get_mod_enabled()) {
			return;
		}

		auto brightness = gay::settings::get_float("trail-brightness");

		if (brightness != 1.0f) {
			if (this->m_player1 && this->m_player1->m_isDart) {
				auto current_color = this->m_player1->m_waveTrail->getColor();
				this->m_player1->m_waveTrail->setColor(apply_brightness(current_color, brightness));
			}

			if (this->m_player2 && this->m_player2->m_isDart) {
				auto current_color = this->m_player2->m_waveTrail->getColor();
				this->m_player2->m_waveTrail->setColor(apply_brightness(current_color, brightness));
			}
		}
	}
};
