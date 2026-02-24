#include <gay/settings.hpp>
#include <gay/ui/settings_popup.hpp>

#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class GwtSettingsRedirect: public CCNode {
  public:
	void on_open_settings(CCObject*) {
		GwtSettingsPopup::create()->show();
	}

	static GwtSettingsRedirect* shared() {
		static auto* inst = [] {
			auto* r = new GwtSettingsRedirect();
			r->init();
			r->retain();
			return r;
		}();
		return inst;
	}
};

static geode::ListenerHandle s_settings_listener;

$execute {
	s_settings_listener = geode::ModPopupUIEvent().listen([](FLAlertLayer* popup, std::string_view mod_id, std::optional<Mod*>) -> bool {
		if (mod_id != Mod::get()->getID() || !popup) {
			return false;
		}

		if (auto* btn = popup->getChildByIDRecursive("settings-button")) {
			if (auto* item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
				item->setTarget(GwtSettingsRedirect::shared(), menu_selector(GwtSettingsRedirect::on_open_settings));
			}
		}

		return false;
	});
}

class $modify(GwtPauseLayer, PauseLayer) {
	void on_gwt_settings(CCObject*) {
		GwtSettingsPopup::create()->show();
	}

	void customSetup() override {
		PauseLayer::customSetup();

		if (!gay::settings::get<bool>("enable-pause-btn")) {
			return;
		}

		auto* spr = CCSprite::create("button.png"_spr);
		spr->setID("gwt-settings-icon"_spr);
		spr->setScale(0.25f);

		if (auto* bp = Loader::get()->getLoadedMod("tpdea.betterpause-Better")) {
			if (bp->getSettingValue<int64_t>("type-pause") == 1) {
				spr->setScale(0.95f);
			}
		}

		auto* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GwtPauseLayer::on_gwt_settings));
		btn->setID("gwt-settings-button"_spr);

		if (auto* menu = getChildByIDRecursive("left-button-menu")) {
			menu->addChild(btn);
			menu->updateLayout();
		}
	}
};
