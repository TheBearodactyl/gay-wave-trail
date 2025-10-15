#ifndef COLOR_CELL_HPP
#define COLOR_CELL_HPP

#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>

#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/TextInput.hpp>

#include "../types/color.hpp"

using namespace geode::prelude;
using namespace cocos2d;

class ColorListPopup;

class ColorCell: public cocos2d::CCNode, public geode::ColorPickPopupDelegate {
  protected:
	ColorListPopup* m_parentPopup;
	ColorEntry* m_entry;
	std::function<void()> m_onDelete;
	std::function<void()> m_onColorChange;
	CCMenuItemToggler* m_toggleButton;
	CCSprite* m_colorPreview;
	CCScale9Sprite* m_background;
	TextInput* m_textInput;

	bool init(
		ColorListPopup* parent_popup,
		ColorEntry* entry,
		std::function<void()> on_delete,
		std::function<void()> on_color_change,
		CCSize const& size
	);
	void update_ui();
	void update_color_preview();
	void update_background_color();
	void update_text_input();

  public:
	static ColorCell* create(
		ColorListPopup* parent_popup,
		ColorEntry* entry,
		std::function<void()> on_delete,
		std::function<void()> on_color_change,
		CCSize const& size
	);

	void on_toggle(CCObject*);
	void on_delete(CCObject*);
	void on_color_picker(CCObject*);
	void updateColor(ccColor4B const& color) override;
};

#endif
