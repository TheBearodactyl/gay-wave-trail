#include "color_cell.hpp"
#include "../utils/hex_utils.hpp"

#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <utility>

using namespace geode::prelude;
using namespace gay::utils::hex_code;

bool ColorCell::init(
        ColorListPopup *parent_popup,
        ColorEntry *entry,
        std::function<void()> on_delete,
        const cocos2d::CCSize &size
) {
    if (!CCNode::init()) return false;

    static const float HORIZONTAL_PADDING = 2.6f;

    m_parentPopup = parent_popup;
    m_entry = entry;
    m_onDelete = std::move(on_delete);

    auto entry_color = HexUtils::hex_to_rgb(m_entry->m_hex);

    this->setContentSize(size);
    this->setAnchorPoint(CCPoint{0.5f, 0.5f});

    auto bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({entry_color.r, entry_color.g, entry_color.b});
    bg->setOpacity(100);
    bg->setScale(0.3f);
    bg->setContentSize(size / bg->getScale());
    this->addChildAtPosition(bg, Anchor::Center);

    float m_buttonsSize = 0.f;

    m_toggleButton = CCMenuItemToggler::createWithStandardSprites(
            this,
            menu_selector(ColorCell::on_toggle),
            .6f
    );
    m_toggleButton->setAnchorPoint(ccp(0.5f, 0.5f));

    m_buttonsSize += m_toggleButton->getContentSize().width;

    auto btns_menu = CCMenu::create();
    btns_menu->addChild(m_toggleButton);
    btns_menu->setAnchorPoint(CCPoint{1.0f, 0.5f});
    btns_menu->setContentSize(CCSize{50.f, 30.f});

    if (m_entry->m_userAdded) {
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
        sprite->setScale(0.7f);

        auto delete_btn = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(ColorCell::on_delete)
        );

        delete_btn->setID("delete-button");
        btns_menu->addChild(delete_btn);

        m_buttonsSize += delete_btn->getContentSize().width;
    }

    btns_menu->setLayout(
            RowLayout::create()
                    ->setGap(5.f)
                    ->setAxisAlignment(AxisAlignment::Even)
    );

    btns_menu->updateLayout();
    btns_menu->setID("button-menu");

    auto input_node = TextInput::create(size.width - HORIZONTAL_PADDING * 2 - m_buttonsSize, "Color Hex Code", "FiraCodeNerdMono.fnt"_spr);
    input_node->setScale(1.f);
    input_node->setCommonFilter(CommonFilter::Any);
    input_node->setMaxCharCount(300);
    input_node->setString(m_entry->m_hex, false);
    input_node->setTextAlign(TextInputAlign::Left);
    input_node->setCallback(
            [this](std::string const &str) {
                m_entry->m_hex = str;
            }
    );

    auto menu = CCMenu::create();
    menu->addChild(input_node);
    menu->addChild(btns_menu);

    menu->setLayout(RowLayout::create()->setGap(5.f)->setAxisAlignment(AxisAlignment::Between));
    menu->setID("menu");
    menu->setAnchorPoint(CCPoint{0.5f, 0.5f});
    menu->setContentSize(CCSize{size.width - HORIZONTAL_PADDING * 2, size.height});
    menu->updateLayout();

    this->addChildAtPosition(menu, Anchor::Center, CCPoint{0.0f, 0.0f});
    this->update_ui();

    return true;
}

void ColorCell::update_ui() {
    m_toggleButton->toggle(m_entry->m_enabled);
}

void ColorCell::on_toggle(cocos2d::CCObject *) {
    m_entry->m_enabled = !m_entry->m_enabled;
    this->update_ui();
    m_toggleButton->toggle(!m_toggleButton->isToggled());
}

void ColorCell::on_delete(cocos2d::CCObject *) {
    m_onDelete();
}

ColorCell *ColorCell::create(
        ColorListPopup *parent_popup,
        ColorEntry *entry,
        std::function<void()> on_delete,
        const CCSize &size
) {
    auto ret = new ColorCell();
    if (ret->init(parent_popup, entry, std::move(on_delete), size)) {
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}