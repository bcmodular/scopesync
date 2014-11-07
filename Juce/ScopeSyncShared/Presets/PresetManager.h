/*
  ==============================================================================

    PresetManager.h
    Created: 2 Nov 2014 10:12:07am
    Author:  giles

  ==============================================================================
*/

#ifndef PRESETMANAGER_H_INCLUDED
#define PRESETMANAGER_H_INCLUDED

#include <JuceHeader.h>
#include "PresetFile.h"
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
    void saveTreeViewState();
    void unload();
    void changePanel(Component* newComponent);

    ApplicationCommandManager* getCommandManager() { return commandManager; };

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
    
    ApplicationCommandManager*  commandManager;
    UndoManager                 undoManager;
    PresetManagerWindow&        parentWindow;

    bool initialised;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour);

    void timerCallback() override;
    void actionListenerCallback(const String& message) override;
    
    void copyItem();
    void pasteItem();
    void deleteItems();
    void addItem();
    void addItemFromClipboard();

    void save();
    void saveAs();
    void undo();
    void redo();
    bool canPasteItem();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager);
};

/* =========================================================================
 * PresetMenuBarModel: Sets up the menu bar for the Preset Manager
 */
class PresetMenuBarModel  : public MenuBarModel
{
public:
    PresetMenuBarModel(PresetManagerWindow& owner);

    StringArray getMenuBarNames();
    PopupMenu   getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void        menuItemSelected(int /* menuItemID */, int /* topLevelMenuIndex */) {};

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

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName);
    void createFileMenu(PopupMenu& menu);
    void createEditMenu(PopupMenu& menu);

    void addPresetFile();
    void unload();
    void discardChanges();
    void restoreWindowPosition();
    void updatePresetLibrary();
    void hidePresetManager();

    void showPresetFileChooser();
    void showPresetManager();
    
    void incrNumActions() { numActions += 1; }

    bool presetsHaveChanged();

    PresetFile& getPresetFile() { return presetFile; }

private:
    ApplicationCommandManager*         commandManager;
    UndoManager&                       undoManager;
    PresetFile                         presetFile;
    PresetFileChooser*                 presetFileChooser;
    PresetManager*                     presetManager;
    ScopedPointer<PresetMenuBarModel>  menuModel;
    File                               newPresetFile;

    int numActions;

    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    void actionListenerCallback(const String& message);
    bool newPresetFileIsInLocation();

    static void alertBoxLaunchLocationEditor(int result, Rectangle<int> newConfigWindowPosition, PresetManagerWindow* pmw);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerWindow);
};

#endif  // PRESETMANAGER_H_INCLUDED
