/*
  ==============================================================================

    Presets.h
    Created: 2 Nov 2014 7:31:28am
    Author:  giles

  ==============================================================================
*/

#ifndef PRESETFILE_H_INCLUDED
#define PRESETFILE_H_INCLUDED

#include <JuceHeader.h>
#include "../Configuration/ConfigurationPanel.h"

class ScopeSync;
class PresetFile;

/* =========================================================================
 * NewPresetFileWindow: Container Window for NewPresetFileEditor
 */
class NewPresetFileWindow : public DocumentWindow,
                            public ChangeBroadcaster
{
public:
    NewPresetFileWindow(int posX, int posY,
                        PresetFile& pf,
                        UndoManager& um,
                        const File& file,
                        ApplicationCommandManager* acm);
    ~NewPresetFileWindow();

    void addPresetFile();
    void cancel();
    bool isCancelled() { return cancelled; }
    ValueTree getSettings();
    File      getNewFile() { return newFile; }

private:
    File         newFile;
    ValueTree    settings;
    bool         cancelled;
    PresetFile&  presetFile;
    UndoManager& undoManager;
                                         
    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewPresetFileWindow);
};

/* =========================================================================
 * PresetFilePanel: Edit Panel for Preset Files
 */
class PresetFilePanel : public BasePanel
{
public:
    PresetFilePanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm);
    ~PresetFilePanel();
    
private:
    void rebuildProperties() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetFilePanel)
};

/* =========================================================================
 * PresetPanel: Edit Panel for Presets
 */
class PresetPanel : public BasePanel,
                       public Value::Listener
{
public:
    PresetPanel(ValueTree& preset, UndoManager& um, PresetFile& pf,
                ApplicationCommandManager* acm);
    ~PresetPanel();

    void paintOverChildren(Graphics& g);
    void childBoundsChanged(Component* child) override;
    
private:
    PresetFile& presetFile;
    ScopedPointer<SettingsTable> settingsTable;
    Value valueType;
    
    ScopedPointer<ResizableEdgeComponent> resizerBar;
    ComponentBoundsConstrainer settingsTableConstrainer;

    void rebuildProperties() override;
    
    void createSettingsTable();
   
    void resized() override;
    
    void valueChanged(Value& valueThatChanged) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
};

/* =========================================================================
 * NewPresetFileEditor: Dialog for setting up a new Preset File
 */
class NewPresetFileEditor : public Component,
                            public ApplicationCommandTarget
{
public:
    NewPresetFileEditor(ValueTree&    settings, UndoManager& um,
                        const String& filePath, ApplicationCommandManager* acm);
    ~NewPresetFileEditor();

    void paint(Graphics& g) override;
    void resized() override;

private:
    PresetFilePanel panel;
    ApplicationCommandManager* commandManager;
    UndoManager& undoManager;
    Label        filePathLabel;
    TextButton   addButton;
    TextButton   cancelButton;

    void addPresetFile();
    void cancel();
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewPresetFileEditor);
};

/* =========================================================================
 * PresetFile
 */
class PresetFile : public FileBasedDocument, public ValueTree::Listener
{
public:
    PresetFile();
    ~PresetFile();

    // Overridden methods for FileBasedDocument
    String getDocumentTitle() override;
    Result loadDocument(const File& file) override;
    
    Result saveDocument(const File& file) override;
    File   getLastDocumentOpened() override;
    void   setLastDocumentOpened(const File& file) override;
    
    const String& getLastError()        { return lastError; };
    const String& getLastErrorDetails() { return lastErrorDetails; };
    SaveResult    saveIfNeededAndUserAgrees(bool offerCancelOption);

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved) override;
    void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved) override;
    void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

    ValueTree    getPresetFileRoot() { return presetFileRoot; };
    
    PropertiesFile& getPresetProperties();
    
    void createPresetFile(const File& filePath);

    void addNewPreset(ValueTree& newPreset, const ValueTree& presetValues, int targetIndex, UndoManager* um);

private:
    ValueTree  presetFileRoot;
    ScopedPointer<PropertiesFile> properties;

    String     lastError;
    String     lastErrorDetails;

    static const char*  presetFileFileExtension;
    
    void        setPresetFileRoot(const ValueTree& newRoot);
    void        setupPresetProperties();
    
    static ValueTree getDefaultPreset();
    bool             presetNameExists(const String& presetName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetFile)
};


#endif  // PRESETFILE_H_INCLUDED
