#include "color_popup.hpp"

#include "color_cell.hpp"
#include <utility>

using namespace geode::prelude;

bool ColorListPopup::setup(std::vector<ColorEntry> entries, std::function<void(std::vector<ColorEntry>)> callback) {
  m_entries = entries;
  m_callback = callback;

  this->create_list();

  auto spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
  spr->setScale(0.7f);

  auto add_btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ColorListPopup::onAdd));
  add_btn->setAnchorPoint({0.5f, 0.5f});

  auto menu = CCMenu::create();
  menu->addChild(add_btn);
  menu->setZOrder(1);

  m_mainLayer->addChildAtPosition(menu, Anchor::BottomRight, {-5.f - add_btn->getContentWidth() / 2.f, 8.f + add_btn->getContentHeight() / 2.f});

  return true;
}

void ColorListPopup::create_list() {
  auto size = this->m_mainLayer->getContentSize();

  constexpr float HORIZONTAL_PADDING = 5.f;

  if (m_list) {
    m_list->removeFromParent();
  }

  m_list = ScrollLayer::create({size.width - HORIZONTAL_PADDING * 2, size.height - HORIZONTAL_PADDING * 2 - 4.f});

  m_list->m_contentLayer->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End)->setAutoGrowAxis(size.height - HORIZONTAL_PADDING * 2 - 4.f)->setGap(HORIZONTAL_PADDING / 2));

  m_list->setPosition({HORIZONTAL_PADDING, HORIZONTAL_PADDING + 2.f});

  for (int i = 0; i < m_entries.size(); i++) {
    auto cell = ColorCell::create(
        this, &m_entries[i],
        [this, i] {
          m_entries.erase(m_entries.begin() + i);
          this->create_list();
        },
        CCSize{320.f - HORIZONTAL_PADDING * 2, 35.f});

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

void ColorListPopup::onClose(cocos2d::CCObject *sender) {
  m_callback(m_entries);

  Popup::onClose(sender);
}

void ColorListPopup::onAdd(cocos2d::CCObject *) {
  m_entries.emplace_back("#", true, true);

  this->create_list();
}

ColorListPopup *ColorListPopup::create(std::vector<ColorEntry> colors, const std::function<void(std::vector<ColorEntry>)> &callback) {
  auto ret = new ColorListPopup();
  auto size = CCSize{320.f, 240.f};

  if (ret->initAnchored(size.width, size.height, std::move(colors), callback, "geode.loader/GE_square01.png")) {
    ret->autorelease();
    return ret;
  }

  CC_SAFE_DELETE(ret);
  return nullptr;
}

void ColorListPopup::updateColor(const cocos2d::ccColor3B &color) {
  auto popup = geode::ColorPickPopup::create(color);
  popup->setDelegate(this);
  popup->show();
}