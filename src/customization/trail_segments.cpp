#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <gay/settings/gay_settings.hpp>

using namespace geode::prelude;

struct SegmentState {
	float accumulated_distance = 0.0f;
	CCPoint last_point = CCPointZero;
	bool in_gap = false;
	float gap_distance = 0.0f;
};

static SegmentState s_segment_p1;
static SegmentState s_segment_p2;

struct TrailSegmentsPlayLayer: Modify<TrailSegmentsPlayLayer, PlayLayer> {
	void resetLevel() {
		s_segment_p1 = SegmentState();
		s_segment_p2 = SegmentState();

		PlayLayer::resetLevel();
	}

	void onQuit() {
		s_segment_p1 = SegmentState();
		s_segment_p2 = SegmentState();

		PlayLayer::onQuit();
	}
};

struct TrailSegmentsHardStreak: Modify<TrailSegmentsHardStreak, HardStreak> {
	void updateStroke(float delta) {
		auto* play_layer = GameManager::sharedState()->m_playLayer;

		if (play_layer) {
			if (play_layer->m_player1 && this == play_layer->m_player1->m_waveTrail) {
			} else if (play_layer->m_player2 && this == play_layer->m_player2->m_waveTrail) {
			}
		}

		HardStreak::updateStroke(delta);
	}

	void addPoint(cocos2d::CCPoint point_coords) {
		if (!gay::settings::get<bool>("trail-segments") || !gay::settings::get_mod_enabled()) {
			HardStreak::addPoint(point_coords);
			return;
		}

		auto* play_layer = GameManager::sharedState()->m_playLayer;
		if (!play_layer) {
			HardStreak::addPoint(point_coords);
			return;
		}

		SegmentState* state = nullptr;

		if (play_layer->m_player1 && this == play_layer->m_player1->m_waveTrail) {
			state = &s_segment_p1;
		} else if (play_layer->m_player2 && this == play_layer->m_player2->m_waveTrail) {
			state = &s_segment_p2;
		}

		if (!state) {
			HardStreak::addPoint(point_coords);
			return;
		}

		float segment_length = gay::settings::get_float("segment-length");
		float segment_gap = gay::settings::get_float("segment-gap");

		if (state->last_point.equals(CCPointZero)) {
			state->last_point = point_coords;
			HardStreak::addPoint(point_coords);
			return;
		}

		float distance = ccpDistance(point_coords, state->last_point);
		state->accumulated_distance += distance;

		if (state->in_gap) {
			state->gap_distance += distance;

			if (state->gap_distance >= segment_gap) {
				state->in_gap = false;
				state->gap_distance = 0.0f;
				state->accumulated_distance = 0.0f;

				this->stopStroke();
				this->resumeStroke();
			}
		} else {
			if (state->accumulated_distance >= segment_length) {
				state->in_gap = true;
				state->gap_distance = 0.0f;
				state->accumulated_distance = 0.0f;
			}

			HardStreak::addPoint(point_coords);
		}

		state->last_point = point_coords;
	}
};
