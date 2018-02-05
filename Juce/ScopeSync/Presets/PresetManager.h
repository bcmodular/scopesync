/**
 * The Preset Manager allows users to set up and manage Parameter
 * Presets
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

#ifndef PRESETMANAGER_H_INCLUDED
#define PRESETMANAGER_H_INCLUDED

#include <JuceHeader.h>
#include "PresetFile.h"
#include "../Resources/ImageLoader.h"
#include "../Windows/UserSettings.h"

class BCMTreeView;
class PresetManagerWindow;
class PresetFileChooser;
class PresetFile;

/* =========================================================================
 * PresetManager: Regular version to display in Preset Manager window
 */
class PresetManager : public  Component,
                      public  ApplicationCommandTarget,
                      private Timer,
                      public  ActionListener
{
public:
    PresetManager(PresetManagerWindow& parent);
    ~PresetManager();

    void paint(Graphics& g) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component* child) override;
    void saveTreeViewState() const;
    void unload();
    void changePanel(Component* newComponent);

    ApplicationCommandManager* getCommandManager() const;

private:
    LookAndFeel_V3             lookAndFeel;
    Label                      fileNameLabel;
    ImageButton                addButton;
    ImageButton                openButton;
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
    PresetFile&                presetFile;
    SharedResourcePointer<ImageLoader> imageLoader;
	SharedResourcePointer<UserSettings> userSettings;

    ApplicationCommandManager*  commandManager;
    UndoManager                 undoManager;
    PresetManagerWindow&        parentWindow;

    bool initialised;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;

    static void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour, ImageLoader* imgLoader);

    void timerCallback() override;
    void actionListenerCallback(const String& message) override;
    
    void copyItem() const;
    void pasteItem() const;
    void deleteItems() const;
    void addItem() const;
    void addItemFromClipboard() const;

    void save() const;
    void saveAs() const;
    void undo();
    void redo();
    bool canPasteItem() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager);
};

/* =========================================================================
 * PresetMenuBarModel: Sets up the menu bar for the Preset Manager
 */
class PresetMenuBarModel  : public MenuBarModel
{
public:
    PresetMenuBarModel(PresetManagerWindow& owner);

    StringArray getMenuBarNames() override;
    PopupMenu   getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
    void        menuItemSelected(int /* menuItemID */, int /* topLevelMenuIndex */) override;;

    PresetManagerWindow& presetManagerWindow;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetMenuBarModel);
};

/* =========================================================================
 * PresetManagerWindow: Parent window for the Config Mgr
 */
class PresetManagerWindow : public DocumentWindow,
                            public ActionListener
{
public:
    PresetManagerWindow(const String& filePath,
                        ApplicationCommandManager* acm, 
                        UndoManager& um, 
                        int posX, int posY);
    ~PresetManagerWindow();

    ApplicationCommandManager* getCommandManager() const;
    static StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName) const;
    void createFileMenu(PopupMenu& menu) const;
    void createEditMenu(PopupMenu& menu) const;

    void addPresetFile();
    void unload();
    void discardChanges();
    void restoreWindowPosition();
    void updatePresetLibrary();
    void hidePresetManager(bool offerToSave = true);

    void showPresetFileChooser();
    void showPresetManager();
    
    void incrNumActions() { numActions += 1; }

    bool presetsHaveChanged() const;

    PresetFile& getPresetFile() { return presetFile; }

	UserSettings* getUserSettings() { return userSettings; }

private:
    ApplicationCommandManager*         commandManager;
    UndoManager&                       undoManager;
    PresetFile                         presetFile;
    PresetFileChooser*                 presetFileChooser;
    PresetManager*                     presetManager;
    ScopedPointer<PresetMenuBarModel>  menuModel;
    File                               newPresetFile;
    bool                               offerToSaveOnExit;

	SharedResourcePointer<UserSettings> userSettings;

    int numActions;

    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    void actionListenerCallback(const String& message) override;
    bool newPresetFileIsInLocation();

    static void alertBoxLaunchLocationEditor(int result, Rectangle<int> newConfigWindowPosition, PresetManagerWindow* pmw);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerWindow);
};

#endif  // PRESETMANAGER_H_INCLUDED
