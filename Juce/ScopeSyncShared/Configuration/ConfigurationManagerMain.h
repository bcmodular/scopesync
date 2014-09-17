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

class ConfigurationTree;
class PropertyListBuilder;

class ConfigurationManagerMain : public Component,
                                 public ApplicationCommandTarget,
                                 private Timer
{
public:
    ConfigurationManagerMain(ConfigurationManager& owner, ScopeSync& ss);
    ~ConfigurationManagerMain();

    void updateConfigurationFileName();
    void paint(Graphics& g) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component* child) override;
    
private:
    Label                      fileNameLabel;
    TextButton                 saveButton;
    TextButton                 saveAndCloseButton;
    TextButton                 saveAsButton;
    TextButton                 discardChangesButton;
    ScopedPointer<ConfigurationTree> treeView;
    Viewport                   panelView;
    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer treeSizeConstrainer;
    
    ScopeSync&                 scopeSync;
    ApplicationCommandManager& commandManager;
    ConfigurationManager&      configurationManager;
    UndoManager                undoManager;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerMain);
};




#endif  // CONFIGURATIONMANAGERMAIN_H_INCLUDED
