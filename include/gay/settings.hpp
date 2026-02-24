#pragma once

#include <string_view>
#include <utility>

#include <gay/types.hpp>

#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>

namespace gay::settings {
	enum class CheckMode {
		All,
		Any,
		None,
	};

	template<typename T>
	inline T get(std::string_view name) {
		return geode::Mod::get()->getSettingValue<T>(std::string(name));
	}

	template<CheckMode Mode, typename... T>
	inline bool check(const T&... settings) {
		if constexpr (Mode == CheckMode::All) {
			return (get<bool>(settings) && ...);
		} else if constexpr (Mode == CheckMode::Any) {
			return (get<bool>(settings) || ...);
		} else if constexpr (Mode == CheckMode::None) {
			return !(get<bool>(settings) || ...);
		} else {
			std::unreachable();
		}
	}

	inline float get_float(std::string_view name) {
		return static_cast<float>(geode::Mod::get()->getSettingValue<double>(std::string(name)));
	}

	inline bool is_enabled() {
		return get<bool>("enabled");
	}
} // namespace gay::settings

class ColorListSetting: public geode::SettingBaseValueV3<gay::ColorList> {
  public:
	static geode::Result<std::shared_ptr<geode::SettingV3>>
	parse(const std::string& key, const std::string& mod_id, const matjson::Value& json);
	geode::SettingNodeV3* createNode(float width) override;
};

class ColorListSettingNode: public geode::SettingValueNodeV3<ColorListSetting> {
  protected:
	bool init(std::shared_ptr<ColorListSetting> setting, float width);
	void on_view(cocos2d::CCObject*);

  public:
	static ColorListSettingNode* create(std::shared_ptr<ColorListSetting>, float width);
};

template<>
struct geode::SettingTypeForValueType<gay::ColorList> {
	using SettingType = ColorListSetting;
};

template<>
struct matjson::Serialize<gay::ColorEntry> {
	static geode::Result<gay::ColorEntry> fromJson(const matjson::Value& value) {
		if (!value["hex"].isString() || !value["user_added"].isBool() || !value["enabled"].isBool()) {
			return geode::Err("Invalid ColorEntry JSON");
		}
		return geode::Ok(
			gay::ColorEntry(value["hex"].asString().unwrap(), value["user_added"].asBool().unwrap(), value["enabled"].asBool().unwrap())
		);
	}

	static matjson::Value toJson(const gay::ColorEntry& value) {
		return matjson::makeObject({{"hex", value.hex}, {"user_added", value.user_added}, {"enabled", value.enabled}});
	}
};

template<>
struct matjson::Serialize<gay::ColorList> {
	static geode::Result<gay::ColorList> fromJson(const matjson::Value& value) {
		gay::ColorList ret;
		for (const auto& elem : value) {
			GEODE_UNWRAP_INTO(gay::ColorEntry color, matjson::Serialize<gay::ColorEntry>::fromJson(elem));
			ret.colors.push_back(std::move(color));
		}
		return geode::Ok(ret);
	}

	static matjson::Value toJson(const gay::ColorList& value) {
		auto arr = matjson::Value::array();
		for (const auto& elem : value.colors) {
			arr.push(matjson::Serialize<gay::ColorEntry>::toJson(elem));
		}

		return arr;
	}
};
