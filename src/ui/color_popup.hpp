#ifndef COLOR_POPUP_HPP
#define COLOR_POPUP_HPP

#pragma once

#include "../types/color.hpp"

#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class ColorListPopup : public Popup<std::vector<ColorEntry>, std::function<void(std::vector<ColorEntry>)>>, geode::ColorPickPopupDelegate {
protected:
    std::vector<ColorEntry> m_entries;
    std::function<void(std::vector<ColorEntry>)> m_callback;
    ScrollLayer *m_list;

    bool setup(std::vector<ColorEntry>, std::function<void(std::vector<ColorEntry>)>) override;

    void create_list();

    void onAdd(CCObject *); // ew, camelCase :/
    void onClose(CCObject *) override; // again, ew.
    void updateColor(ccColor3B const& color);

public:
    static ColorListPopup *create(
            std::vector<ColorEntry>,
            const std::function<void(std::vector<ColorEntry>)>&
    );
};

#endif