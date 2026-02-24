#include <gay/color.hpp>
#include <gay/ui/settings_popup.hpp>
#include <gay/utils.hpp>

using namespace geode::prelude;
using namespace gay::color;
namespace rp = gay::color::rose_pine;
using ST = gay::SettingType;
using gay::util::rp_c4;

bool GwtSectionHeader::init(std::string_view title, std::string_view tooltip, float width) {
	if (!CCNode::init()) {
		gay::util::log(gay::util::LogLevel::Debug, "Failed to initialize");
		return false;
	}

	m_header_title = std::string(title);
	m_header_tooltip = std::string(tooltip);

	constexpr float HEIGHT = 28.0f;
	constexpr float INFO_BTN_RESERVED = 20.0f;

	this->setContentSize({width, HEIGHT});
	this->setAnchorPoint({0.0f, 0.0f});

	auto* header_bg = CCLayerColor::create(rp_c4(rp::overlay, 200), width, HEIGHT);
	header_bg->setID("header-bg");
	header_bg->setPosition({0.0f, 0.0f});
	header_bg->ignoreAnchorPointForPosition(true);
	this->addChild(header_bg, -1);

	float info_reserved = tooltip.empty() ? 0.0f : INFO_BTN_RESERVED;

	auto* label = CCLabelBMFont::create(m_header_title.c_str(), "bigFont.fnt");
	label->setID("header-label");
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
		left_rule->setID("left-rule");
		left_rule->setPosition({4.0f, HEIGHT / 2.0f});
		left_rule->ignoreAnchorPointForPosition(true);
		this->addChild(left_rule);
	}

	if (!tooltip.empty()) {
		auto* info_spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		info_spr->setID("info-icon");
		info_spr->setScale(0.38f);

		auto* info_btn = CCMenuItemSpriteExtra::create(info_spr, this, menu_selector(GwtSectionHeader::on_info_btn));
		info_btn->setID("info-btn");

		auto* info_menu = CCMenu::create();
		info_menu->setID("info-menu");
		info_menu->setPosition({0.0f, 0.0f});
		info_menu->setContentSize({INFO_BTN_RESERVED, HEIGHT});
		info_btn->setPosition({width - INFO_BTN_RESERVED / 2.0f, HEIGHT / 2.0f});
		info_menu->addChild(info_btn);
		this->addChild(info_menu);
	}

	float right_rule_start = usable_center + label_half;
	float right_rule_end = width - info_reserved - 4.0f;
	float right_rule_w = right_rule_end - right_rule_start;
	if (right_rule_w > 2.0f) {
		auto* right_rule = CCLayerColor::create(rp_c4(rp::subtle, 60), right_rule_w, 1.5f);
		right_rule->setID("right-rule");
		right_rule->setPosition({right_rule_start, HEIGHT / 2.0f});
		right_rule->ignoreAnchorPointForPosition(true);
		this->addChild(right_rule);
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
