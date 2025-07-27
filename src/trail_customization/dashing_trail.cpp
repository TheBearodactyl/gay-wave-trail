#include "../settings/gay_settings.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace gay::settings;

struct DashingWave : Modify<DashingWave, PlayerObject> {
  void update(float dt) {
    PlayerObject::update(dt);

    if (this->m_isDart && this->m_isDashing && gay::settings::get<bool>("trail-dashing")) {
      this->setRotation(this->getRotation() + 25 * dt);
    }
  }
};
