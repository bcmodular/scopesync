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
class PresetFile;
class BCMTreeView;
class PresetManagerWindow;
class PresetFileChooser;

/* =========================================================================
 * PresetManager: Regular version to display in Preset Manager window
 */
class PresetManager : public  Component,
                      public  ApplicationCommandTarget,
                      private Timer,
                      public  ChangeBroadcaster
{
public:
    PresetManager(File& pf, PresetManagerWindow& parent);
    ~PresetManager();

    void paint(Graphics& g) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component* child) override;
    void saveTreeViewState();
    void unload();
    void changePanel(Component* newComponent);

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    PresetFile&                getPresetFile()     { return *presetFile; };
    
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
    
    ScopedPointer<PresetFile>   presetFile;
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
                            public ChangeBroadcaster,
                            public ChangeListener
{
public:
    PresetManagerWindow(ApplicationCommandManager* acm, UndoManager& um, int posX, int posY);
    ~PresetManagerWindow();

    ApplicationCommandManager* getCommandManager() { return commandManager; };
    StringArray getMenuNames();
    void createMenu(PopupMenu& menu, const String& menuName);
    void createFileMenu(PopupMenu& menu);
    void createEditMenu(PopupMenu& menu);

    void addPresetFile();
    void save();
    void saveAs();
    void unload();
    void discardChanges();
    void restoreWindowPosition();
    void updatePresetLibrary();
    void hidePresetManager();

    void showPresetFileChooser();
    void showPresetManager();
    
    // Need to implement this...
    bool presetsHaveChanged() {return true;}

    enum PresetManagerAction { hideManager, showPreset, showPresetFiles};

    PresetManagerAction getNextAction() { return nextAction; }
    void setNextAction(PresetManagerAction action) { nextAction = action; }

private:
    ApplicationCommandManager*         commandManager;
    UndoManager&                       undoManager;
    File                               presetFile;
    PresetFileChooser*                 presetFileChooser;
    PresetManager*                     presetManager;
    ScopedPointer<PresetMenuBarModel>  menuModel;
    PresetManagerAction                nextAction;
    
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);
    void changeListenerCallback(ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagerWindow);
};

#endif  // PRESETMANAGER_H_INCLUDED
