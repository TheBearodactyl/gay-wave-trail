#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <chrono>
#include "Utils.hpp"

using namespace geode::prelude;
using namespace std::chrono;

float g = 0;

void hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
	float fC = fV * fS;
	float fHPrime = fmod(fH / 60.0, 6);
	float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
	float fM = fV - fC;

	switch (static_cast<int>(fHPrime))
	{
	case 0:
		fR = fC;
		fG = fX;
		fB = 0;
		break;
	case 1:
		fR = fX;
		fG = fC;
		fB = 0;
		break;
	case 2:
		fR = 0;
		fG = fC;
		fB = fX;
		break;
	case 3:
		fR = 0;
		fG = fX;
		fB = fC;
		break;
	case 4:
		fR = fX;
		fG = 0;
		fB = fC;
		break;
	case 5:
		fR = fC;
		fG = 0;
		fB = fX;
		break;
	default:
		fR = 0;
		fG = 0;
		fB = 0;
		break;
	}

	fR += fM;
	fG += fM;
	fB += fM;
}

cocos2d::_ccColor3B get_rainbow(float offset, float saturation) {
	float R, G, B;

	float hue = fmod(g + offset, 360);
	float sat = saturation / 100.0;
	float vc = 1;
	hsv_to_rgb(R, G, B, hue, sat, vc);

	cocos2d::_ccColor3B out;
	out.r = R * 255;
	out.g = G * 255;
	out.b = B * 255;
	return out;
}

class $modify(PlayerObject) {
	void flashPlayer(float p0, float p1, cocos2d::ccColor3B mainColor, cocos2d::ccColor3B secondColor) {
		// uwu
	}
};

class $modify(PlayLayer) {
	CCSprite *progressBar;
	CCLabelBMFont *percentLabel;

	void postUpdate(float p0) {
		float speed = Mod::get()->getSettingValue<double>("speed");
		float saturation = Mod::get() -> getSettingValue<double>("saturation");

		if (g >= 360) {
			g = 0;
		} else {
			g += speed / 10;
		}

		auto rainbowColor = get_rainbow(0, saturation);
		auto rainbowColor2 = get_rainbow(180, saturation);
		auto rainbowColor3 = get_rainbow(90, saturation);
		bool enable = Mod::get()->getSettingValue<bool>("enable");

		if (enable) {
			if (m_player1->m_waveTrail) {
				m_player1->m_waveTrail->setColor(rainbowColor);
			}

			if (m_player2->m_waveTrail) {
				m_player2->m_waveTrail->setColor(rainbowColor2);
			}
		}
	}
};