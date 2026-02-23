#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <utility>

#include <gay/color.hpp>
#include <gay/setting_defs.hpp>
#include <gay/settings.hpp>
#include <gay/ui/color_cell.hpp>
#include <gay/ui/color_popup.hpp>
#include <gay/ui/settings_popup.hpp>

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/TextInput.hpp>

#include <fmt/core.h>

using namespace geode::prelude;
using namespace gay::color;
namespace rp = gay::color::rose_pine;
using ST = gay::SettingType;

static constexpr ccColor4B rp_c4(ccColor3B c, uint8_t a) {
	return {c.r, c.g, c.b, a};
}

bool GwtSectionHeader::init(std::string_view title, std::string_view tooltip, float width) {
	if (!CCNode::init()) {
		return false;
	}

	m_header_title = std::string(title);
	m_header_tooltip = std::string(tooltip);

	constexpr float HEIGHT = 24.0f;
	constexpr float INFO_BTN_RESERVED = 20.0f;

	this->setContentSize({width, HEIGHT});
	this->setAnchorPoint({0.0f, 0.0f});

	float info_reserved = tooltip.empty() ? 0.0f : INFO_BTN_RESERVED;

	auto* label = CCLabelBMFont::create(m_header_title.c_str(), "bigFont.fnt");
	label->setColor(rp::gold);
	label->setScale(0.48f);
	label->setAnchorPoint({0.5f, 0.5f});

	float usable_center = (width - info_reserved) / 2.0f;
	label->setPosition({usable_center, HEIGHT / 2.0f});
	this->addChild(label);

	float label_half = label->getScaledContentWidth() / 2.0f + 6.0f;
	float left_rule_w = usable_center - label_half - 4.0f;

	if (left_rule_w > 2.0f) {
		auto* left_rule = CCLayerColor::create(rp_c4(rp::subtle, 60), left_rule_w, 1.5f);
		left_rule->setPosition({4.0f, HEIGHT / 2.0f});
		left_rule->ignoreAnchorPointForPosition(true);
		this->addChild(left_rule);
	}

	float right_rule_start = usable_center + label_half;
	float right_rule_end = width - info_reserved - 4.0f;
	float right_rule_w = right_rule_end - right_rule_start;
	if (right_rule_w > 2.0f) {
		auto* right_rule = CCLayerColor::create(rp_c4(rp::subtle, 60), right_rule_w, 1.5f);
		right_rule->setPosition({right_rule_start, HEIGHT / 2.0f});
		right_rule->ignoreAnchorPointForPosition(true);
		this->addChild(right_rule);
	}

	if (!tooltip.empty()) {
		auto* info_spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		info_spr->setScale(0.38f);

		auto* info_btn = CCMenuItemSpriteExtra::create(info_spr, this, menu_selector(GwtSectionHeader::on_info_btn));

		auto* info_menu = CCMenu::create();
		info_menu->setPosition({0.0f, 0.0f});
		info_menu->setContentSize({INFO_BTN_RESERVED, HEIGHT});
		info_btn->setPosition({width - INFO_BTN_RESERVED / 2.0f, HEIGHT / 2.0f});
		info_menu->addChild(info_btn);
		this->addChild(info_menu);
	}

	return true;
}

void GwtSectionHeader::on_info_btn(CCObject*) {
	FLAlertLayer::create(nullptr, m_header_title.c_str(), m_header_tooltip.c_str(), "OK", nullptr, 300.0f)->show();
}

