#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

#include <gay/utils.hpp>

#include <Geode/Geode.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/TextInput.hpp>

#include <matjson.hpp>

using gay::ext::RustStr;

namespace gay {
	enum class SettingType {
		Bool,
		Float,
		Int,
		ColorList,
		Choice
	};

	enum class DisplayMode : int {
		Gay = 0,
		Straight = 1,
		Granular = 2,
		_BuiltinCount = 3
	};

	struct ModeText {
		std::string_view name;
		std::string_view desc;
	};

	struct SectionInfo {
		std::string_view key;
		std::array<std::string_view, 3> titles;
		std::string_view tooltip = "";
	};

	struct SettingDisplayInfo {
		std::string_view key;
		SettingType type;
		std::array<ModeText, 3> mode_text;
		std::string_view section_key = "";
		double min_val = 0.0;
		double max_val = 10.0;
		std::span<const std::string_view> choices = {};
		bool granular_only = false;
	};

	ModeText get_mode_text(const SettingDisplayInfo& info, int mode_idx);
	std::string_view get_section_title(const SectionInfo& section, int mode_idx);
} // namespace gay

class GwtSettingsPopup;

class GwtSectionHeader: public cocos2d::CCNode {
	using cocos2d::CCNode::init;

  protected:
	RustStr m_header_title;
	RustStr m_header_tooltip;

	bool init(std::string_view title, std::string_view tooltip, float width);
	void on_info_btn(cocos2d::CCObject*);

  public:
	static GwtSectionHeader* create(std::string_view title, std::string_view tooltip, float width);
};

class GwtSettingRow: public cocos2d::CCNode {
	using cocos2d::CCNode::init;

  protected:
	RustStr m_setting_key;
	gay::SettingType m_setting_type = gay::SettingType::Bool;
	std::function<void()> m_on_change;
	CCMenuItemSpriteExtra* m_reset_btn = nullptr;
	GwtSettingsPopup* m_popup = nullptr;

	float m_control_left_x = 0.f;

	geode::TextInput* m_num_input = nullptr;
	CCMenuItemSpriteExtra* m_arrow_left_btn = nullptr;
	CCMenuItemSpriteExtra* m_big_arrow_left_btn = nullptr;
	CCMenuItemSpriteExtra* m_arrow_right_btn = nullptr;
	CCMenuItemSpriteExtra* m_big_arrow_right_btn = nullptr;
	cocos2d::CCSprite* m_arrow_left_spr = nullptr;
	cocos2d::CCSprite* m_big_arrow_left_spr = nullptr;
	cocos2d::CCSprite* m_arrow_right_spr = nullptr;
	cocos2d::CCSprite* m_big_arrow_right_spr = nullptr;

	double m_min_val = 0.0;
	double m_max_val = 10.0;

	std::vector<std::string_view> m_choices;
	int32_t m_choice_index = 0;
	cocos2d::CCLabelBMFont* m_choice_label = nullptr;

	bool init(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width, GwtSettingsPopup* popup);
	void build_bool_control(float width);
	void build_numeric_control(const gay::SettingDisplayInfo& info, float width);
	void build_colorlist_control(float width);
	void build_choice_control(const gay::SettingDisplayInfo& info, float width);

	void update_state(cocos2d::CCNode* invoker = nullptr);
	void update_choice_label();
	void update_reset_btn();

	void on_bool_toggle(cocos2d::CCObject*);
	void on_info_btn(cocos2d::CCObject*);
	void on_view_colors(cocos2d::CCObject*);

	void on_arrow(cocos2d::CCObject*);
	void on_choice_dropdown(cocos2d::CCObject*);
	void on_reset(cocos2d::CCObject*);

	float value_to_slider(double value) const;
	double value_from_slider(float t) const;

  public:
	static GwtSettingRow*
	create(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width, GwtSettingsPopup* popup);

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
	CCMenuItemSpriteExtra* m_apply_btn = nullptr;
	CCMenuItemSpriteExtra* m_close_btn = nullptr;
	CCMenuItemSpriteExtra* m_discard_btn = nullptr;

	std::unordered_map<std::string, matjson::Value> m_pending;

	static constexpr float WIDTH = 440.f;
	static constexpr float HEIGHT = 315.f;
	static constexpr float LIST_W = 400.f;
	static constexpr float SEARCH_H = 30.f;
	static constexpr float LIST_H = 210.f;

	static float s_saved_scroll_y;

	bool setup();
	void rebuild_list(bool preserve_scroll = false);
	void on_search_changed(const std::string& query);
	void on_clear_search(cocos2d::CCObject*);
	void on_apply(cocos2d::CCObject*);
	void on_discard(cocos2d::CCObject*);
	void onClose(cocos2d::CCObject*) override;
	void update_apply_btn();
	void maybe_show_granular_warning();

  public:
	static GwtSettingsPopup* create();

	void request_rebuild();
	bool has_pending(const std::string& key) const;
	matjson::Value get_pending(const std::string& key) const;
	void set_pending(const std::string& key, matjson::Value value);
	void clear_pending(const std::string& key);

	template<typename T>
	T get_effective_value(const std::string& key) const {
		auto it = m_pending.find(key);
		if (it != m_pending.end()) {
			auto r = it->second.template as<T>();
			if (r.isOk()) {
				return r.unwrap();
			}
		}
		return geode::Mod::get()->getSettingValue<T>(key);
	}
};
