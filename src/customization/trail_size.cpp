#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct TrailSizePlayLayer: Modify<TrailSizePlayLayer, PlayLayer> {
	void postUpdate(float dt) {
		PlayLayer::postUpdate(dt);

		this->m_player1->m_waveTrail->m_pulseSize = gay::settings::get<float>("trail-size");
		this->m_player1->m_waveTrail->m_waveSize = gay::settings::get<float>("pulse-size");

		this->m_player2->m_waveTrail->m_pulseSize = gay::settings::get<float>("trail-size");
		this->m_player2->m_waveTrail->m_waveSize = gay::settings::get<float>("pulse-size");
	}
};
