#ifndef COLOR_POPUP_HPP
#define COLOR_POPUP_HPP

#pragma once

#include <Geode/ui/Popup.hpp>

#include <gay/types/color.hpp>

using namespace geode::prelude;

class ColorListPopup: public Popup<std::vector<ColorEntry>, std::function<void(std::vector<ColorEntry>)>>, geode::ColorPickPopupDelegate {
  protected:
	std::vector<ColorEntry> m_entries;
	std::function<void(std::vector<ColorEntry>)> m_callback;
	ScrollLayer* m_list;

	bool setup(std::vector<ColorEntry>, std::function<void(std::vector<ColorEntry>)>) override;

	void create_list();

	void onAdd(CCObject*);
	void onClose(CCObject*) override;
	void updateColor(ccColor4B const& color) override;

  public:
	static ColorListPopup* create(std::vector<ColorEntry>, const std::function<void(std::vector<ColorEntry>)>&);
};

#endif