GwtSectionHeader* GwtSectionHeader::create(std::string_view title, std::string_view tooltip, float width) {
	auto* ret = new GwtSectionHeader();
	if (ret->init(title, tooltip, width)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

static constexpr float ROW_HEIGHT_NORMAL = 30.0f;
static constexpr float INFO_BTN_WIDTH = 20.0f;
static int s_row_index = 0;
static constexpr float CTRL_W = 140.0f;
static constexpr float CTRL_RIGHT_PAD = 6.0f;

bool GwtSettingRow::init(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width) {
	if (!CCNode::init()) {
		return false;
	}

	m_setting_key = std::string(info.key);
	m_setting_type = info.type;

	float row_h = ROW_HEIGHT_NORMAL;

	this->setContentSize({width, row_h});
	this->setAnchorPoint({0.0f, 0.0f});

	{
		uint8_t alpha = s_row_index++ % 2 ? 255 : 200;
		auto* bg_layer = CCLayerColor::create(rp_c4(rp::surface, alpha), width, row_h);
		bg_layer->setPosition({0.0f, 0.0f});
		bg_layer->ignoreAnchorPointForPosition(true);
		this->addChild(bg_layer, -1);
	}

	bool is_gay = (mode == gay::DisplayMode::Gay);
	std::string_view name = is_gay ? info.gay_name : info.straight_name;
	std::string_view desc = is_gay ? info.gay_desc : info.straight_desc;

	float ctrl_row_y = row_h / 2.0f;

	constexpr float LABEL_LEFT_PAD = 6.0f;
	float ctrl_left = width - CTRL_W - CTRL_RIGHT_PAD;
	float info_reserved = desc.empty() ? 0.0f : (INFO_BTN_WIDTH + 2.0f);
	float usable_label = ctrl_left - LABEL_LEFT_PAD - info_reserved - 4.0f;

	auto* label = CCLabelBMFont::create(std::string(name).c_str(), "bigFont.fnt");
	label->setScale(0.3f);
	label->setColor({rp::text.r, rp::text.g, rp::text.b});
	label->setAnchorPoint({0.0f, 0.5f});
	label->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
	label->setPosition({LABEL_LEFT_PAD, ctrl_row_y});
	if (usable_label > 0.0f && label->getScaledContentWidth() > usable_label) {
		label->setScale(0.3f * usable_label / label->getScaledContentWidth());
	}
	this->addChild(label);

	if (!desc.empty()) {
		auto* info_spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		info_spr->setScale(0.4f);

		auto* info_btn = CCMenuItemSpriteExtra::create(info_spr, this, menu_selector(GwtSettingRow::on_info_btn));
		info_btn->setID("info-btn");

		auto* arr = CCArray::create();
		arr->addObject(CCString::create(std::string(name)));
		arr->addObject(CCString::create(std::string(desc)));
		info_btn->setUserObject(arr);

		float info_x = LABEL_LEFT_PAD + label->getScaledContentWidth() + INFO_BTN_WIDTH / 2.0f + 2.0f;

		auto* info_menu = CCMenu::create();
		info_menu->setPosition({0.0f, 0.0f});
		info_menu->setContentSize({width, ROW_HEIGHT_NORMAL});
		info_btn->setPosition({info_x, ROW_HEIGHT_NORMAL / 2.0f});
		info_menu->addChild(info_btn);
		this->addChild(info_menu);
	}

	if (info.type == ST::Bool) {
		bool current = Mod::get()->getSettingValue<bool>(m_setting_key);

		auto* on_spr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
		auto* off_spr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
		on_spr->setScale(0.6f);
		off_spr->setScale(0.6f);

		auto* toggler = CCMenuItemToggler::create(off_spr, on_spr, this, menu_selector(GwtSettingRow::on_bool_toggle));
		toggler->toggle(current);

		auto* toggle_menu = CCMenu::create();
		toggle_menu->setContentSize({30.0f, row_h});
		toggle_menu->setPosition({width - 30.0f, 0.0f});
		toggler->setPosition({15.0f, row_h / 2.0f});
		toggle_menu->addChild(toggler);
		this->addChild(toggle_menu);
	} else if (info.type == ST::Float || info.type == ST::Int) {
		std::string cur_str;
		if (info.type == ST::Int) {
			int64_t v = Mod::get()->getSettingValue<int64_t>(m_setting_key);
			cur_str = std::to_string(v);
		} else {
			double v = Mod::get()->getSettingValue<double>(m_setting_key);
			cur_str = fmt::format("{:.2f}", v);
		}

		constexpr float INPUT_W = 62.0f;
		constexpr float INPUT_SCALE = 0.65f;
		constexpr float RIGHT_PAD = 10.0f;
		constexpr float CTL_W = CTRL_W - RIGHT_PAD;

		auto* ctl_node = CCNode::create();
		ctl_node->setContentSize({CTL_W, ROW_HEIGHT_NORMAL});
		ctl_node->setAnchorPoint({1.0f, 0.0f});
		ctl_node->setPosition({width - RIGHT_PAD, 0.0f});
		this->addChild(ctl_node);

		auto make_arrow = [&](int tag) -> CCMenuItemSpriteExtra* {
			auto* spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
			bool is_big = (std::abs(tag) == 10);
			spr->setScale(is_big ? 0.45f : 0.35f);
			if (tag > 0) {
				spr->setFlipX(true);
			}
			auto* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GwtSettingRow::on_step));
			btn->setTag(tag);
			return btn;
		};

		auto* minus_big = make_arrow(-10);
		auto* minus_btn = make_arrow(-1);
		auto* plus_btn = make_arrow(+1);
		auto* plus_big = make_arrow(+10);

		auto* input = TextInput::create(INPUT_W, "Value");
		input->setScale(INPUT_SCALE);
		input->setCommonFilter(info.type == ST::Int ? CommonFilter::Int : CommonFilter::Float);
		input->setString(cur_str, false);
		m_num_input = input;

		float local_y = ROW_HEIGHT_NORMAL / 2.0f;
		float cx = CTL_W / 2.0f;

		constexpr float A_GAP = 14.0f;
		constexpr float AA_GAP = 13.0f;
		constexpr float IN_HALF = INPUT_W * INPUT_SCALE / 2.0f;

		auto* arr_menu = CCMenu::create();
		arr_menu->setContentSize({CTL_W, ROW_HEIGHT_NORMAL});
		arr_menu->setPosition({0.0f, 0.0f});
		arr_menu->ignoreAnchorPointForPosition(true);

		input->setPosition({cx, local_y});
		minus_btn->setPosition({cx - IN_HALF - A_GAP, local_y});
		minus_big->setPosition({cx - IN_HALF - A_GAP - AA_GAP, local_y});
		plus_btn->setPosition({cx + IN_HALF + A_GAP, local_y});
		plus_big->setPosition({cx + IN_HALF + A_GAP + AA_GAP, local_y});

		arr_menu->addChild(minus_big);
		arr_menu->addChild(minus_btn);
		arr_menu->addChild(input);
		arr_menu->addChild(plus_btn);
		arr_menu->addChild(plus_big);
		ctl_node->addChild(arr_menu);

		std::string key_copy = m_setting_key;
		bool is_int = (info.type == ST::Int);
		input->setCallback([key_copy, is_int](const std::string& str) {
			if (is_int) {
				auto r = geode::utils::numFromString<int64_t>(str);
				if (r.isOk()) {
					Mod::get()->setSettingValue<int64_t>(key_copy, r.unwrap());
				}
			} else {
				auto r = geode::utils::numFromString<double>(str);
				if (r.isOk()) {
					Mod::get()->setSettingValue<double>(key_copy, r.unwrap());
				}
			}
		});
	} else if (info.type == ST::ColorList) {
		auto* view_spr = ButtonSprite::create("View");
		view_spr->setScale(0.6f);

		auto* view_btn = CCMenuItemSpriteExtra::create(view_spr, this, menu_selector(GwtSettingRow::on_view_colors));
		auto* color_menu = CCMenu::create();
		color_menu->setContentSize({70.0f, row_h});
		color_menu->setPosition({width - 74.0f, 0.0f});
		view_btn->setPosition({35.0f, row_h / 2.0f});
		color_menu->addChild(view_btn);
		this->addChild(color_menu);
	} else if (info.type == ST::Choice) {
		m_choices.assign(info.choices.begin(), info.choices.end());

		int64_t stored = Mod::get()->getSettingValue<int64_t>(m_setting_key);
		m_choice_index = static_cast<int>(std::clamp<int64_t>(stored, 0, static_cast<int64_t>(m_choices.size()) - 1));

		constexpr float DROPDOWN_W = 140.0f;
		constexpr float DROPDOWN_H = 22.0f;

		auto* btn_bg = geode::NineSlice::create("square02b_small.png");
		btn_bg->setID("choice-button-bg");
		btn_bg->setContentSize({DROPDOWN_W, DROPDOWN_H});
		btn_bg->setColor({rp::base.r, rp::base.g, rp::base.b});
		btn_bg->setOpacity(120);

		m_choice_label = CCLabelBMFont::create("", "bigFont.fnt");
		m_choice_label->setScale(0.28f);

		m_choice_label->setColor({rp::text.r, rp::text.g, rp::text.b});
		m_choice_label->setAnchorPoint({0.0f, 0.5f});
		m_choice_label->setPosition({8.0f, DROPDOWN_H / 2.0f});
		btn_bg->addChild(m_choice_label, 1);

		auto* arrow_spr = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
		arrow_spr->setScale(0.6f);


		arrow_spr->setColor({rp::subtle.r, rp::subtle.g, rp::subtle.b});
		arrow_spr->setPosition({DROPDOWN_W - 12.0f, DROPDOWN_H / 2.0f});
		btn_bg->addChild(arrow_spr, 1);

		auto* dropdown_btn = CCMenuItemSpriteExtra::create(btn_bg, this, menu_selector(GwtSettingRow::on_choice_dropdown));

		auto* dropdown_menu = CCMenu::create();
		dropdown_menu->setContentSize({DROPDOWN_W + 10.0f, row_h});
		dropdown_menu->setPosition({width - DROPDOWN_W - 14.0f, 0.0f});
		dropdown_btn->setPosition({(DROPDOWN_W + 10.0f) / 2.0f, row_h / 2.0f});
		dropdown_menu->addChild(dropdown_btn);
		this->addChild(dropdown_menu);

		update_choice_label();
	}

	return true;
}

