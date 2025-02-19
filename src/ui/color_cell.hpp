#ifndef COLOR_CELL_HPP
#define COLOR_CELL_HPP

#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include "../types/color.hpp"

using namespace geode::prelude;
using namespace cocos2d;

class ColorListPopup;
class ColorCell : public cocos2d::CCNode {
protected:
    ColorListPopup* m_parentPopup;
    ColorEntry* m_entry;
    std::function<void()> m_onDelete;
    CCMenuItemToggler* m_toggleButton;

    bool init(ColorListPopup* parent_popup, ColorEntry* entry, std::function<void()> on_delete, CCSize const& size);
    void update_ui();

public:
    static ColorCell* create(ColorListPopup* parent_popup, ColorEntry* entry, std::function<void()> on_delete, CCSize const& size);
    void on_toggle(CCObject*);
    void on_delete(CCObject*);
};

#endif