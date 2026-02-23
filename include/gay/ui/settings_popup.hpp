#pragma once

#include <span>
#include <string>
#include <string_view>

#include <Geode/Geode.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/TextInput.hpp>

namespace gay {
	enum class SettingType {
		Bool,
		Float,
		Int,
		ColorList,
		Choice,
	};

	struct SettingDisplayInfo {
		std::string_view key;
		std::string_view gay_name;
		std::string_view gay_desc;
		std::string_view straight_name;
		std::string_view straight_desc;
		SettingType type;
		std::string_view section_straight = "";
		std::string_view section_gay = "";
		std::string_view section_tooltip = "";

		double min_val = 0.0;
		double max_val = 10.0;

		std::span<const std::string_view> choices = {};
	};

	enum class DisplayMode : int {
		Gay = 0,
		Straight = 1,
	};
} // namespace gay

class GwtSectionHeader: public cocos2d::CCNode {
  protected:
	std::string m_header_title;
	std::string m_header_tooltip;

	bool init(std::string_view title, std::string_view tooltip, float width);
	void on_info_btn(cocos2d::CCObject*);

  public:
	static GwtSectionHeader* create(std::string_view title, std::string_view tooltip, float width);
};

class GwtSettingRow: public cocos2d::CCNode {
  protected:
	std::string m_setting_key;
	gay::SettingType m_setting_type = gay::SettingType::Bool;
	geode::TextInput* m_num_input = nullptr;
	std::vector<std::string_view> m_choices;
	int m_choice_index = 0;
	cocos2d::CCLabelBMFont* m_choice_label = nullptr;
	std::function<void()> m_on_change;

	bool init(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width);
	void on_bool_toggle(cocos2d::CCObject* sender);
	void on_info_btn(cocos2d::CCObject* sender);
	void on_view_colors(cocos2d::CCObject* sender);
	void on_step(cocos2d::CCObject* sender);
	void on_choice_dropdown(cocos2d::CCObject* sender);
	void update_choice_label();

  public:
	static GwtSettingRow* create(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width);

	void set_on_change(std::function<void()> cb) {
		m_on_change = std::move(cb);
	}
};

class GwtSettingsPopup: public geode::Popup {
  protected:
	geode::ScrollLayer* m_list = nullptr;
	geode::TextInput* m_search = nullptr;
	CCMenuItemSpriteExtra* m_clear_btn = nullptr;
	cocos2d::CCLayerColor* m_list_bg = nullptr;

	static constexpr float WIDTH = 440.f;
	static constexpr float HEIGHT = 280.f;
	static constexpr float LIST_W = 400.f;
	static constexpr float SEARCH_H = 30.f;
	static constexpr float LIST_H = 210.f;

	bool setup();
	void rebuild_list();
	void on_search_changed(const std::string& query);
	void on_clear_search(cocos2d::CCObject*);

  public:
	static GwtSettingsPopup* create();
};
