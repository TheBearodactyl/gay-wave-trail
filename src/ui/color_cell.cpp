#include <gay/color.hpp>
#include <gay/types.hpp>
#include <gay/ui/color_cell.hpp>
#include <gay/ui/color_popup.hpp>

#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

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

	static constexpr float H_PAD = 2.6f;

	m_parent_popup = parent;
	m_entry = entry;
	m_on_delete = std::move(on_delete);
	m_on_color_change = std::move(on_color_change);

	if (!m_entry) {
		return false;
	}

	auto col = gay::color::hex_to_rgb(m_entry->hex);

	setContentSize(size);
	setAnchorPoint({0.5f, 0.5f});

	m_background = NineSlice::create("square02b_001.png");
	m_background->setID("color-cell-bg");
	m_background->setColor({col.r, col.g, col.b});
	m_background->setOpacity(100);
	m_background->setScale(0.3f);
	m_background->setContentSize(size / m_background->getScale());
	addChildAtPosition(m_background, Anchor::Center);

	float btns_w = 0.f;

	m_toggle_btn = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ColorCell::on_toggle), 0.6f);
	m_toggle_btn->setID("color-toggle");
	m_toggle_btn->setAnchorPoint({0.5f, 0.5f});
	btns_w += m_toggle_btn->getContentSize().width;

	auto* btns_menu = CCMenu::create();
	btns_menu->addChild(m_toggle_btn);
	btns_menu->setAnchorPoint({1.f, 0.5f});
	btns_menu->setContentSize({50.f, 30.f});

	m_color_preview = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
	m_color_preview->setID("color-preview");
	m_color_preview->setScale(0.5f);
	m_color_preview->setColor({col.r, col.g, col.b});

	auto* pick_btn = CCMenuItemSpriteExtra::create(m_color_preview, this, menu_selector(ColorCell::on_color_picker));
	pick_btn->setID("color-picker-button");
	btns_menu->addChild(pick_btn);
	btns_w += pick_btn->getContentSize().width * 0.5f;

	if (m_entry->user_added) {
		auto* del_spr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
		del_spr->setScale(0.7f);
		auto* del_btn = CCMenuItemSpriteExtra::create(del_spr, this, menu_selector(ColorCell::on_delete_btn));
		del_btn->setID("delete-button");
		btns_menu->addChild(del_btn);
		btns_w += del_btn->getContentSize().width * 0.7f;
	}

	btns_menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Even));
	btns_menu->updateLayout();
	btns_menu->setID("button-menu");

	m_text_input = TextInput::create(size.width - H_PAD * 2 - btns_w, "Color Hex Code", "FiraCodeNerdMono.fnt"_spr);
	m_text_input->setScale(1.f);
	m_text_input->setCommonFilter(CommonFilter::Any);
	m_text_input->setMaxCharCount(300);
	m_text_input->setString(m_entry->hex, false);
	m_text_input->setTextAlign(TextInputAlign::Left);
	m_text_input->setID("hex-input");
	m_text_input->setCallback([this](const std::string& s) {
		if (!m_entry) {
			return;
		}
		m_entry->hex = s;
		update_color_preview();
		update_background_color();
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
	menu->setContentSize({size.width - H_PAD * 2, size.height});
	menu->updateLayout();
	addChildAtPosition(menu, Anchor::Center);

	update_ui();
	return true;
}

void ColorCell::update_ui() {
	m_toggle_btn->toggle(m_entry->enabled);
}

void ColorCell::update_color_preview() {
	if (!m_color_preview || !m_entry) {
		return;
	}
	auto c = gay::color::hex_to_rgb(m_entry->hex);
	m_color_preview->setColor({c.r, c.g, c.b});
}

void ColorCell::update_background_color() {
	if (!m_background || !m_entry) {
		return;
	}
	auto c = gay::color::hex_to_rgb(m_entry->hex);
	m_background->setColor({c.r, c.g, c.b});
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
	update_ui();
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
	auto cur = gay::color::hex_to_rgb(m_entry->hex);
	auto* popup = ColorPickPopup::create(cur);
	popup->setCallback([this](const ccColor4B& c) {
		if (!m_entry) {
			return;
		}
		m_entry->hex = gay::color::rgb_to_hex(c);
		update_color_preview();
		update_background_color();
		update_text_input();
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
	auto* r = new ColorCell();
	if (r->init(parent, entry, std::move(on_delete), std::move(on_color_change), size)) {
		r->autorelease();
		return r;
	}
	delete r;
	return nullptr;
}
