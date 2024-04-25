#include "rainbow_trail.hpp"

#include <cmath>

void RainbowTrail::hsv_to_rgb(float &fR, float &fG, float &fB, float &fH, float &fS, float &fV) {
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    int hue_segment = static_cast<int>(fHPrime);
    switch (hue_segment) {
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

float RainbowTrail::g = 0;

cocos2d::_ccColor3B RainbowTrail::get_rainbow(float offset, float saturation) {
    float R;
    float G;
    float B;

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
