

#include <gay/color.hpp>
#include <gay/constants.hpp>
#include <gay/setting_defs.hpp>
#include <gay/settings.hpp>
#include <gay/ui/color_popup.hpp>
#include <gay/ui/settings_popup.hpp>
#include <gay/utils.hpp>

#include <Geode/ui/TextInput.hpp>

#include <fmt/core.h>
#include <matjson.hpp>

using namespace geode::prelude;
namespace rp = gay::color::rose_pine;
using ST = gay::SettingType;
using gay::util::rp_c4;

class GwtChoiceDropdown: public CCLayerColor {
  protected:
	std::vector<std::string_view> m_options;
	int m_selected = 0;
	std::function<void(int)> m_callback;

	bool init(std::vector<std::string_view> opts, int sel, std::function<void(int)> cb, CCPoint anchor) {
		if (!CCLayerColor::initWithColor({0, 0, 0, 0})) {
			return false;
		}
		m_options = std::move(opts);
		m_selected = sel;
		m_callback = std::move(cb);

		auto win = CCDirector::sharedDirector()->getWinSize();
		setContentSize(win);
		setTouchEnabled(true);
		setTouchPriority(-510);

		constexpr float IH = 24.f, IW = 180.f, PAD = 4.f;
		float lh = (float)m_options.size() * IH + PAD * 2.f;

		auto* panel = NineSlice::create("square02b_001.png");
		panel->setContentSize({IW + PAD * 2, lh});
		panel->setColor({rp::surface.r, rp::surface.g, rp::surface.b});
		panel->setOpacity(230);
		panel->setAnchorPoint({1.f, 1.f});
		panel->setPosition({anchor.x, anchor.y < lh ? anchor.y + lh : anchor.y});
		addChild(panel, 1);

		auto* menu = CCMenu::create();
		menu->setPosition({0.f, 0.f});
		menu->setContentSize(panel->getContentSize());
		menu->setTouchPriority(-511);

		for (int i = 0; i < (int)m_options.size(); ++i) {
			auto* bg = NineSlice::create("square02b_small.png");
			bg->setContentSize({IW, IH - 2.f});
			bg->setColor(
				i == m_selected ? ccColor3B {rp::pine.r, rp::pine.g, rp::pine.b} : ccColor3B {rp::overlay.r, rp::overlay.g, rp::overlay.b}
			);
			bg->setOpacity(200);

			auto* lbl = CCLabelBMFont::create(std::string(m_options[i]).c_str(), "bigFont.fnt");
			lbl->setScale(0.28f);
			lbl->setAnchorPoint({0.f, 0.5f});
			lbl->setColor({rp::text.r, rp::text.g, rp::text.b});
			lbl->setPosition({8.f, (IH - 2.f) / 2.f});
			float maxW = IW - 16.f;
			if (lbl->getScaledContentWidth() > maxW) {
				lbl->setScale(0.28f * maxW / lbl->getScaledContentWidth());
			}
			bg->addChild(lbl, 1);

			auto* btn = CCMenuItemSpriteExtra::create(bg, this, menu_selector(GwtChoiceDropdown::on_select));
			btn->setTag(i);
			btn->setPosition({PAD + IW / 2.f, lh - PAD - IH * (float)i - (IH - 2.f) / 2.f});
			menu->addChild(btn);
		}
		panel->addChild(menu);
		return true;
	}

	void on_select(CCObject* s) {
		if (m_callback) {
			m_callback(s->getTag());
		}
		removeFromParent();
	}

	bool ccTouchBegan(CCTouch*, CCEvent*) override {
		removeFromParent();
		return true;
	}

	void registerWithTouchDispatcher() override {
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -510, true);
	}

  public:
	static GwtChoiceDropdown* create(std::vector<std::string_view> opts, int sel, std::function<void(int)> cb, CCPoint anchor) {
		auto* r = new GwtChoiceDropdown();
		if (r->init(std::move(opts), sel, std::move(cb), anchor)) {
			r->autorelease();
			return r;
		}
		delete r;
		return nullptr;
	}
};