void GwtSettingRow::update_choice_label() {
	if (!m_choice_label || m_choices.empty()) {
		return;
	}

	m_choice_label->setString(std::string(m_choices[m_choice_index]).c_str());

	constexpr float MAX_W = 105.0f;
	float base = 0.28f;
	m_choice_label->setScale(base);
	if (m_choice_label->getScaledContentWidth() > MAX_W) {
		m_choice_label->setScale(base * MAX_W / m_choice_label->getScaledContentWidth());
	}
}

void GwtSettingRow::on_bool_toggle(CCObject* sender) {
	auto* toggler = static_cast<CCMenuItemToggler*>(sender);
	Mod::get()->setSettingValue<bool>(m_setting_key, !toggler->isToggled());
	if (m_on_change) {
		m_on_change();
	}
}

void GwtSettingRow::on_info_btn(CCObject* sender) {
	auto* btn = static_cast<CCMenuItemSpriteExtra*>(sender);
	auto* arr = static_cast<CCArray*>(btn->getUserObject());
	if (!arr || arr->count() < 2) {
		return;
	}
	auto* title_str = static_cast<CCString*>(arr->objectAtIndex(0));
	auto* desc_str = static_cast<CCString*>(arr->objectAtIndex(1));
	FLAlertLayer::create(nullptr, title_str->getCString(), desc_str->getCString(), "OK", nullptr, 300.0f)->show();
}

