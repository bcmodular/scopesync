/**
 * Classes used in managing Parameter Preset files
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

#ifndef PRESETFILE_H_INCLUDED
#define PRESETFILE_H_INCLUDED

#include <JuceHeader.h>
#include "../Configuration/ConfigurationPanel.h"
#include "../Windows/UserSettings.h"

class ScopeSync;
class PresetFile;

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

    void paintOverChildren(Graphics& g) override;
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

    const String& getLastError() const;
    const String& getLastErrorDetails() const;
    SaveResult    saveIfNeededAndUserAgrees(bool offerCancelOption);

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int oldIndex) override;
    void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

    ValueTree getPresetFileRoot() const;
    
    PropertiesFile& getPresetProperties() const;
    
    void createPresetFile();

    void addNewPreset(ValueTree& newPreset, const ValueTree& presetValues, int targetIndex, UndoManager* um);

private:
    ValueTree  presetFileRoot;
    ScopedPointer<PropertiesFile> properties;
	SharedResourcePointer<UserSettings> userSettings;

    String     lastError;
    String     lastErrorDetails;

    static const char*  presetFileFileExtension;
    
    void        setPresetFileRoot(const ValueTree& newRoot);
    void        setupPresetProperties();
    
    static ValueTree getDefaultPreset();
    bool             presetNameExists(const String& presetName) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetFile)
};


#endif  // PRESETFILE_H_INCLUDED
