#pragma once
#include <Geode/loader/SettingNode.hpp>

using namespace geode::prelude;

class SettingSectionValue;

class SettingSectionValue : public SettingValue {
protected:
  std::string m_placeholder;

public:
  SettingSectionValue( std::string const &key, std::string const &modID, std::string const &placeholder )
      : SettingValue( key, modID ), m_placeholder( placeholder ) {}

  bool load( matjson::Value const &json ) override {
    return true;
  }
  bool save( matjson::Value &json ) const override {
    return true;
  }
  SettingNode *createNode( float width ) override;
};

class SettingSectionNode : public SettingNode {
protected:
  bool init( SettingSectionValue *value, float width ) {
    if ( ! SettingNode::init( value ) )
      return false;
    this->setContentSize( { width, 40.f } );
    auto menu = CCMenu::create();
    std::string sectionName = Mod::get()->getSettingDefinition( value->getKey() )->get<CustomSetting>()->json->get<std::string>( "name" );
    auto infoSpr = CCSprite::createWithSpriteFrameName( "GJ_infoIcon_001.png" );
    infoSpr->setScale( .7F );
    //auto infoBtn
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr,
        this,
        menu_selector( SettingSectionNode::onInfoBtn ) );
    infoBtn->setID( value->getKey() );
    infoBtn->setPositionX( Mod::get()->getSettingDefinition( value->getKey() )->get<CustomSetting>()->json->get<int>( "posX" ) );
    auto label = CCLabelBMFont::create( sectionName.c_str(), "goldFont.fnt" );

    label->setScale( .6F );
    menu->setPosition( width / 2, 23.f );
    menu->addChild( label );
    menu->addChild( infoBtn );
    this->addChild( menu );
    return true;
  }

  void onInfoBtn( CCObject *sender ) {
    // i dont want to deal with template hell
    auto node = reinterpret_cast<CCMenuItemSpriteExtra *>( sender );
    if ( node == nullptr ) return;
    FLAlertLayer::create( nullptr,
                          Mod::get()->getSettingDefinition( node->getID() )->get<CustomSetting>()->json->get<std::string>( "name" ).c_str(),
                          Mod::get()->getSettingDefinition( node->getID() )->get<CustomSetting>()->json->get<std::string>( "description" ).c_str(),
                          "OK",
                          nullptr,
                          Mod::get()->getSettingDefinition( node->getID() )->get<CustomSetting>()->json->get<int>( "scale" ) )
        ->show();
  }

public:
  void commit() override {
    // Let the UI know you have committed the value
    this->dispatchCommitted();
  }

  // Geode calls this to query if the setting value has been changed,
  // and those changes haven't been committed
  bool hasUncommittedChanges() override {
    return false;
  }

  // Geode calls this to query if the setting has a value that is
  // different from its default value
  bool hasNonDefaultValue() override {
    return true;
  }

  void resetToDefault() override {
  }
  static SettingSectionNode *create( SettingSectionValue *value, float width ) {
    auto ret = new SettingSectionNode;
    if ( ret && ret->init( value, width ) ) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE( ret );
    return nullptr;
  }
};