void GwtSettingRow::on_view_colors(CCObject*) {
	auto colors = Mod::get()->getSettingValue<gay::ColorList>(m_setting_key);
	std::string key_copy = m_setting_key;
	ColorListPopup::create(colors.colors, [key_copy](std::vector<gay::ColorEntry> new_colors) {
		Mod::get()->setSettingValue<gay::ColorList>(key_copy, gay::ColorList {std::move(new_colors)});
	})->show();
}

void GwtSettingRow::on_step(CCObject* sender) {
	if (!m_num_input) {
		return;
	}

	int raw_tag = sender->getTag();
	int direction = (raw_tag > 0) ? 1 : -1;
	bool is_big = (std::abs(raw_tag) == 10);
	double base_step = (m_setting_type == ST::Int) ? 1.0 : 0.1;
	double step = is_big ? base_step * 10.0 : base_step;
	double delta = static_cast<double>(direction) * step;

	if (m_setting_type == ST::Int) {
		int64_t next = Mod::get()->getSettingValue<int64_t>(m_setting_key) + static_cast<int64_t>(delta);
		Mod::get()->setSettingValue<int64_t>(m_setting_key, next);
		m_num_input->setString(fmt::format("{}", next), false);
	} else {
		double next = Mod::get()->getSettingValue<double>(m_setting_key) + delta;
		next = std::round(next / base_step) * base_step;
		Mod::get()->setSettingValue<double>(m_setting_key, next);
		m_num_input->setString(fmt::format("{:.2f}", next), false);
	}
}

class GwtChoiceDropdown: public CCLayerColor {
  protected:
	std::vector<std::string_view> m_options;
	int m_selected = 0;
	std::function<void(int)> m_callback;

	bool init(std::vector<std::string_view> options, int selected, std::function<void(int)> cb, const CCPoint& anchor_pos) {
		if (!CCLayerColor::initWithColor({0, 0, 0, 0})) {
			return false;
		}

		m_options = std::move(options);
		m_selected = selected;
		m_callback = std::move(cb);

		auto win_size = CCDirector::sharedDirector()->getWinSize();
		this->setContentSize(win_size);
		this->setTouchEnabled(true);
		this->setTouchPriority(-510);

		constexpr float ITEM_H = 24.0f;
		constexpr float ITEM_W = 180.0f;
		constexpr float PAD = 4.0f;
		float panel_w = ITEM_W + PAD * 2.0f;
		float list_h = static_cast<float>(m_options.size()) * ITEM_H + PAD * 2.0f;

		auto* panel = NineSlice::create("square02b_001.png");
		panel->setContentSize({panel_w, list_h});

		panel->setColor({rp::surface.r, rp::surface.g, rp::surface.b});
		panel->setOpacity(230);

		float panel_top = anchor_pos.y;
		float panel_x = anchor_pos.x;

		if (panel_top - list_h < 0.0f) {
			panel_top = anchor_pos.y + list_h;
		}

		panel->setAnchorPoint({1.0f, 1.0f});
		panel->setPosition({panel_x, panel_top});
		this->addChild(panel, 1);

		auto* menu = CCMenu::create();
		menu->setPosition({0.0f, 0.0f});
		menu->setContentSize(panel->getContentSize());

		for (int i = 0; i < static_cast<int>(m_options.size()); i++) {
			auto* item_bg = NineSlice::create("square02b_small.png");
			item_bg->setContentSize({ITEM_W, ITEM_H - 2.0f});


			item_bg->setColor(
				i == m_selected ? ccColor3B {rp::pine.r, rp::pine.g, rp::pine.b} : ccColor3B {rp::overlay.r, rp::overlay.g, rp::overlay.b}
			);
			item_bg->setOpacity(200);

			auto* lbl = CCLabelBMFont::create(std::string(m_options[i]).c_str(), "bigFont.fnt");
			lbl->setScale(0.28f);
			lbl->setAnchorPoint({0.0f, 0.5f});

			lbl->setColor({rp::text.r, rp::text.g, rp::text.b});
			lbl->setPosition({8.0f, (ITEM_H - 2.0f) / 2.0f});

			float max_lbl_w = ITEM_W - 16.0f;
			if (lbl->getScaledContentWidth() > max_lbl_w) {
				lbl->setScale(0.28f * max_lbl_w / lbl->getScaledContentWidth());
			}

			item_bg->addChild(lbl, 1);

			auto* btn = CCMenuItemSpriteExtra::create(item_bg, this, menu_selector(GwtChoiceDropdown::on_select));
			btn->setTag(i);

			float item_y = list_h - PAD - ITEM_H * static_cast<float>(i) - (ITEM_H - 2.0f) / 2.0f;
			btn->setPosition({PAD + ITEM_W / 2.0f, item_y});
			menu->addChild(btn);
		}

		menu->setTouchPriority(-511);
		panel->addChild(menu);

		return true;
	}