static CCSprite* make_big_arrow_spr(bool point_right) {
	auto* container = CCSprite::create();
	container->setCascadeColorEnabled(true);
	container->setCascadeOpacityEnabled(true);

	auto* s1 = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	auto* s2 = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	if (point_right) {
		s1->setFlipX(true);
		s2->setFlipX(true);
	}

	container->setContentSize(s1->getContentSize() + ccp(20, 0));

	container->addChildAtPosition(s2, Anchor::Center, ccp(point_right ? 10.f : -10.f, 0.f));
	container->addChildAtPosition(s1, Anchor::Center, ccp(point_right ? -10.f : 10.f, 0.f));
	container->setScale(0.3f);
	return container;
}

static CCSprite* make_small_arrow_spr(bool point_right) {
	auto* s = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
	s->setFlipX(point_right);
	s->setScale(0.5f);
	return s;
}

void GwtSettingRow::build_bool_control(float width) {
	bool cur = m_popup ? m_popup->get_effective_value<bool>(m_setting_key) : Mod::get()->getSettingValue<bool>(m_setting_key);

	auto* toggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(GwtSettingRow::on_bool_toggle), 0.55f);

	toggler->m_onButton->setContentSize({25.f, 25.f});
	toggler->m_offButton->setContentSize({25.f, 25.f});
	toggler->m_onButton->getNormalImage()->setPosition(ccp(25.f, 25.f) / 2.f);
	toggler->m_offButton->getNormalImage()->setPosition(ccp(25.f, 25.f) / 2.f);
	toggler->setID("bool-toggler");
	toggler->toggle(cur);

	auto* menu = CCMenu::create();
	menu->setID("toggle-menu");
	menu->setContentSize({30.f, gay::ROW_HEIGHT_NORMAL});
	menu->setPosition({width - 30.f, 0.f});
	toggler->setPosition({15.f, gay::ROW_HEIGHT_NORMAL / 2.f});
	menu->addChild(toggler);
	addChild(menu);

	m_control_left_x = width - 30.f;
}

