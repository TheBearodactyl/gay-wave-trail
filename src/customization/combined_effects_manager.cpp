#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct CombinedEffectsManager: Modify<CombinedEffectsManager, PlayLayer> {
	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!gay::settings::get_mod_enabled()) {
			return;
		}

		ensure_fx_compat();
	}

	void ensure_fx_compat() {
		bool has_particles = gay::settings::get<bool>("trail-particles");
		bool has_afterimage = gay::settings::get<bool>("trail-afterimage");
		bool has_segments = gay::settings::get<bool>("trail-segments");
		bool has_wave = gay::settings::get<bool>("trail-wave-effect");

		if (has_segments && has_wave) {
			if (this->m_player1 && this->m_player1->m_waveTrail) {
				this->m_player1->m_waveTrail->setOpacity(200);
			}
			if (this->m_player2 && this->m_player2->m_waveTrail) {
				this->m_player2->m_waveTrail->setOpacity(200);
			}
		}

		if (has_particles && has_afterimage) {
			auto particle_density = gay::settings::get<int>("particle-density");
			if (particle_density > 20) {
				log::warn("High particle density with afterimage enabled may impact performance");
			}
		}
	}
};
