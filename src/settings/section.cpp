#include "section.hpp"
#include <Geode/loader/Dirs.hpp>
#include <filesystem>

SettingNode *SettingSectionValue::createNode( float width ) {
  return SettingSectionNode::create( this, width );
}
