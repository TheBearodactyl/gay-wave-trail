#include "Geode/modify/Modify.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

struct TrailPulseSize : Modify<TrailPulseSize, PlayLayer> {
  void postUpdate( float p0 ) {
    PlayLayer::postUpdate( p0 );

    float pulse_size = Mod::get()->getSettingValue<double>( "wave-pulse-size" );
    float trail_size = Mod::get()->getSettingValue<double>( "wave-trail-size" );

    this->m_player1->m_waveTrail->m_pulseSize = pulse_size;
    this->m_player1->m_waveTrail->m_waveSize = trail_size;
    this->m_player2->m_waveTrail->m_pulseSize = pulse_size;
    this->m_player2->m_waveTrail->m_waveSize = trail_size;
  }
};
