#include "color_list_setting.hpp"

#include <Geode/cocos/base_nodes/CCNode.h>

#include <utility>

#include "../../ui/color_popup.hpp"

using namespace geode::prelude;

Result<std::shared_ptr<SettingV3>> ColorListSetting::parse(const std::string& key, const std::string& modID, const matjson::Value& json) {
	auto ret = std::make_shared<ColorListSetting>();
	auto root = checkJson(json, "ColorListSetting");

	ret->parseBaseProperties(key, modID, root);
	root.checkUnknownKeys();

	return root.ok(ret).map([](std::shared_ptr<ColorListSetting> value) { return std::static_pointer_cast<SettingV3>(std::move(value)); });
}

bool ColorListSettingNode::init(std::shared_ptr<ColorListSetting> setting, float width) {
	if (!SettingValueNodeV3::init(std::move(setting), width)) {
		return false;
	}

	float height = 40.0f;
	this->setContentSize({width, height});

	CCMenu* menu = CCMenu::create();
	menu->setPosition({0.0f, 0.0f});
	menu->setID("colors-menu");

	auto view_spr = ButtonSprite::create("View");
	view_spr->setScale(0.72f);

	auto view_btn = CCMenuItemSpriteExtra::create(view_spr, this, menu_selector(ColorListSettingNode::onView));
	view_btn->setPosition(width - 40.f, height - 20.f);

	menu->addChild(view_btn);
	this->addChild(menu);

	handleTouchPriority(this);

	return true;
}

void ColorListSettingNode::onView(cocos2d::CCObject*) {
	auto setting = this->getSetting();
	ColorListPopup::create(setting->getValue().colors, [this](std::vector<ColorEntry> colors) {
		this->setValue(ColorList {std::move(colors)}, this);
	})->show();
}

ColorListSettingNode* ColorListSettingNode::create(std::shared_ptr<ColorListSetting> setting, float width) {
	auto ret = new ColorListSettingNode();
	if (ret->init(std::move(setting), width)) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

SettingNodeV3* ColorListSetting::createNode(float width) {
	return ColorListSettingNode::create(std::static_pointer_cast<ColorListSetting>(shared_from_this()), width);
}