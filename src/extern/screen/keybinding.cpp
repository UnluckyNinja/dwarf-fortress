/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/screen/keybinding.hpp"

#include "extern/interface.hpp"
#include "extern/enabler.hpp"

#include "intern/utils/numbers.hpp"

BindingGroup::BindingGroup(const ::std::string& name_in, const interface_key_t start_in, const interface_key_t end_in) :
  name(name_in), start(start_in), end(end_in) {
}

::std::vector< BindingGroup > KeybindingScreen::groups;

KeybindingScreen::KeybindingScreen() {
  if (groups.empty()) {
    groups.push_back(BindingGroup("General", INTERFACEKEY_NONE, WORLDKEY_START - 1));
    groups.push_back(BindingGroup("World", WORLDKEY_START, ADVENTURERKEY_START - 1));
    groups.push_back(BindingGroup("Adventurer", ADVENTURERKEY_START, EMBARKKEY_START - 1));
    groups.push_back(BindingGroup("Dwarf mode", DWARFMAINKEY_START, MILITIAKEY_START - 1));
    groups.push_back(BindingGroup("Embark", EMBARKKEY_START, BUILDINGKEY_START - 1));
    groups.push_back(BindingGroup("Building", BUILDINGKEY_START, WORKSHOPKEY_START - 1));
    groups.push_back(BindingGroup("Workshop", WORKSHOPKEY_START, PILEZONEKEY_START - 1));
    groups.push_back(BindingGroup("Pilezone", PILEZONEKEY_START, STOCKORDERKEY_START - 1));
    groups.push_back(BindingGroup("Stockorder", STOCKORDERKEY_START, DWARFMAINKEY_START - 1));
    groups.push_back(BindingGroup("Militia", MILITIAKEY_START, INTERFACEKEY_STRING_A000 - 1));
    groups.push_back(BindingGroup("Text entry", INTERFACEKEY_STRING_A000, INTERFACEKEY_STRING_A255));
  }

  gview.addscreen(this, INTERFACE_PUSH_AT_BACK, NULL); // HACK
  mode = mode_main;

  main.add("Macros", sel_macros);
  for (::std::size_t i = 0; i < groups.size(); i++) {
    main.set(i + 2, groups[i].name, sel_first_group + i);
  }

  main.set(groups.size() + 3, "Save and exit", sel_save_exit);
  main.add("Exit, discard changes when DF quits", sel_just_exit);
  enabler.flag |= ENABLERFLAG_RENDER;
}