void GwtSettingRow::build_numeric_control(const gay::SettingDisplayInfo& info, float width) {
	m_min_val = info.min_val;
	m_max_val = info.max_val;
	bool is_int = (info.type == ST::Int);

	double small_step = is_int ? 1.0 : 0.1;
	double big_step = is_int ? 10.0 : 1.0;

	m_big_arrow_left_spr = make_big_arrow_spr(false);
	m_arrow_left_spr = make_small_arrow_spr(false);
	m_arrow_right_spr = make_small_arrow_spr(true);
	m_big_arrow_right_spr = make_big_arrow_spr(true);

	m_big_arrow_left_btn = CCMenuItemSpriteExtra::create(m_big_arrow_left_spr, this, menu_selector(GwtSettingRow::on_arrow));
	m_big_arrow_left_btn->setUserObject(ObjWrapper<double>::create(-big_step));
	m_big_arrow_left_btn->setID("big-arrow-left-btn");

	m_arrow_left_btn = CCMenuItemSpriteExtra::create(m_arrow_left_spr, this, menu_selector(GwtSettingRow::on_arrow));
	m_arrow_left_btn->setUserObject(ObjWrapper<double>::create(-small_step));
	m_arrow_left_btn->setID("arrow-left-btn");

	m_arrow_right_btn = CCMenuItemSpriteExtra::create(m_arrow_right_spr, this, menu_selector(GwtSettingRow::on_arrow));
	m_arrow_right_btn->setUserObject(ObjWrapper<double>::create(+small_step));
	m_arrow_right_btn->setID("arrow-right-btn");

	m_big_arrow_right_btn = CCMenuItemSpriteExtra::create(m_big_arrow_right_spr, this, menu_selector(GwtSettingRow::on_arrow));
	m_big_arrow_right_btn->setUserObject(ObjWrapper<double>::create(+big_step));
	m_big_arrow_right_btn->setID("big-arrow-right-btn");

	const float menu_w = gay::CTRL_W;
	m_num_input = TextInput::create(menu_w - 40.f, "Value");
	m_num_input->setID("numeric-input");
	m_num_input->setScale(0.7f);
	m_num_input->setCommonFilter(CommonFilter::Any);

	std::string key_cp = m_setting_key;
	GwtSettingsPopup* pp = m_popup;
	double min_v = m_min_val, max_v = m_max_val;
	m_num_input->setCallback([this, key_cp, is_int, pp, min_v, max_v](const std::string& s) {
		if (s.empty() || s == "-" || s == "-." || s == ".") {
			return;
		}

		if (is_int) {
			auto r = geode::utils::numFromString<int64_t>(s);
			if (!r.isOk()) {
				return;
			}
			int64_t val = std::clamp(r.unwrap(), (int64_t)min_v, (int64_t)max_v);
			if (pp) {
				pp->set_pending(key_cp, matjson::Value(val));
			} else {
				Mod::get()->setSettingValue<int64_t>(key_cp, val);
			}
		} else {
			auto r = geode::utils::numFromString<double>(s);
			if (!r.isOk()) {
				return;
			}
			double val = std::clamp(r.unwrap(), min_v, max_v);
			if (pp) {
				pp->set_pending(key_cp, matjson::Value(val));
			} else {
				Mod::get()->setSettingValue<double>(key_cp, val);
			}
		}
		update_state(m_num_input);
		update_reset_btn();
	});

	auto* menu = CCMenu::create();
	menu->setID("numeric-menu");
	menu->setContentSize({menu_w, gay::ROW_HEIGHT_NORMAL});
	menu->addChildAtPosition(m_big_arrow_left_btn, Anchor::Left, ccp(5.f, 0.f));
	menu->addChildAtPosition(m_arrow_left_btn, Anchor::Left, ccp(22.f, 0.f));
	menu->addChildAtPosition(m_num_input, Anchor::Center, ccp(0.f, 0.f));
	menu->addChildAtPosition(m_arrow_right_btn, Anchor::Right, ccp(-22.f, 0.f));
	menu->addChildAtPosition(m_big_arrow_right_btn, Anchor::Right, ccp(-5.f, 0.f));
	menu->ignoreAnchorPointForPosition(true);

	const float menu_x = width - gay::CTRL_RIGHT_PAD - menu_w;
	menu->setPosition({menu_x, gay::ROW_HEIGHT_NORMAL / 2.f - menu->getContentHeight() / 2.f});
	addChild(menu);

	m_control_left_x = menu_x;
	update_state(nullptr);
}

void GwtSettingRow::build_colorlist_control(float width) {
	auto* spr = ButtonSprite::create("View");
	spr->setScale(0.6f);
	auto* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GwtSettingRow::on_view_colors));
	btn->setID("view-colors-btn");

	auto* menu = CCMenu::create();
	menu->setID("color-menu");
	menu->setContentSize({70.f, gay::ROW_HEIGHT_NORMAL});
	menu->setPosition({width - 74.f, 0.f});
	btn->setPosition({45.f, gay::ROW_HEIGHT_NORMAL / 2.f});
	menu->addChild(btn);
	addChild(menu);

	m_control_left_x = width - 74.f;
}

