#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/TextInput.hpp>

#include <gay/ui/color_cell.hpp>
#include <gay/utils/hex_utils.hpp>

#include <utility>

using namespace geode::prelude;
using namespace gay::utils::hex_code;

bool ColorCell::init(
	ColorListPopup* parent_popup,
	ColorEntry* entry,
	std::function<void()> on_delete,
	std::function<void()> on_color_change,
	const cocos2d::CCSize& size
) {
	if (!CCNode::init()) {
		return false;
	}

	static const float HORIZONTAL_PADDING = 2.6f;

	m_parentPopup = parent_popup;
	m_entry = entry;
	m_onDelete = std::move(on_delete);
	m_onColorChange = std::move(on_color_change);

	auto entry_color = HexUtils::hex_to_rgb(m_entry->m_hex);

	this->setContentSize(size);
	this->setAnchorPoint(CCPoint {0.5f, 0.5f});

	m_background = CCScale9Sprite::create("square02b_001.png");
	m_background->setColor({entry_color.r, entry_color.g, entry_color.b});
	m_background->setOpacity(100);
	m_background->setScale(0.3f);
	m_background->setContentSize(size / m_background->getScale());
	this->addChildAtPosition(m_background, Anchor::Center);

	float m_buttonsSize = 0.f;

	m_toggleButton = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ColorCell::on_toggle), .6f);
	m_toggleButton->setAnchorPoint(ccp(0.5f, 0.5f));
	m_buttonsSize += m_toggleButton->getContentSize().width;

	auto btns_menu = CCMenu::create();
	btns_menu->addChild(m_toggleButton);
	btns_menu->setAnchorPoint(CCPoint {1.0f, 0.5f});
	btns_menu->setContentSize(CCSize {50.f, 30.f});

	m_colorPreview = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
	m_colorPreview->setScale(0.5f);
	m_colorPreview->setColor({entry_color.r, entry_color.g, entry_color.b});

	auto color_picker_btn = CCMenuItemSpriteExtra::create(m_colorPreview, this, menu_selector(ColorCell::on_color_picker));
	color_picker_btn->setID("color-picker-button");
	btns_menu->addChild(color_picker_btn);
	m_buttonsSize += color_picker_btn->getContentSize().width * 0.5f;

	if (m_entry->m_userAdded) {
		auto sprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
		sprite->setScale(0.7f);

		auto delete_btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ColorCell::on_delete));

		delete_btn->setID("delete-button");
		btns_menu->addChild(delete_btn);
		m_buttonsSize += delete_btn->getContentSize().width * 0.7f;
	}

	btns_menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Even));

	btns_menu->updateLayout();
	btns_menu->setID("button-menu");

	m_textInput = TextInput::create(size.width - HORIZONTAL_PADDING * 2 - m_buttonsSize, "Color Hex Code", "FiraCodeNerdMono.fnt"_spr);
	m_textInput->setScale(1.f);
	m_textInput->setCommonFilter(CommonFilter::Any);
	m_textInput->setMaxCharCount(300);
	m_textInput->setString(m_entry->m_hex, false);
	m_textInput->setTextAlign(TextInputAlign::Left);
	m_textInput->setID("hex-input");
	m_textInput->setCallback([this](std::string const& str) {
		m_entry->m_hex = str;
		this->update_color_preview();
		this->update_background_color();
		if (m_onColorChange) {
			m_onColorChange();
		}
	});

	auto menu = CCMenu::create();
	menu->addChild(m_textInput);
	menu->addChild(btns_menu);

	menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Between));
	menu->setID("menu");
	menu->setAnchorPoint(CCPoint {0.5f, 0.5f});
	menu->setContentSize(CCSize {size.width - HORIZONTAL_PADDING * 2, size.height});
	menu->updateLayout();

	this->addChildAtPosition(menu, Anchor::Center, CCPoint {0.0f, 0.0f});
	this->update_ui();

	return true;
}

void ColorCell::update_ui() {
	m_toggleButton->toggle(m_entry->m_enabled);
}

void ColorCell::update_color_preview() {
	if (!m_colorPreview)
		return;
	auto color = HexUtils::hex_to_rgb(m_entry->m_hex);
	m_colorPreview->setColor({color.r, color.g, color.b});
}

void ColorCell::update_background_color() {
	if (!m_background)
		return;
	auto color = HexUtils::hex_to_rgb(m_entry->m_hex);
	m_background->setColor({color.r, color.g, color.b});
}

void ColorCell::update_text_input() {
	if (!m_textInput)
		return;
	m_textInput->setString(m_entry->m_hex, false);
}

void ColorCell::on_toggle(cocos2d::CCObject*) {
	m_entry->m_enabled = !m_entry->m_enabled;
	this->update_ui();
	m_toggleButton->toggle(!m_toggleButton->isToggled());
}

void ColorCell::on_delete(cocos2d::CCObject*) {
	if (m_onDelete) {
		m_onDelete();
	}
}

void ColorCell::on_color_picker(cocos2d::CCObject*) {
	auto current_color = HexUtils::hex_to_rgb(m_entry->m_hex);
	auto popup = geode::ColorPickPopup::create(current_color);
	popup->setDelegate(this);
	popup->show();
}

void ColorCell::updateColor(const ccColor4B& color) {
	m_entry->m_hex = HexUtils::rgb_to_hex(color);

	this->update_color_preview();
	this->update_background_color();
	this->update_text_input();

	if (m_onColorChange) {
		m_onColorChange();
	}
}

ColorCell* ColorCell::create(
	ColorListPopup* parent_popup,
	ColorEntry* entry,
	std::function<void()> on_delete,
	std::function<void()> on_color_change,
	const CCSize& size
) {
	auto ret = new ColorCell();
	if (ret->init(parent_popup, entry, std::move(on_delete), std::move(on_color_change), size)) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}
