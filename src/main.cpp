#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/GJRobotSprite.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "Utils.hpp"
#include <chrono>

using namespace geode::prelude;

// thanks to shadowforce78
void HSVtoRGB(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV)
{
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if (0 <= fHPrime && fHPrime < 1)
    {
        fR = fC;
        fG = fX;
        fB = 0;
    }
    else if (1 <= fHPrime && fHPrime < 2)
    {
        fR = fX;
        fG = fC;
        fB = 0;
    }
    else if (2 <= fHPrime && fHPrime < 3)
    {
        fR = 0;
        fG = fC;
        fB = fX;
    }
    else if (3 <= fHPrime && fHPrime < 4)
    {
        fR = 0;
        fG = fX;
        fB = fC;
    }
    else if (4 <= fHPrime && fHPrime < 5)
    {
        fR = fX;
        fG = 0;
        fB = fC;
    }
    else if (5 <= fHPrime && fHPrime < 6)
    {
        fR = fC;
        fG = 0;
        fB = fX;
    }
    else
    {
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
cocos2d::_ccColor3B getRainbow(float offset, float saturation)
{
    float R;
    float G;
    float B;

    float hue = fmod(g + offset, 360);
    float sat = saturation / 100.0;
    float vc = 1;
    HSVtoRGB(R, G, B, hue, sat, vc);

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

	auto rainbowColor = getRainbow(0, saturation);
	auto rainbowColor2 = getRainbow(180, saturation);
	auto rainbowColor3 = getRainbow(90, saturation);
	bool enable = Mod::get()->getSettingValue<bool>("enable");

	if (enable == true) {
	    if (m_fields->progressBar == nullptr || m_fields->percentLabel == nullptr) {
                for (size_t i = 0; i < this->getChildrenCount(); i++) {
                    auto obj = this->getChildren()->objectAtIndex(i);
                        if (Utils::getNodeName(obj) == "cocos2d::CCLabelBMFont" && m_fields->percentLabel == nullptr) {
                            auto labelTest = static_cast<CCLabelBMFont *>(obj);
                            if (strlen(labelTest->getString()) < 6) {
                                m_fields->percentLabel = labelTest;
                            } 
			} else if (Utils::getNodeName(obj) == "cocos2d::CCSprite" && m_fields->progressBar == nullptr) {
                            m_fields->progressBar = static_cast<CCSprite *>(obj);
                        }
                    }
			
		    if (m_player1->m_waveTrail) {
		        m_player1->m_waveTrail->setColor(rainbowColor);
		    }

		    if (m_player2->m_waveTrail) {
		        m_player2->m_waveTrail->setColor(rainbowColor2);
		    }
	      }
	}

	PlayLayer::postUpdate(p0);
	    
    }
};
