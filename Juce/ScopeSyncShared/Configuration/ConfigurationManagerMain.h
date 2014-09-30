/**
 * Main display component for the Configuration Manager. Contains
 * the TreeView and edit Panels
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * ScopeSync is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ScopeSync.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  Simon Russell
 *  Will Ellis
 *  Jessica Brandt
 */

#ifndef CONFIGURATIONMANAGERMAIN_H_INCLUDED
#define CONFIGURATIONMANAGERMAIN_H_INCLUDED

#include <JuceHeader.h>
#include "../Core/ScopeSyncGUI.h"
#include "ConfigurationManager.h"

class ConfigurationTree;
class PropertyListBuilder;

class ConfigurationManagerMain : public  Component,
                                 public  ApplicationCommandTarget,
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
    void saveTreeViewState();
    void unload();
    void changePanel(Component* newComponent);

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    Configuration&             getConfiguration()  { return configurationManager.getConfiguration(); };

private:
    LookAndFeel_V3             lookAndFeel;
    Label                      fileNameLabel;
    ImageButton                saveButton;
    ImageButton                saveAsButton;
    ImageButton                applyChangesButton;
    ImageButton                discardChangesButton;
    ImageButton                undoButton;
    ImageButton                redoButton;
    ScopedPointer<ConfigurationTree> treeView;
    ScopedPointer<Component>   panel;
    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer treeSizeConstrainer;
    
    ScopeSync&                 scopeSync;
    ApplicationCommandManager* commandManager;
    ConfigurationManager&      configurationManager;
    UndoManager                undoManager;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour);

    void timerCallback() override;
    void undo();
    void redo();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerMain);
};




#endif  // CONFIGURATIONMANAGERMAIN_H_INCLUDED
