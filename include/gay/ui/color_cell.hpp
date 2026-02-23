#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>

#include <gay/types.hpp>

class ColorListPopup;

class ColorCell: public cocos2d::CCNode {
  protected:
	ColorListPopup* m_parent_popup;
	gay::ColorEntry* m_entry;
	std::function<void()> m_on_delete;
	std::function<void()> m_on_color_change;
	CCMenuItemToggler* m_toggle_btn;
	cocos2d::CCSprite* m_color_preview;
	geode::NineSlice* m_background;
	geode::TextInput* m_text_input;

	bool init(
		ColorListPopup* parent,
		gay::ColorEntry* entry,
		std::function<void()> on_delete,
		std::function<void()> on_color_change,
		const cocos2d::CCSize& size
	);

	void update_ui();
	void update_color_preview();
	void update_background_color();
	void update_text_input();

  public:
	static ColorCell* create(
		ColorListPopup* parent,
		gay::ColorEntry* entry,
		std::function<void()> on_delete,
		std::function<void()> on_color_change,
		const cocos2d::CCSize& size
	);

	void on_toggle(cocos2d::CCObject*);
	void on_delete_btn(cocos2d::CCObject*);
	void on_color_picker(cocos2d::CCObject*);
};