	void on_select(CCObject* sender) {
		int idx = sender->getTag();
		if (m_callback) {
			m_callback(idx);
		}
		this->removeFromParent();
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
		this->removeFromParent();
		return true;
	}

	void registerWithTouchDispatcher() override {
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -510, true);
	}

  public:
	static GwtChoiceDropdown*
	create(std::vector<std::string_view> options, int selected, std::function<void(int)> cb, const CCPoint& anchor_pos) {
		auto* ret = new GwtChoiceDropdown();
		if (ret->init(std::move(options), selected, std::move(cb), anchor_pos)) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}
};

void GwtSettingRow::on_choice_dropdown(CCObject* sender) {
	if (m_choices.empty()) {
		return;
	}

	auto* btn = static_cast<CCNode*>(sender);
	auto btn_world = btn->convertToWorldSpace({btn->getContentSize().width, 0.0f});

	std::string key_copy = m_setting_key;
	auto* dropdown = GwtChoiceDropdown::create(
		m_choices,
		m_choice_index,
		[this, key_copy](int idx) {
			m_choice_index = idx;
			Mod::get()->setSettingValue<int64_t>(key_copy, static_cast<int64_t>(m_choice_index));
			update_choice_label();
			if (m_on_change) {
				m_on_change();
			}
		},
		btn_world
	);

	CCDirector::sharedDirector()->getRunningScene()->addChild(dropdown, 9999);
}

