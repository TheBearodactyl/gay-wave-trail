#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/cocos/include/ccTypes.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/ui/Notification.hpp>

using namespace geode::prelude;

// thanks to shadowforce78
void HSVtoRGBbutDifferentIGuess(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if (0 <= fHPrime && fHPrime < 1) {
        fR = fC;
        fG = fX;
        fB = 0;
    } else if (1 <= fHPrime && fHPrime < 2) {
        fR = fX;
        fG = fC;
        fB = 0;
    } else if (2 <= fHPrime && fHPrime < 3) {
        fR = 0;
        fG = fC;
        fB = fX;
    } else if (3 <= fHPrime && fHPrime < 4) {
        fR = 0;
        fG = fX;
        fB = fC;
    } else if (4 <= fHPrime && fHPrime < 5) {
        fR = fX;
        fG = 0;
        fB = fC;
    } else if (5 <= fHPrime && fHPrime < 6) {
        fR = fC;
        fG = 0;
        fB = fX;
    } else {
        fR = 0;
        fG = 0;
        fB = 0;
    }

    fR += fM;
    fG += fM;
    fB += fM;
}

// thanks to shadowforce78
float g = 0;

cocos2d::_ccColor3B getRainbow(float offset, float saturation) {
    float R;
    float G;
    float B;

    float hue = fmod(g + offset, 360);
    float sat = saturation / 100.0;
    float vc = 1;
    HSVtoRGBbutDifferentIGuess(R, G, B, hue, sat, vc);

    cocos2d::_ccColor3B out;
    out.r = R * 255;
    out.g = G * 255;
    out.b = B * 255;
    return out;
}

// thanks to shadowforce78
class $modify(PlayerObject) {
    void flashPlayer(float p0, float p1, cocos2d::ccColor3B mainColor, cocos2d::ccColor3B secondColor){
        // neutralised kappa
    }
};

class $modify(PlayLayer) {
    void postUpdate(float p0) {
        float speed = Mod::get()->getSettingValue<double>("speed");
	float saturation = Mod::get()->getSettingValue<double>("saturation");
	    
        if (g >= 360) {
    	    g = 0;
	} else {
    	    g += speed / 10;
	}

    	auto rainbowColor = getRainbow(0, saturation);
	auto rainbowColor2 = getRainbow(180, saturation);
	auto rainbowColor3 = getRainbow(90, saturation);
	bool enable = Mod::get()->getSettingValue<bool>("enable");
	bool isQolmodLoaded = Loader::get()->isModLoaded("TheSillyDoggo.Cheats");
	Notification* notif = Notification::create("GWT Won't work with QOLMod!", NotificationIcon::Warning, 3.0f);

	if (enable == true) {
   	    if (m_player1->m_waveTrail) {
   	        m_player1->m_waveTrail->setColor(rainbowColor);
            }

	    if (m_player2->m_waveTrail) {
                m_player2->m_waveTrail->setColor(rainbowColor2);
	    }
	
            if (isQolmodLoaded == true) {
                notif->show();
	    )
	}

	PlayLayer::postUpdate(p0);
    }
};
