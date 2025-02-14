#include "colors_popup.hpp"

#include <optional>

#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include "color_cell.hpp"

bool ColorListPopup::setup(
        std::vector<std::string> colors,
        std::function<void(std::vector<std::string>)> setColorsCallback) {
    m_colors = colors;
    m_setColorsCallback = setColorsCallback;

    this->createList();

    auto spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    spr->setScale(0.7f);
    auto addBtn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(ColorListPopup::onAdd));
    addBtn->setAnchorPoint({0.5f, 0.5f});
    auto menu = CCMenu::create();
    menu->addChild(addBtn);
    menu->setZOrder(1);
    m_mainLayer->addChildAtPosition(menu, Anchor::BottomRight,
                                    {-5.f - addBtn->getContentWidth() / 2.f,
                                     8.f + addBtn->getContentHeight() / 2.f});

    return true;
}

void ColorListPopup::onClose(CCObject *sender) {
    m_setColorsCallback(m_colors);
    Popup::onClose(sender);
}

void ColorListPopup::onAdd(CCObject *) {
    m_colors.push_back("#");// Add empty color string
    this->createList();
}

ColorListPopup *ColorListPopup::create(
        std::vector<std::string> colors,
        std::function<void(std::vector<std::string>)> setColorsCallback) {
    auto ret = new ColorListPopup();
    auto size = ret->getPopupSize();
    if (ret && ret->initAnchored(size.width, size.height, colors,
                                 setColorsCallback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCSize ColorListPopup::getPopupSize() { return {320.f, 240.f}; }

void ColorListPopup::createList() {
    auto size = this->m_mainLayer->getContentSize();

    constexpr float HORIZONTAL_PADDING = 5.f;

    if (m_list) {
        m_list->removeFromParent();
    }

    m_list = ScrollLayer::create({size.width - HORIZONTAL_PADDING * 2,
                                  size.height - HORIZONTAL_PADDING * 2 - 4.f});
    m_list->m_contentLayer->setLayout(
            ColumnLayout::create()
                    ->setAxisReverse(true)
                    ->setAxisAlignment(AxisAlignment::End)
                    ->setAutoGrowAxis(size.height - HORIZONTAL_PADDING * 2 - 4.f)
                    ->setGap(HORIZONTAL_PADDING / 2));
    m_list->setPosition({HORIZONTAL_PADDING, HORIZONTAL_PADDING + 2.f});

    for (int i = 0; i < m_colors.size(); i++) {
        auto cell = ColorCell::create(
                this, &m_colors[i],
                [this, i] {
                    m_colors.erase(m_colors.begin() + i);
                    this->createList();
                },
                CCSize{this->getPopupSize().width - HORIZONTAL_PADDING * 2, 35.f});
        cell->setAnchorPoint({0.f, 0.f});
        m_list->m_contentLayer->addChild(cell);
    }
    auto menu = CCMenu::create();
    menu->setContentSize({0.f, 36.f});
    m_list->m_contentLayer->addChild(menu);

    m_list->m_contentLayer->updateLayout();
    this->m_mainLayer->addChild(m_list);
    handleTouchPriority(this);
}
