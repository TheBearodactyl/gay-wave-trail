#include <gay/types.hpp>
#include <gay/ui/color_cell.hpp>
#include <gay/ui/color_popup.hpp>

#include <Geode/ui/General.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

bool ColorListPopup::setup(std::vector<gay::ColorEntry> entries, std::function<void(std::vector<gay::ColorEntry>)> callback) {
	m_entries = std::move(entries);
	m_callback = std::move(callback);

	create_list();

	auto* spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	spr->setScale(0.7f);

	auto* add_btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ColorListPopup::on_add));
	add_btn->setID("add-color-btn");
	add_btn->setAnchorPoint({0.5f, 0.5f});

	auto* menu = CCMenu::create();
	menu->setID("add-color-menu");
	menu->addChild(add_btn);
	menu->setZOrder(1);
	m_mainLayer
		->addChildAtPosition(menu, Anchor::BottomRight, {-5.f - add_btn->getContentWidth() / 2.f, 8.f + add_btn->getContentHeight() / 2.f});

	return true;
}

void ColorListPopup::create_list() {
	auto sz = m_mainLayer->getContentSize();
	constexpr float PAD = 5.f;

	if (m_list) {
		m_list->removeFromParent();
	}

	m_list = ScrollLayer::create({sz.width - PAD * 2, sz.height - PAD * 2 - 4.f});
	m_list->setID("color-list");
	m_list->m_contentLayer->setLayout(
		ColumnLayout::create()
			->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::End)
			->setAutoGrowAxis(sz.height - PAD * 2 - 4.f)
			->setGap(PAD / 2)
	);
	m_list->setPosition({PAD, PAD + 2.f});

	for (size_t i = 0; i < m_entries.size(); ++i) {
		auto* cell = ColorCell::create(
			this,
			&m_entries[i],
			[this, i] {
				if (i < m_entries.size()) {
					m_entries.erase(m_entries.begin() + (ptrdiff_t)i);
					create_list();
				}
			},
			[] {
			},
			{320.f - PAD * 2, 35.f}
		);

		cell->setAnchorPoint({0.f, 0.f});
		m_list->m_contentLayer->addChild(cell);
	}

	auto* spacer = CCMenu::create();
	spacer->setID("bottom-spacer");
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
	create_list();
}

ColorListPopup*
ColorListPopup::create(std::vector<gay::ColorEntry> colors, const std::function<void(std::vector<gay::ColorEntry>)>& callback) {
	auto* r = new ColorListPopup();

	if (r->init(320.f, 240.f, "GJ_square01.png")) {
		r->setup(std::move(colors), callback);
		r->autorelease();
		return r;
	}

	delete r;
	return nullptr;
}
