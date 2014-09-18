/*
  ==============================================================================

    ConfigurationMenuBarModel.h
    Created: 14 Sep 2014 8:31:47am
    Author:  giles

  ==============================================================================
*/

#ifndef CONFIGURATIONMENUBARMODEL_H_INCLUDED
#define CONFIGURATIONMENUBARMODEL_H_INCLUDED

#include <JuceHeader.h>
#include "ConfigurationManager.h"

class ConfigurationMenuBarModel  : public MenuBarModel
{
public:
    ConfigurationMenuBarModel(ConfigurationManager& owner);

    StringArray getMenuBarNames();
    PopupMenu   getMenuForIndex (int topLevelMenuIndex, const String& menuName);
    void        menuItemSelected (int menuItemID, int topLevelMenuIndex);

    ConfigurationManager& configurationManager;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationMenuBarModel);
};



#endif  // CONFIGURATIONMENUBARMODEL_H_INCLUDED
