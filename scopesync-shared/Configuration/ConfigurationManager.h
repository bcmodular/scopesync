/**
 * UI Component for managing the current Configuration. The
 * ConfigurationManager class is also the container window that
 * acts as the parent for the sub-components
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef CONFIGURATIONMANAGER_H_INCLUDED
#define CONFIGURATIONMANAGER_H_INCLUDED
#include <JuceHeader.h>
#include "../Core/ScopeSync.h"

class PropertyListBuilder;
class BCMTreeView;
class ScopeSync;

/* =========================================================================
 * ConfigurationManager: Regular version to display in Config Mgr window
 */
class ConfigurationManager : public  Component,
                             public  ApplicationCommandTarget,
                             private Timer,
                             public  ChangeListener
{
public:
    ConfigurationManager(ScopeSync& ss, ConfigurationManagerWindow& parent);
    ~ConfigurationManager();

    void paint(Graphics& g) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component* child) override;
    void saveTreeViewState() const;
    void unload();
    void changePanel(Component* newComponent);
    Component* createParameterPanelComponent(ValueTree& tree);
    static Component* createParameterPanelComponent(ValueTree& tree, 
                                                    ScopeSync& scopeSync, 
                                                    UndoManager& undoManager, 
                                                    ApplicationCommandManager* commandManager,
                                                    ChangeListener* listener);
    ApplicationCommandManager* getCommandManager() const { return commandManager; };
    Configuration&             getConfiguration() const { return scopeSync.getConfiguration(); };
    ScopeSync&                 getScopeSync() const { return scopeSync; };

private:
    LookAndFeel_V3             lookAndFeel;
    Label                      fileNameLabel;
    ImageButton                addButton;
    ImageButton                saveButton;
    ImageButton                saveAsButton;
    ImageButton                applyChangesButton;
    ImageButton                discardChangesButton;
    ImageButton                undoButton;
    ImageButton                redoButton;
    ScopedPointer<BCMTreeView> treeView;
    ScopedPointer<Component>   panel;
    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer treeSizeConstrainer;
    SharedResourcePointer<ImageLoader> imageLoader;

    ScopeSync&                  scopeSync;
    ApplicationCommandManager*  commandManager;
    UndoManager                 undoManager;
    ConfigurationManagerWindow& parentWindow;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;

	static void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour, ImageLoader* imgLoader);

    void timerCallback() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    void undo();
    void redo();
    bool canPasteItem() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManager);
};

/* =========================================================================
 * ConfigurationManagerCallout: Version to show in Callout box
 */
class ConfigurationManagerCallout : public Component,
                                    public Timer,
                                    public ApplicationCommandTarget,
                                    public ChangeListener
{
public:
    ConfigurationManagerCallout(ScopeSync& ss, int width, int height);
    ~ConfigurationManagerCallout();

    void paint(Graphics& g) override;
    void resized() override;
    void changePanel(Component* newComponent);

    void timerCallback() override;
    int  getNumActions() const { return numActions; }

private:
    LookAndFeel_V3             lookAndFeel;
    ScopedPointer<Component>   panel;
    
    ScopeSync&                 scopeSync;
    UndoManager&               undoManager;
    ApplicationCommandManager* commandManager;
    
    int numActions;

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    void undo();
    void redo();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerCallout);
};

/* =========================================================================
 * ConfigurationMenuBarModel: Sets up the menu bar for the Config Mgr
 */
class ConfigurationMenuBarModel  : public MenuBarModel
{
public:
    ConfigurationMenuBarModel(ConfigurationManagerWindow& owner);

    StringArray getMenuBarNames() override;
    PopupMenu   getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
    void        menuItemSelected(int /* menuItemID */, int /* topLevelMenuIndex */) override {};

    ConfigurationManagerWindow& configurationManagerWindow;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationMenuBarModel);
};

/* =========================================================================
 * ConfigurationManagerWindow: Parent window for the Config Mgr
 */
class ConfigurationManagerWindow : public DocumentWindow
{
public:
    ConfigurationManagerWindow(ScopeSync& owner, int posX, int posY);
    ~ConfigurationManagerWindow();

    ApplicationCommandManager* getCommandManager() const { return commandManager; };

	static StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName) const;
    void createFileMenu(PopupMenu& menu) const;
    void createEditMenu(PopupMenu& menu) const;

    void addConfig() const;
    void save() const;
    void saveAs() const;
    void unload();
    void refreshContent();
    void reloadConfiguration() const;
    void restoreWindowPosition();

private:
    ApplicationCommandManager*               commandManager;
    ScopeSync&                               scopeSync;
    ConfigurationManager*                    configurationManager;
    ScopedPointer<ConfigurationMenuBarModel> menuModel;
    
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerWindow);
};

/* =========================================================================
 * ConfigurationManagerCalloutWindow: Mini-version of the Config Mgr for a
 *                                    call-out box
 */
class ConfigurationManagerCalloutWindow : public Component,
                                          public ChangeBroadcaster
{
public:
    ConfigurationManagerCalloutWindow(ScopeSync& owner, int width, int height);
    ~ConfigurationManagerCalloutWindow();

    void setMappingPanel(ValueTree& mapping, const Identifier& compType, const String& compName) const;
    void setParameterPanel(ValueTree& parameter) const;
    void setStyleOverridePanel(ValueTree& styleOverride,
                               const Identifier& componentType, 
                               const String& componentName, 
                               const String& widgetTemplateId, 
                               const String& fillColour  = String(),
                               const String& lineColour  = String(),
                               const String& fillColour2 = String(),
                               const String& lineColour2 = String()) const;
    
private:
    ScopeSync&                 scopeSync;
    UndoManager&               undoManager;
    ApplicationCommandManager* commandManager;
    ScopedPointer<ConfigurationManagerCallout>  configurationManagerCallout;

    void paint(Graphics& g) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationManagerCalloutWindow);
};

#endif  // CONFIGURATIONMANAGER_H_INCLUDED
