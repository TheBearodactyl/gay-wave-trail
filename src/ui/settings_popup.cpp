
#include <gay/color.hpp>
#include <gay/constants.hpp>
#include <gay/setting_defs.hpp>
#include <gay/settings.hpp>
#include <gay/ui/settings_popup.hpp>
#include <gay/utils.hpp>

#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/TextInput.hpp>

#include "Geode/ui/Popup.hpp"

#include <fmt/core.h>
#include <matjson.hpp>

using namespace geode::prelude;
using namespace gay::color;
namespace rp = gay::color::rose_pine;
using ST = gay::SettingType;
using gay::util::rp_c4;

float GwtSettingsPopup::s_saved_scroll_y = 0.f;

bool GwtSettingsPopup::setup() {
	m_closeBtn->setVisible(false);
	m_bgSprite->setVisible(false);

	auto* full_bg = CCLayerColor::create(rp_c4(rp::surface, 255));
	auto full_bg_size = m_mainLayer->getContentSize() - CCSize {6.f, 6.f};
	full_bg->setContentSize(full_bg_size);
	full_bg->ignoreAnchorPointForPosition(false);
	full_bg->setAnchorPoint({0.5f, 0.5f});
	full_bg->setPosition(m_mainLayer->getContentSize() / 2.f);
	full_bg->setID("full-bg-layer-color");
	m_mainLayer->addChild(full_bg, -2);

	constexpr float OUTER_BORDER = 2.f;
	auto* popup_border =
		CCLayerColor::create(rp_c4(rp::h_med, 255), full_bg_size.width + OUTER_BORDER * 2.f, full_bg_size.height + OUTER_BORDER * 2.f);
	popup_border->setID("popup-border");
	popup_border->ignoreAnchorPointForPosition(false);
	popup_border->setAnchorPoint({0.5f, 0.5f});
	popup_border->setPosition(m_mainLayer->getContentSize() / 2.f);
	m_mainLayer->addChild(popup_border, -3);

	auto* title_label = CCLabelBMFont::create("Settings for Gay Wave Trail", "goldFont.fnt");
	title_label->setID("popup-title");
	title_label->setScale(0.6f);
	title_label->setColor({rp::gold.r, rp::gold.g, rp::gold.b});
	title_label->setAnchorPoint({0.5f, 0.5f});
	m_mainLayer->addChildAtPosition(title_label, Anchor::Top, ccp(0.f, -15.f));

	constexpr float PAD = 2.f;
	m_list_bg = CCLayerColor::create(rp_c4(rp::base, 255));
	CCSize list_bg_size = {LIST_W + PAD * 2, LIST_H + SEARCH_H + PAD * 2};
	m_list_bg->setContentSize(list_bg_size);
	m_list_bg->setID("list-bg-layer-color");
	m_list_bg->ignoreAnchorPointForPosition(false);
	m_mainLayer->addChildAtPosition(m_list_bg, Anchor::Center, ccp(0.f, -4.f));

	constexpr float LIST_BORDER = 1.5f;
	auto* list_border =
		CCLayerColor::create(rp_c4(rp::overlay, 220), list_bg_size.width + LIST_BORDER * 2.f, list_bg_size.height + LIST_BORDER * 2.f);
	list_border->setID("list-border");
	list_border->ignoreAnchorPointForPosition(false);

	m_mainLayer->addChildAtPosition(list_border, Anchor::Center, ccp(0.f, -4.f));
	list_border->setZOrder(-1);

	auto* list_inner = CCLayerColor::create(rp_c4(rp::surface, 180));
	list_inner->setContentSize({LIST_W, LIST_H + SEARCH_H});
	list_inner->setID("list-inner-layer-color");
	list_inner->ignoreAnchorPointForPosition(false);
	m_list_bg->addChildAtPosition(list_inner, Anchor::Center);

	auto* search_menu = CCMenu::create();
	search_menu->setID("search-menu");
	search_menu->setContentSize({LIST_W, SEARCH_H});

	m_search = TextInput::create((LIST_W - 15.f) / 0.7f - 40.f, "Search settings...");
	m_search->setTextAlign(TextInputAlign::Left);
	m_search->setScale(0.7f);
	m_search->setCallback([this](const std::string& q) {
		on_search_changed(q);
	});
	m_search->setID("search-input");
	search_menu->addChildAtPosition(m_search, Anchor::Left, ccp(16.f, 0.f), ccp(0.f, 0.5f));

	auto* clr_spr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
	clr_spr->setScale(0.45f);
	clr_spr->setColor({rp::muted.r, rp::muted.g, rp::muted.b});
	clr_spr->setOpacity(90);
	m_clear_btn = CCMenuItemSpriteExtra::create(clr_spr, this, menu_selector(GwtSettingsPopup::on_clear_search));
	m_clear_btn->setID("clear-search-btn");
	m_clear_btn->setEnabled(false);
	search_menu->addChildAtPosition(m_clear_btn, Anchor::Right, ccp(-20.f, 0.f));

	list_inner->addChildAtPosition(search_menu, Anchor::Top, ccp(0.f, 0.f), ccp(0.5f, 1.f));

	auto* search_separator = CCLayerColor::create(rp_c4(rp::overlay, 180), LIST_W, 1.5f);
	search_separator->setID("search-separator");
	search_separator->ignoreAnchorPointForPosition(false);

	list_inner->addChildAtPosition(search_separator, Anchor::Top, ccp(0.f, -SEARCH_H), ccp(0.5f, 1.f));
	search_separator->setZOrder(1);

	rebuild_list();

	if (s_saved_scroll_y != 0.f) {
		m_list->m_contentLayer->setPositionY(s_saved_scroll_y);
	}

	auto* scrollbar = Scrollbar::create(m_list);
	scrollbar->setID("scrollbar");
	m_mainLayer->addChildAtPosition(scrollbar, Anchor::Center, ccp(LIST_W / 2.f + 10.f, -SEARCH_H / 2.f - 4.f));

	auto* bottom_menu = CCMenu::create();
	bottom_menu->setID("bottom-menu");
	bottom_menu->setContentSize({LIST_W, 30.f});

	auto* apply_spr = ButtonSprite::create("Apply", "goldFont.fnt", "geode.loader/GE_button_05-uhd.png", 0.7f);
	apply_spr->setScale(0.65f);
	m_apply_btn = CCMenuItemSpriteExtra::create(apply_spr, this, menu_selector(GwtSettingsPopup::on_apply));
	m_apply_btn->setID("apply-btn");

	auto* close_spr = ButtonSprite::create("Close", "goldFont.fnt", "geode.loader/GE_button_05-uhd.png", 0.7f);
	close_spr->setScale(0.65f);
	m_close_btn = CCMenuItemSpriteExtra::create(close_spr, this, menu_selector(GwtSettingsPopup::onClose));
	m_close_btn->setID("close-btn");

	float btn_half_gap = m_apply_btn->getScaledContentWidth() / 2.f + 6.f;
	bottom_menu->addChildAtPosition(m_apply_btn, Anchor::Center, ccp(-btn_half_gap, 0.f));
	bottom_menu->addChildAtPosition(m_close_btn, Anchor::Center, ccp(btn_half_gap, 0.f));

	m_mainLayer->addChildAtPosition(bottom_menu, Anchor::Bottom, ccp(0.f, 18.f));

	update_apply_btn();

	for (auto* ch : m_mainLayer->getChildrenExt()) {
		if (typeinfo_cast<CCLabelBMFont*>(ch)) {
			static_cast<CCNode*>(ch)->setZOrder(1);
		}
	}

	handleTouchPriority(this);
	return true;
}