GwtSettingRow* GwtSettingRow::create(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width) {
	auto* ret = new GwtSettingRow();
	if (ret->init(info, mode, width)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool GwtSettingsPopup::setup() {
	auto* full_bg = CCLayerColor::create(rp_c4(rp::surface, 255));
	full_bg->setContentSize(m_mainLayer->getContentSize());
	full_bg->ignoreAnchorPointForPosition(true);
	full_bg->setPosition({0.f, 0.f});
	full_bg->setID("full-bg-layer-color");
	m_mainLayer->addChild(full_bg, -2);

	this->m_bgSprite->setOpacity(0);

	m_list_bg = CCLayerColor::create(rp_c4(rp::muted, 255));
	m_list_bg->setContentSize({LIST_W, LIST_H + SEARCH_H});
	m_list_bg->setID("list-bg-layer-color");
	m_list_bg->ignoreAnchorPointForPosition(false);
	m_mainLayer->addChildAtPosition(m_list_bg, Anchor::Center);

	auto* search_menu = CCMenu::create();
	search_menu->setContentSize({LIST_W, SEARCH_H});

	m_search = TextInput::create((LIST_W - 15.f) / 0.7f - 40.f, "Search settings...");
	m_search->setTextAlign(TextInputAlign::Left);
	m_search->setScale(0.7f);
	m_search->setCallback([this](const std::string& q) { this->on_search_changed(q); });
	m_search->setID("search-input");
	search_menu->addChildAtPosition(m_search, Anchor::Left, ccp(16.0f, 0.f), ccp(0.f, 0.5f));

	auto* clear_spr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
	clear_spr->setScale(0.45f);
	m_clear_btn = CCMenuItemSpriteExtra::create(clear_spr, this, menu_selector(GwtSettingsPopup::on_clear_search));
	m_clear_btn->setID("clear-search-btn");
	m_clear_btn->setEnabled(false);

	clear_spr->setColor({rp::muted.r, rp::muted.g, rp::muted.b});
	clear_spr->setOpacity(90);
	search_menu->addChildAtPosition(m_clear_btn, Anchor::Right, ccp(-20.f, 0.f));

	m_list_bg->addChildAtPosition(search_menu, Anchor::Top, ccp(0.f, 0.f), ccp(0.5f, 1.f));

	this->rebuild_list();

	auto* scrollbar = Scrollbar::create(m_list);
	m_mainLayer->addChildAtPosition(scrollbar, Anchor::Center, ccp(LIST_W / 2.f + 10.f, -SEARCH_H / 2.f));

	// auto* borders = ListBorders::create();
	// borders->setContentSize({LIST_W + 1.f, LIST_H + SEARCH_H + 1.f});
	// m_mainLayer->addChildAtPosition(borders, Anchor::Center);

	for (auto* child : m_mainLayer->getChildrenExt()) {
		if (typeinfo_cast<CCLabelBMFont*>(child)) {
			static_cast<CCNode*>(child)->setZOrder(1);
		}
	}

	handleTouchPriority(this);
	return true;
}

void GwtSettingsPopup::on_search_changed(const std::string& query) {
	this->rebuild_list();

	if (m_clear_btn) {
		bool active = !query.empty();
		auto* clr_spr = static_cast<CCSprite*>(m_clear_btn->getNormalImage());
		m_clear_btn->setEnabled(active);

		clr_spr->setColor(active ? ccColor3B {rp::text.r, rp::text.g, rp::text.b} : ccColor3B {rp::muted.r, rp::muted.g, rp::muted.b});
		clr_spr->setOpacity(active ? 255 : 90);
	}
}

void GwtSettingsPopup::rebuild_list() {
	auto mode = static_cast<gay::DisplayMode>(Mod::get()->getSettingValue<int64_t>("display-mode"));
	std::string query = m_search ? std::string(m_search->getString()) : std::string {};
	bool has_query = !query.empty();

	auto to_lower = [](std::string s) {
		for (auto& c : s) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		return s;
	};

	std::string lower_query = to_lower(query);

	auto matches = [&](const gay::SettingDisplayInfo& info) -> bool {
		if (!has_query) {
			return true;
		}
		bool name_hit = to_lower(std::string(mode == gay::DisplayMode::Gay ? info.gay_name : info.straight_name)).find(lower_query) !=
						std::string::npos;
		bool key_hit = to_lower(std::string(info.key)).find(lower_query) != std::string::npos;
		return name_hit || key_hit;
	};

	if (!m_list) {
		m_list = ScrollLayer::create({LIST_W, LIST_H});
		m_list->m_contentLayer->setLayout(
			ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End)->setAutoGrowAxis(LIST_H)->setGap(0.f)
		);
		m_list_bg->addChildAtPosition(m_list, Anchor::BottomLeft);
	}

	m_list->m_contentLayer->removeAllChildren();
	s_row_index = 0;

	std::string_view current_section_straight;

	for (const auto& info : SETTING_DISPLAY_INFO) {
		if (has_query) {
			if (!matches(info)) {
				continue;
			}
		} else {
			bool starts_new_section = !info.section_straight.empty() && info.section_straight != current_section_straight;

			if (starts_new_section) {
				current_section_straight = info.section_straight;

				std::string_view header_title =
					(mode == gay::DisplayMode::Gay && !info.section_gay.empty()) ? info.section_gay : info.section_straight;

				if (auto* header = GwtSectionHeader::create(header_title, info.section_tooltip, LIST_W)) {
					m_list->m_contentLayer->addChild(header);
				}
			}
		}

		auto* row = GwtSettingRow::create(info, mode, LIST_W);
		if (!row) {
			continue;
		}

		if (info.key == "display-mode") {
			row->set_on_change([this] { this->rebuild_list(); });
		}

		m_list->m_contentLayer->addChild(row);
	}

	m_list->m_contentLayer->updateLayout();
	m_list->scrollToTop();

	handleTouchPriority(this);
}

void GwtSettingsPopup::on_clear_search(CCObject*) {
	if (m_search) {
		m_search->setString("");
	}
	this->on_search_changed("");
}

GwtSettingsPopup* GwtSettingsPopup::create() {
	auto* ret = new GwtSettingsPopup();
	if (ret->init(WIDTH, HEIGHT, "GJ_square01.png")) {
		ret->setup();
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool ColorCell::init(
	ColorListPopup* parent,
	gay::ColorEntry* entry,
	std::function<void()> on_delete,
	std::function<void()> on_color_change,
	const CCSize& size
) {
	if (!CCNode::init()) {
		return false;
	}

	static constexpr float HORIZONTAL_PADDING = 2.6f;

	m_parent_popup = parent;
	m_entry = entry;
	m_on_delete = std::move(on_delete);
	m_on_color_change = std::move(on_color_change);

	if (!m_entry) {
		return false;
	}

	auto entry_color = gay::color::hex_to_rgb(m_entry->hex);

	this->setContentSize(size);
	this->setAnchorPoint({0.5f, 0.5f});

	m_background = geode::NineSlice::create("square02b_001.png");
	m_background->setID("color-cell-bg");
	m_background->setColor({entry_color.r, entry_color.g, entry_color.b});
	m_background->setOpacity(100);
	m_background->setScale(0.3f);
	m_background->setContentSize(size / m_background->getScale());
	this->addChildAtPosition(m_background, Anchor::Center);

	float buttons_width = 0.f;

	m_toggle_btn = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ColorCell::on_toggle), .6f);
	m_toggle_btn->setAnchorPoint({0.5f, 0.5f});
	buttons_width += m_toggle_btn->getContentSize().width;

	auto* btns_menu = CCMenu::create();
	btns_menu->addChild(m_toggle_btn);
	btns_menu->setAnchorPoint({1.0f, 0.5f});
	btns_menu->setContentSize({50.f, 30.f});

	m_color_preview = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
	m_color_preview->setScale(0.5f);
	m_color_preview->setColor({entry_color.r, entry_color.g, entry_color.b});

	auto* color_picker_btn = CCMenuItemSpriteExtra::create(m_color_preview, this, menu_selector(ColorCell::on_color_picker));
	color_picker_btn->setID("color-picker-button");
	btns_menu->addChild(color_picker_btn);
	buttons_width += color_picker_btn->getContentSize().width * 0.5f;

	if (m_entry->user_added) {
		auto* sprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
		sprite->setScale(0.7f);
		auto* delete_btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ColorCell::on_delete_btn));
		delete_btn->setID("delete-button");
		btns_menu->addChild(delete_btn);
		buttons_width += delete_btn->getContentSize().width * 0.7f;
	}

	btns_menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Even));
	btns_menu->updateLayout();
	btns_menu->setID("button-menu");

	m_text_input = TextInput::create(size.width - HORIZONTAL_PADDING * 2 - buttons_width, "Color Hex Code", "FiraCodeNerdMono.fnt"_spr);
	m_text_input->setScale(1.f);
	m_text_input->setCommonFilter(CommonFilter::Any);
	m_text_input->setMaxCharCount(300);
	m_text_input->setString(m_entry->hex, false);
	m_text_input->setTextAlign(TextInputAlign::Left);
	m_text_input->setID("hex-input");
	m_text_input->setCallback([this](const std::string& str) {
		if (!m_entry) {
			return;
		}
		m_entry->hex = str;
		this->update_color_preview();
		this->update_background_color();
		if (m_on_color_change) {
			m_on_color_change();
		}
	});

	auto* menu = CCMenu::create();
	menu->addChild(m_text_input);
	menu->addChild(btns_menu);
	menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Between));
	menu->setID("menu");
	menu->setAnchorPoint({0.5f, 0.5f});
	menu->setContentSize({size.width - HORIZONTAL_PADDING * 2, size.height});
	menu->updateLayout();

	this->addChildAtPosition(menu, Anchor::Center);
	this->update_ui();

	return true;
}

