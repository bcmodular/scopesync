/*
  ==============================================================================

    ConfigurationManagerMain.h
    Created: 14 Sep 2014 9:24:07am
    Author:  giles

  ==============================================================================
*/

#ifndef CONFIGURATIONMANAGERMAIN_H_INCLUDED
#define CONFIGURATIONMANAGERMAIN_H_INCLUDED

#include <JuceHeader.h>
#include "../Core/ScopeSyncGUI.h"
#include "ConfigurationManager.h"

class PropertyListBuilder;

class ConfigurationManagerMain : public Component,
                                 public ApplicationCommandTarget
{
public:
    ConfigurationManagerMain(ConfigurationManager& owner, ScopeSyncGUI& gui);
    ~ConfigurationManagerMain();

    void updateConfigurationFileName();

private:
    Label                      fileNameLabel;
    PropertyPanel              propertyPanel;
    TextButton                 saveButton;
    TextButton                 saveAndCloseButton;
    TextButton                 saveAsButton;
    TextButton                 discardChangesButton;
    ScopeSyncGUI&              scopeSyncGUI;
    ApplicationCommandManager& commandManager;
    ConfigurationManager&      configurationManager;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void rebuildProperties();
    void createPropertyEditors(PropertyListBuilder& propertyPanel);
    void resized();
    void paint(Graphics& g);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerMain);
};




#endif  // CONFIGURATIONMANAGERMAIN_H_INCLUDED
