#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include <gay/settings.hpp>

using namespace geode::prelude;
namespace settings = gay::settings;

struct TrailSizeHook: Modify<TrailSizeHook, PlayLayer> {
	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!settings::is_enabled()) {
			return;
		}

		float wave_size = settings::get_float("trail-size");
		float pulse_size = settings::get_float("pulse-size");

		if (this->m_player1 && this->m_player1->m_waveTrail) {
			this->m_player1->m_waveTrail->m_waveSize = wave_size;
			this->m_player1->m_waveTrail->m_pulseSize = pulse_size;
		}

		if (this->m_player2 && this->m_player2->m_waveTrail) {
			this->m_player2->m_waveTrail->m_waveSize = wave_size;
			this->m_player2->m_waveTrail->m_pulseSize = pulse_size;
		}
	}
};

struct TrailTiltHook: Modify<TrailTiltHook, HardStreak> {
	void addPoint(CCPoint point) {
		if (settings::is_enabled() && settings::get<bool>("tilt-enabled")) {
			point.x += settings::get_float("tilt-x-offset");
			point.y += settings::get_float("tilt-y-offset");
		}

		HardStreak::addPoint(point);
	}
};

struct ThicknessHook: Modify<ThicknessHook, PlayLayer> {
	struct Fields {
		CCPoint p1_last_pos = CCPointZero;
		CCPoint p2_last_pos = CCPointZero;
		float p1_speed = 0.0f;
		float p2_speed = 0.0f;
	};

	void postUpdate(float dt) override {
		PlayLayer::postUpdate(dt);

		if (!settings::is_enabled() || !settings::get<bool>("trail-thickness-variation") || settings::get<bool>("disable-pulse")) {
			return;
		}

		if (this->m_player1) {
			update_player_thickness(this->m_player1, m_fields->p1_last_pos, m_fields->p1_speed, dt);
		}
		if (this->m_player2) {
			update_player_thickness(this->m_player2, m_fields->p2_last_pos, m_fields->p2_speed, dt);
		}
	}

	void update_player_thickness(PlayerObject* player, CCPoint& last_pos, float& speed, float dt) {
		if (!player || !player->m_isDart || !player->m_waveTrail || dt <= 0.0f) {
			return;
		}

		CCPoint current_pos = player->getPosition();
		float distance = ccpDistance(current_pos, last_pos);
		speed = distance / dt;
		last_pos = current_pos;

		float base_size = settings::get_float("trail-size");
		float multiplier = settings::get_float("thickness-multiplier");
		float speed_factor = std::clamp(speed / 500.0f, 0.2f, 2.0f);
		float new_size = base_size * (1.0f + (speed_factor - 1.0f) * multiplier);

		player->m_waveTrail->m_pulseSize = new_size;
	}

	void resetLevel() override {
		PlayLayer::resetLevel();
		m_fields->p1_last_pos = CCPointZero;
		m_fields->p2_last_pos = CCPointZero;
		m_fields->p1_speed = 0.0f;
		m_fields->p2_speed = 0.0f;
	}
};

struct SegmentState {
	float accumulated_distance = 0.0f;
	CCPoint last_point = CCPointZero;
	bool in_gap = false;
	float gap_distance = 0.0f;
};

static SegmentState s_segment_p1;
static SegmentState s_segment_p2;

struct SegmentsPlayHook: Modify<SegmentsPlayHook, PlayLayer> {
	void resetLevel() {
		s_segment_p1 = {};
		s_segment_p2 = {};
		PlayLayer::resetLevel();
	}

	void onQuit() {
		s_segment_p1 = {};
		s_segment_p2 = {};
		PlayLayer::onQuit();
	}
};

struct SegmentsStreakHook: Modify<SegmentsStreakHook, HardStreak> {
	void addPoint(CCPoint point) {
		if (!settings::is_enabled() || !settings::get<bool>("trail-segments")) {
			HardStreak::addPoint(point);
			return;
		}

		auto* play_layer = GameManager::sharedState()->m_playLayer;
		if (!play_layer) {
			HardStreak::addPoint(point);
			return;
		}

		SegmentState* state = nullptr;
		if (play_layer->m_player1 && play_layer->m_player1->m_waveTrail && this == play_layer->m_player1->m_waveTrail) {
			state = &s_segment_p1;
		} else if (play_layer->m_player2 && play_layer->m_player2->m_waveTrail && this == play_layer->m_player2->m_waveTrail) {
			state = &s_segment_p2;
		}

		if (!state) {
			HardStreak::addPoint(point);
			return;
		}

		float seg_length = settings::get_float("segment-length");
		float seg_gap = settings::get_float("segment-gap");

		if (state->last_point.equals(CCPointZero)) {
			state->last_point = point;
			HardStreak::addPoint(point);
			return;
		}

		float distance = ccpDistance(point, state->last_point);
		state->accumulated_distance += distance;

		if (state->in_gap) {
			state->gap_distance += distance;
			if (state->gap_distance >= seg_gap) {
				state->in_gap = false;
				state->gap_distance = 0.0f;
				state->accumulated_distance = 0.0f;
				this->stopStroke();
				this->resumeStroke();
			}
		} else {
			if (state->accumulated_distance >= seg_length) {
				state->in_gap = true;
				state->gap_distance = 0.0f;
				state->accumulated_distance = 0.0f;
			}
			HardStreak::addPoint(point);
		}

		state->last_point = point;
	}
};

struct DashingHook: Modify<DashingHook, PlayerObject> {
	void update(float dt) {
		PlayerObject::update(dt);

		if (this->m_isDart && this->m_isDashing && settings::get<bool>("trail-dashing")) {
			this->setRotation(this->getRotation() + this->m_dashAngle * dt);
		}
	}
};
