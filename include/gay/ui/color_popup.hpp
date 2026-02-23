#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

#include <gay/types.hpp>

class ColorListPopup: public geode::Popup {
  protected:
	std::vector<gay::ColorEntry> m_entries;
	std::function<void(std::vector<gay::ColorEntry>)> m_callback;
	geode::ScrollLayer* m_list = nullptr;

	bool setup(std::vector<gay::ColorEntry>, std::function<void(std::vector<gay::ColorEntry>)>);
	void create_list();

	void on_add(cocos2d::CCObject*);
	void onClose(cocos2d::CCObject*) override;

  public:
	static ColorListPopup* create(std::vector<gay::ColorEntry>, const std::function<void(std::vector<gay::ColorEntry>)>&);
};
