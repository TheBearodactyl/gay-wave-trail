#include "../settings/gay_settings.hpp"
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Disable the Pulse
class $modify(HardStreak) {
  void updateStroke(float dt) {
    auto wave_pulse_size = gay::settings::get_pulse_size();

    if (gay::settings::get_disable_pulse() && gay::settings::get_mod_enabled() && PlayLayer::get()) {
      double min = gay::settings::get_add_minimum_width() ? 0.5 : 0.0;

      this->m_pulseSize = static_cast<float>(min + gay::settings::get_pulse_size());
    }

    HardStreak::updateStroke(dt);
  }
};

// Disable the Trail itself
class $modify(PlayLayer) {
  void postUpdate(float dt) {
    if (gay::settings::get_mod_enabled() && gay::settings::get_disable_trail()) {
      this->m_player1->m_waveTrail->setVisible(false);
      this->m_player2->m_waveTrail->setVisible(false);
    }

    PlayLayer::postUpdate(dt);
  }
};