void ColorCell::update_ui() {
	m_toggle_btn->toggle(m_entry->enabled);
}

void ColorCell::update_color_preview() {
	if (!m_color_preview || !m_entry) {
		return;
	}
	auto color = gay::color::hex_to_rgb(m_entry->hex);
	m_color_preview->setColor({color.r, color.g, color.b});
}

void ColorCell::update_background_color() {
	if (!m_background || !m_entry) {
		return;
	}
	auto color = gay::color::hex_to_rgb(m_entry->hex);
	m_background->setColor({color.r, color.g, color.b});
}

void ColorCell::update_text_input() {
	if (!m_text_input) {
		return;
	}
	m_text_input->setString(m_entry->hex, false);
}

void ColorCell::on_toggle(CCObject*) {
	if (!m_entry) {
		return;
	}
	m_entry->enabled = !m_entry->enabled;
	this->update_ui();
	m_toggle_btn->toggle(!m_toggle_btn->isToggled());
}

void ColorCell::on_delete_btn(CCObject*) {
	if (m_on_delete) {
		m_on_delete();
	}
}

void ColorCell::on_color_picker(CCObject*) {
	if (!m_entry) {
		return;
	}
	auto current_color = gay::color::hex_to_rgb(m_entry->hex);
	auto* popup = ColorPickPopup::create(current_color);
	popup->setCallback([this](const ccColor4B& color) {
		if (!m_entry) {
			return;
		}
		m_entry->hex = gay::color::rgb_to_hex(color);
		this->update_color_preview();
		this->update_background_color();
		this->update_text_input();
		if (m_on_color_change) {
			m_on_color_change();
		}
	});
	popup->show();
}