void GwtSettingRow::build_choice_control(const gay::SettingDisplayInfo& info, float width) {
	m_choices.assign(info.choices.begin(), info.choices.end());
	int64_t stored = m_popup ? m_popup->get_effective_value<int64_t>(m_setting_key) : Mod::get()->getSettingValue<int64_t>(m_setting_key);
	m_choice_index = (int)std::clamp<int64_t>(stored, 0, (int64_t)m_choices.size() - 1);

	constexpr float DW = 140.f, DH = 22.f;
	auto* bg = NineSlice::create("square02b_small.png");
	bg->setContentSize({DW, DH});
	bg->setColor({rp::base.r, rp::base.g, rp::base.b});
	bg->setOpacity(120);

	m_choice_label = CCLabelBMFont::create("", "bigFont.fnt");
	m_choice_label->setScale(0.28f);
	m_choice_label->setColor({rp::text.r, rp::text.g, rp::text.b});
	m_choice_label->setAnchorPoint({0.f, 0.5f});
	m_choice_label->setPosition({8.f, DH / 2.f});
	bg->addChild(m_choice_label, 1);

	auto* arrow = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
	arrow->setScale(0.6f);
	arrow->setColor({rp::subtle.r, rp::subtle.g, rp::subtle.b});
	arrow->setPosition({DW - 12.f, DH / 2.f});
	bg->addChild(arrow, 1);

	auto* btn = CCMenuItemSpriteExtra::create(bg, this, menu_selector(GwtSettingRow::on_choice_dropdown));
	btn->setID("dropdown-btn");

	auto* menu = CCMenu::create();
	menu->setContentSize({DW + 10.f, gay::ROW_HEIGHT_NORMAL});
	menu->setPosition({width - DW - 14.f, 0.f});
	btn->setPosition({(DW + 10.f) / 2.f, gay::ROW_HEIGHT_NORMAL / 2.f});
	menu->addChild(btn);
	addChild(menu);

	m_control_left_x = width - DW - 14.f;
	update_choice_label();
}

bool GwtSettingRow::init(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width, GwtSettingsPopup* popup) {
	if (!CCNode::init()) {
		return false;
	}

	m_setting_key = std::string(info.key);
	m_setting_type = info.type;
	m_popup = popup;

	setContentSize({width, gay::ROW_HEIGHT_NORMAL});
	setAnchorPoint({0.f, 0.f});

	auto build_bg = [&] {
		float h = getContentSize().height;
		bool even = gay::s_row_index++ % 2 == 0;
		auto col = even ? rp::surface : rp::base;
		auto* bg = CCLayerColor::create(rp_c4(col, even ? 255 : 230), width, h);
		bg->setID("row-bg");
		bg->setPosition({0.f, 0.f});
		bg->ignoreAnchorPointForPosition(true);
		addChild(bg, -1);

		auto* border = CCLayerColor::create(rp_c4(rp::overlay, 80), width, 1.f);
		border->setID("row-border");
		border->setPosition({0.f, 0.f});
		border->ignoreAnchorPointForPosition(true);
		addChild(border, 0);
	};

	switch (info.type) {
		case ST::Bool:
			build_bool_control(width);
			break;
		case ST::Int:
		case ST::Float:
			build_numeric_control(info, width);
			break;
		case ST::ColorList:
			build_colorlist_control(width);
			break;
		case ST::Choice:
			build_choice_control(info, width);
			break;
	}

	build_bg();

	float h = getContentSize().height;
	float cy = h / 2.f;

	bool is_gay = (mode == gay::DisplayMode::Gay);
	std::string_view name = is_gay ? info.gay_name : info.straight_name;
	std::string_view desc = is_gay ? info.gay_desc : info.straight_desc;

	constexpr float LABEL_PAD = 6.f;
	float ctrl_left = width - gay::CTRL_W - gay::CTRL_RIGHT_PAD;
	float info_rsv = desc.empty() ? 0.f : (gay::INFO_BTN_WIDTH + 2.f);

	float usable_label = ctrl_left - LABEL_PAD - info_rsv - 4.f;

	auto* label = CCLabelBMFont::create(std::string(name).c_str(), "bigFont.fnt");
	label->setID("setting-label");
	label->setScale(0.3f);
	label->setColor({rp::text.r, rp::text.g, rp::text.b});
	label->setAnchorPoint({0.f, 0.5f});
	label->setAlignment(kCCTextAlignmentLeft);
	label->setPosition({LABEL_PAD, cy});
	if (usable_label > 0.f && label->getScaledContentWidth() > usable_label) {
		label->setScale(0.3f * usable_label / label->getScaledContentWidth());
	}
	addChild(label);

	if (!desc.empty()) {
		auto* spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		spr->setScale(0.4f);
		auto* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GwtSettingRow::on_info_btn));
		btn->setID("info-btn");
		auto* arr = CCArray::create();
		arr->addObject(CCString::create(std::string(name)));
		arr->addObject(CCString::create(std::string(desc)));
		btn->setUserObject(arr);

		auto* menu = CCMenu::create();
		menu->setPosition({0.f, 0.f});
		menu->setContentSize({width, h});
		btn->setPosition({LABEL_PAD + label->getScaledContentWidth() + gay::INFO_BTN_WIDTH / 2.f + 2.f, cy});
		menu->addChild(btn);
		addChild(menu);
	}

	{
		auto* spr = CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png");
		spr->setScale(0.4f);
		m_reset_btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GwtSettingRow::on_reset));
		m_reset_btn->setID("reset-btn");

		auto* menu = CCMenu::create();
		menu->setPosition({0.f, 0.f});
		menu->setContentSize({width, h});

		m_reset_btn->setPosition({m_control_left_x - gay::RESET_BTN_WIDTH / 2.f - 2.f, cy});
		menu->addChild(m_reset_btn);
		addChild(menu);
	}

	update_reset_btn();
	return true;
}