void GwtSettingsPopup::on_search_changed(const std::string& query) {
	rebuild_list();
	if (!m_clear_btn) {
		return;
	}
	bool active = !query.empty();
	auto* spr = static_cast<CCSprite*>(m_clear_btn->getNormalImage());
	m_clear_btn->setEnabled(active);
	spr->setColor(active ? ccColor3B {rp::text.r, rp::text.g, rp::text.b} : ccColor3B {rp::muted.r, rp::muted.g, rp::muted.b});
	spr->setOpacity(active ? 255 : 90);
}

void GwtSettingsPopup::rebuild_list(bool preserve_scroll) {
	auto mode = static_cast<gay::DisplayMode>(get_effective_value<int64_t>("display-mode"));
	std::string query = m_search ? std::string(m_search->getString()) : std::string {};
	bool has_query = !query.empty();

	struct ScoredInfo {
		const gay::SettingDisplayInfo* info;
		int score;
	};

	std::vector<ScoredInfo> scored;

	for (const auto& info : SETTING_DISPLAY_INFO) {
		if (!has_query) {
			scored.push_back({&info, 0});
			continue;
		}
		std::string name = std::string(mode == gay::DisplayMode::Gay ? info.gay_name : info.straight_name);
		std::string key = std::string(info.key);
		auto nr = gay::util::fuzzy_match(query, name);
		auto kr = gay::util::fuzzy_match(query, key);
		if (nr.matched || kr.matched) {
			scored.push_back({&info, std::max(nr.score, kr.score)});
		}
	}

	if (has_query) {
		std::stable_sort(scored.begin(), scored.end(), [](const ScoredInfo& a, const ScoredInfo& b) {
			return a.score > b.score;
		});
	}

	if (!m_list) {
		m_list = ScrollLayer::create({LIST_W, LIST_H});
		m_list->setID("settings-list");
		m_list->m_contentLayer->setLayout(
			ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End)->setAutoGrowAxis(LIST_H)->setGap(0.f)
		);

		auto* inner = m_list_bg->getChildByID("list-inner-layer-color");
		(inner ? inner : static_cast<CCNode*>(m_list_bg))->addChildAtPosition(m_list, Anchor::BottomLeft);
	}

	m_list->m_contentLayer->removeAllChildren();
	gay::s_row_index = 0;

	std::string_view cur_section;
	for (const auto& s : scored) {
		const auto& info = *s.info;

		if (!has_query && !info.section_straight.empty() && info.section_straight != cur_section) {
			cur_section = info.section_straight;
			std::string_view title =
				(mode == gay::DisplayMode::Gay && !info.section_gay.empty()) ? info.section_gay : info.section_straight;
			if (auto* hdr = GwtSectionHeader::create(title, info.section_tooltip, LIST_W)) {
				m_list->m_contentLayer->addChild(hdr);
			}
		}

		auto* row = GwtSettingRow::create(info, mode, LIST_W, this);
		if (!row) {
			continue;
		}
		if (info.key == "display-mode") {
			row->set_on_change([this] {
				rebuild_list();
			});
		}
		m_list->m_contentLayer->addChild(row);
	}

	float saved_y = m_list->m_contentLayer->getPositionY();
	m_list->m_contentLayer->updateLayout();
	if (preserve_scroll) {
		m_list->m_contentLayer->setPositionY(saved_y);
	} else {
		m_list->scrollToTop();
	}
	handleTouchPriority(this);
}