ColorCell* ColorCell::create(
	ColorListPopup* parent,
	gay::ColorEntry* entry,
	std::function<void()> on_delete,
	std::function<void()> on_color_change,
	const CCSize& size
) {
	auto* ret = new ColorCell();
	if (ret->init(parent, entry, std::move(on_delete), std::move(on_color_change), size)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool ColorListPopup::setup(std::vector<gay::ColorEntry> entries, std::function<void(std::vector<gay::ColorEntry>)> callback) {
	m_entries = std::move(entries);
	m_callback = std::move(callback);

	this->create_list();

	auto* spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	spr->setScale(0.7f);

	auto* add_btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ColorListPopup::on_add));
	add_btn->setAnchorPoint({0.5f, 0.5f});

	auto* menu = CCMenu::create();
	menu->addChild(add_btn);
	menu->setZOrder(1);

	m_mainLayer
		->addChildAtPosition(menu, Anchor::BottomRight, {-5.f - add_btn->getContentWidth() / 2.f, 8.f + add_btn->getContentHeight() / 2.f});

	return true;
}

void ColorListPopup::create_list() {
	auto size = m_mainLayer->getContentSize();
	constexpr float PADDING = 5.f;

	if (m_list) {
		m_list->removeFromParent();
	}

	m_list = ScrollLayer::create({size.width - PADDING * 2, size.height - PADDING * 2 - 4.f});
	m_list->m_contentLayer->setLayout(
		ColumnLayout::create()
			->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::End)
			->setAutoGrowAxis(size.height - PADDING * 2 - 4.f)
			->setGap(PADDING / 2)
	);
	m_list->setPosition({PADDING, PADDING + 2.f});

	for (size_t i = 0; i < m_entries.size(); i++) {
		auto* cell = ColorCell::create(
			this,
			&m_entries[i],
			[this, i] {
				if (i < m_entries.size()) {
					m_entries.erase(m_entries.begin() + static_cast<ptrdiff_t>(i));
					this->create_list();
				}
			},
			[this] {},
			{320.f - PADDING * 2, 35.f}
		);

		cell->setAnchorPoint({0.f, 0.f});
		m_list->m_contentLayer->addChild(cell);
	}

	auto* spacer = CCMenu::create();
	spacer->setContentSize({0.f, 36.f});
	m_list->m_contentLayer->addChild(spacer);
	m_list->m_contentLayer->updateLayout();

	m_mainLayer->addChild(m_list);
	handleTouchPriority(this);
}

void ColorListPopup::onClose(CCObject* sender) {
	m_callback(m_entries);
	Popup::onClose(sender);
}

void ColorListPopup::on_add(CCObject*) {
	m_entries.emplace_back("#", true, true);
	this->create_list();
}

ColorListPopup*
ColorListPopup::create(std::vector<gay::ColorEntry> colors, const std::function<void(std::vector<gay::ColorEntry>)>& callback) {
	auto* ret = new ColorListPopup();
	if (ret->init(320.f, 240.f, "GJ_square01.png")) {
		ret->setup(std::move(colors), callback);
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

class $modify(GwtPauseLayer, PauseLayer) {
	void on_gwt_settings(CCObject*) {
		GwtSettingsPopup::create()->show();
	}

	void customSetup() override {
		PauseLayer::customSetup();

		if (!gay::settings::get<bool>("enable-pause-btn")) {
			return;
		}

		auto* btn_sprite = CCSprite::create("button.png"_spr);
		btn_sprite->setScale(.25f);

		if (auto* better_pause = Loader::get()->getLoadedMod("tpdea.betterpause-Better")) {
			if (better_pause->getSettingValue<int64_t>("type-pause") == 1) {
				btn_sprite->setScale(.95f);
			}
		}

		auto* btn = CCMenuItemSpriteExtra::create(btn_sprite, this, menu_selector(GwtPauseLayer::on_gwt_settings));
		btn->setID("gwt-settings-button"_spr);

		if (auto* left_menu = getChildByIDRecursive("left-button-menu")) {
			left_menu->addChild(btn);
			left_menu->updateLayout();
		}
	}
};