void GwtSettingRow::update_state(CCNode* invoker) {
	if (!m_arrow_left_btn) {
		return;
	}

	bool is_int = (m_setting_type == ST::Int);
	double cur =
		is_int
			? (double)(m_popup ? m_popup->get_effective_value<int64_t>(m_setting_key) : Mod::get()->getSettingValue<int64_t>(m_setting_key))
			: (m_popup ? m_popup->get_effective_value<double>(m_setting_key) : Mod::get()->getSettingValue<double>(m_setting_key));

	if (m_num_input && invoker != m_num_input) {
		double rounded = std::round(cur * 100000.0) / 100000.0;
		m_num_input->setString(is_int ? fmt::format("{}", (int64_t)rounded) : fmt::format("{:.2f}", rounded), false);
	}

	bool can_left = cur > m_min_val;
	bool can_right = cur < m_max_val;

	auto set_arrow = [](CCMenuItemSpriteExtra* btn, CCSprite* spr, bool en) {
		if (!btn || !spr) {
			return;
		}
		btn->setEnabled(en);
		spr->setOpacity(en ? 255 : 155);
		spr->setColor(en ? ccWHITE : ccGRAY);
	};
	set_arrow(m_arrow_left_btn, m_arrow_left_spr, can_left);
	set_arrow(m_big_arrow_left_btn, m_big_arrow_left_spr, can_left);
	set_arrow(m_arrow_right_btn, m_arrow_right_spr, can_right);
	set_arrow(m_big_arrow_right_btn, m_big_arrow_right_spr, can_right);
}

void GwtSettingRow::on_arrow(CCObject* sender) {
	if (!m_num_input) {
		return;
	}
	double delta = static_cast<ObjWrapper<double>*>(static_cast<CCNode*>(sender)->getUserObject())->getValue();

	bool is_int = (m_setting_type == ST::Int);
	if (is_int) {
		int64_t cur = m_popup ? m_popup->get_effective_value<int64_t>(m_setting_key) : Mod::get()->getSettingValue<int64_t>(m_setting_key);
		int64_t next = cur + (int64_t)delta;
		next = (int64_t)std::clamp((double)next, m_min_val, m_max_val);
		if (m_popup) {
			m_popup->set_pending(m_setting_key, matjson::Value(next));
		} else {
			Mod::get()->setSettingValue<int64_t>(m_setting_key, next);
		}
	} else {
		double cur = m_popup ? m_popup->get_effective_value<double>(m_setting_key) : Mod::get()->getSettingValue<double>(m_setting_key);
		double base = std::abs(delta) >= 1.0 ? 1.0 : 0.1;
		double next = std::clamp(std::round((cur + delta) / base) * base, m_min_val, m_max_val);
		if (m_popup) {
			m_popup->set_pending(m_setting_key, matjson::Value(next));
		} else {
			Mod::get()->setSettingValue<double>(m_setting_key, next);
		}
	}

	update_state(static_cast<CCNode*>(sender));
	update_reset_btn();
}