void GwtSettingsPopup::on_clear_search(CCObject*) {
	if (m_search) {
		m_search->setString("");
	}
	on_search_changed("");
}

void GwtSettingsPopup::on_apply(CCObject*) {
	for (const auto& [key, val] : m_pending) {
		for (const auto& info : SETTING_DISPLAY_INFO) {
			if (info.key != key) {
				continue;
			}
			switch (info.type) {
				case ST::Bool: {
					auto r = val.as<bool>();
					if (r.isOk()) {
						Mod::get()->setSettingValue<bool>(key, r.unwrap());
					}
					break;
				}
				case ST::Float: {
					auto r = val.as<double>();
					if (r.isOk()) {
						Mod::get()->setSettingValue<double>(key, r.unwrap());
					}
					break;
				}
				case ST::Int:
				case ST::Choice: {
					auto r = val.as<int64_t>();
					if (r.isOk()) {
						Mod::get()->setSettingValue<int64_t>(key, r.unwrap());
					}
					break;
				}
				case ST::ColorList: {
					auto r = matjson::Serialize<gay::ColorList>::fromJson(val);
					if (r.isOk()) {
						Mod::get()->setSettingValue<gay::ColorList>(key, r.unwrap());
					}
					break;
				}
			}
			break;
		}
	}
	m_pending.clear();
	update_apply_btn();
}

void GwtSettingsPopup::on_discard(CCObject*) {
	m_pending.clear();
	update_apply_btn();
}

void GwtSettingsPopup::onClose(CCObject* sender) {
	if (m_list) {
		s_saved_scroll_y = m_list->m_contentLayer->getPositionY();
	}

	if (!m_pending.empty()) {
		geode::createQuickPopup(
			"Unsaved Changes",
			"You have some unsaved changes, do you want to apply them?",
			"Nah",
			"Sure",
			[this, sender](bool btn1, bool btn2) {
				if (btn1) {
					this->m_pending.clear();
				}

				if (btn2) {
					this->on_apply(nullptr);
				}

				Popup::onClose(sender);
			}
		);

		return;
	}

	Popup::onClose(sender);
}

void GwtSettingsPopup::update_apply_btn() {
	bool has = !m_pending.empty();
	auto dim = [](CCMenuItemSpriteExtra* btn, bool en) {
		if (!btn) {
			return;
		}
		btn->setEnabled(en);
		if (auto* spr = dynamic_cast<CCSprite*>(btn->getNormalImage())) {
			spr->setOpacity(en ? 255 : 120);
		}
	};
	dim(m_apply_btn, has);
	dim(m_discard_btn, has);
}

bool GwtSettingsPopup::has_pending(const std::string& key) const {
	return m_pending.count(key) > 0;
}

matjson::Value GwtSettingsPopup::get_pending(const std::string& key) const {
	auto it = m_pending.find(key);
	return it != m_pending.end() ? it->second : matjson::Value {};
}

void GwtSettingsPopup::set_pending(const std::string& key, matjson::Value v) {
	m_pending[key] = std::move(v);
	update_apply_btn();
}

void GwtSettingsPopup::clear_pending(const std::string& key) {
	m_pending.erase(key);
	update_apply_btn();
}

GwtSettingsPopup* GwtSettingsPopup::create() {
	auto* r = new GwtSettingsPopup();
	if (r->init(WIDTH, HEIGHT, "GJ_square01.png")) {
		r->setup();
		r->autorelease();
		return r;
	}
	delete r;
	return nullptr;
}

void GwtSettingsPopup::request_rebuild() {
	rebuild_list(true);
}
