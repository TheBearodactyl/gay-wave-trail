#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;
using namespace gay::settings;

struct DashingWave: Modify<DashingWave, PlayerObject> {
	void update(float dt) {
		PlayerObject::update(dt);

		if (this->m_isDart and this->m_isDashing and gay::settings::get<bool>("trail-dashing")) {
			this->setRotation(this->getRotation() + 25 * dt);
		}
	}
};