void GwtSettingRow::update_choice_label() {
	if (!m_choice_label || m_choices.empty()) {
		return;
	}
	m_choice_label->setString(std::string(m_choices[m_choice_index]).c_str());
	constexpr float MAX_W = 105.f, BASE = 0.28f;
	m_choice_label->setScale(BASE);
	if (m_choice_label->getScaledContentWidth() > MAX_W) {
		m_choice_label->setScale(BASE * MAX_W / m_choice_label->getScaledContentWidth());
	}
}

void GwtSettingRow::update_reset_btn() {
	if (!m_reset_btn) {
		return;
	}
	auto setting = Mod::get()->getSetting(m_setting_key);
	bool is_default = !setting || (m_popup && m_popup->has_pending(m_setting_key) ? false : setting->isDefaultValue());
	m_reset_btn->setVisible(!is_default);
}

void GwtSettingRow::on_bool_toggle(CCObject* sender) {
	bool nv = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
	if (m_popup) {
		m_popup->set_pending(m_setting_key, matjson::Value(nv));
	} else {
		Mod::get()->setSettingValue<bool>(m_setting_key, nv);
	}
	update_reset_btn();
	if (m_on_change) {
		m_on_change();
	}
}

void GwtSettingRow::on_info_btn(CCObject* sender) {
	auto* arr = static_cast<CCArray*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getUserObject());
	if (!arr || arr->count() < 2) {
		return;
	}
	FLAlertLayer::create(
		nullptr,
		static_cast<CCString*>(arr->objectAtIndex(0))->getCString(),
		static_cast<CCString*>(arr->objectAtIndex(1))->getCString(),
		"OK",
		nullptr,
		300.f
	)
		->show();
}

void GwtSettingRow::on_view_colors(CCObject*) {
	auto colors =
		m_popup ? m_popup->get_effective_value<gay::ColorList>(m_setting_key) : Mod::get()->getSettingValue<gay::ColorList>(m_setting_key);
	std::string kc = m_setting_key;
	GwtSettingsPopup* pp = m_popup;
	ColorListPopup::create(colors.colors, [kc, pp](std::vector<gay::ColorEntry> nc) {
		if (pp) {
			pp->set_pending(kc, matjson::Serialize<gay::ColorList>::toJson({std::move(nc)}));
		} else {
			Mod::get()->setSettingValue<gay::ColorList>(kc, {std::move(nc)});
		}
	})->show();
}

void GwtSettingRow::on_choice_dropdown(CCObject* sender) {
	if (m_choices.empty()) {
		return;
	}
	auto* btn = static_cast<CCNode*>(sender);
	auto world = btn->convertToWorldSpace({btn->getContentSize().width, 0.f});
	std::string kc = m_setting_key;
	GwtSettingsPopup* pp = m_popup;
	auto* dd = GwtChoiceDropdown::create(
		m_choices,
		m_choice_index,
		[this, kc, pp](int idx) {
			m_choice_index = idx;
			if (pp) {
				pp->set_pending(kc, matjson::Value((int64_t)idx));
			} else {
				Mod::get()->setSettingValue<int64_t>(kc, (int64_t)idx);
			}
			update_choice_label();
			update_reset_btn();
			if (m_on_change) {
				m_on_change();
			}
		},
		world
	);
	CCDirector::sharedDirector()->getRunningScene()->addChild(dd, 9999);
}

void GwtSettingRow::on_reset(CCObject*) {
	auto setting = Mod::get()->getSetting(m_setting_key);
	if (!setting) {
		return;
	}
	if (m_popup) {
		m_popup->clear_pending(m_setting_key);
	}
	setting->reset();
	update_reset_btn();
	if (m_popup) {
		m_popup->request_rebuild();
	}
}

GwtSettingRow* GwtSettingRow::create(const gay::SettingDisplayInfo& info, gay::DisplayMode mode, float width, GwtSettingsPopup* popup) {
	auto* r = new GwtSettingRow();
	if (r->init(info, mode, width, popup)) {
		r->autorelease();
		return r;
	}
	delete r;
	return nullptr;
}
