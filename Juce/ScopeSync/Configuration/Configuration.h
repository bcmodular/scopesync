/**
 * Class to handle the Configuration file and its ValueTree
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

#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED
#include <JuceHeader.h>
#include "../Configuration/ConfigurationPanel.h"
#include "../Windows/UserSettings.h"
#include "../Core/ScopeIDs.h"

class ScopeSync;

/* =========================================================================
 * NewConfigurationWindow: Container Window for NewConfigurationEditor
 */
class NewConfigurationWindow : public DocumentWindow
{
public:
    NewConfigurationWindow(int posX, int posY,
                           ScopeSync& ss,
                           const File& file,
                           ApplicationCommandManager* acm);
    ~NewConfigurationWindow();

    void addConfiguration() const;
    void cancel() const;

private:
    ScopeSync&     scopeSync;
    File           newFile;
    ValueTree      settings;
    bool           cancelled;
    Configuration& configuration;

    void closeButtonPressed() override;
    void restoreWindowPosition(int posX, int posY);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewConfigurationWindow);
};

/* =========================================================================
 * NewConfigurationEditor: Dialog for setting up a new Configuration
 */
class NewConfigurationEditor : public Component,
                               public ApplicationCommandTarget
{
public:
    NewConfigurationEditor(ScopeSync& ss, ValueTree& settings,
                           const String& filePath, ApplicationCommandManager* acm);
    ~NewConfigurationEditor();

    void paint(Graphics& g) override;
    void resized() override;

private:
    ConfigurationPanel panel;
    ApplicationCommandManager* commandManager;
    Label        filePathLabel;
    TextButton   addButton;
    TextButton   cancelButton;

    void addConfiguration() const;
    void cancel() const;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewConfigurationEditor);
};

/* =========================================================================
 * Configuration
 */
class Configuration : public FileBasedDocument, public ValueTree::Listener
{
public:
	Configuration();
    ~Configuration();

    // Overridden methods for FileBasedDocument
    String getDocumentTitle() override;
    Result loadDocument(const File& file) override;
    void   setMissingDefaultValues();

    Result saveDocument(const File& file) override;
    File   getLastDocumentOpened() override;
    void   setLastDocumentOpened(const File& file) override;
    
    void          setLastFailedFile(const File& file) { lastFailedFile = file; };
    const File&   getLastFailedFile() const { return lastFailedFile; };
    const String& getLastError() const { return lastError; };
    const String& getLastErrorDetails() const { return lastErrorDetails; };
    SaveResult    saveIfNeededAndUserAgrees(bool offerCancelOption);

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int oldIndex) override;
    void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

    ValueTree    getParameters() const;
    ValueTree    getConfigurationRoot() const { return configurationRoot; };
    ValueTree    getMapping() const;
    XmlElement&  getLayout(String& errorText, String& errorDetails, bool forceReload);

    PropertiesFile& getConfigurationProperties() const;
    
    String getConfigurationDirectory() const;
    int    getConfigurationUID();
    int    generateConfigurationUID();
    String getLayoutDirectory() const { return layoutDirectory; }

    void      createConfiguration(const File& filePath, const ValueTree& initialSettings);
    ValueTree getEmptyConfiguration() const;
    
    bool replaceConfiguration(const String& newFileName);
	void migrateFromV102();

	void addNewParameter(ValueTree& newParameter, const ValueTree& paramValues, int targetIndex, UndoManager* um) const;
    void updateParameterFromPreset(ValueTree& parameter, const ValueTree& preset, bool overwriteNames, UndoManager* undoManager) const;

    void deleteMapping(const Identifier& mappingType, 
                       ValueTree& mappingToDelete,
                       UndoManager* um) const;
    void addNewMapping(const Identifier& mappingType,
                       const String& componentName, 
                       const String& parameterName, 
                       ValueTree& newMapping,
                       int targetIndex, 
                       UndoManager* um) const;

    StringArray getComponentNames(const Identifier& componentType, bool checkStyleOverrideFlag = false);
    void setupParameterLists(StringArray& parameterDescriptions, Array<var>& parameterNames, bool discreteOnly) const;
    void setupSettingLists(const String& parameterName, StringArray& settingNames, Array<var>& settingValues) const;

    static Identifier getMappingParentId(const Identifier& componentType);
    static Identifier getComponentTypeId(const String& typeString);
    static String     getComponentTypeName(const Identifier& type);

    ValueTree getStyleOverride(const Identifier& componentType, const String& componentName) const;
    
    void deleteStyleOverride(const Identifier& componentType, 
                             ValueTree& styleOverrideToDelete,
                             UndoManager* um) const;

    void deleteAllStyleOverrides(const Identifier& componentType, const String& widgetTemplateId, UndoManager* um) const;
    
    void addStyleOverride(const Identifier& componentType,
                          const String&     componentName, 
                          const String&     widgetTemplateId,
                          ValueTree&        newStyleOverride,
                          int               targetIndex, 
                          UndoManager*      um) const;

    void addStyleOverrideToAll(const Identifier& componentType,
                               const String&     widgetTemplateId,
                               ValueTree&        newStyleOverride,
                               UndoManager*      um) const;
    
    static ValueTree getDefaultParameter();

    static ValueTree getDefaultFixedParameter();
    
private:
    ValueTree  configurationRoot;
    ValueTree  loaderConfigurationRoot;
    XmlElement layoutXml;       // Layout XML loaded from definition file(s)
    XmlElement loaderLayoutXml; // Default Layout XML
    String     layoutDirectory;

    StringArray sliderNames;
    StringArray labelNames;
    StringArray comboBoxNames;
    StringArray tabbedComponentNames;
    StringArray textButtonNames;

    ScopedPointer<PropertiesFile> properties;
	SharedResourcePointer<UserSettings> userSettings;
	SharedResourcePointer<ScopeCodeMapper> scopeCodeMapper;

    File       lastFailedFile;
    String     lastError;
    String     lastErrorDetails;

    bool       layoutLoaded;
    
    static const char*  configurationFileExtension;

    void        setConfigurationRoot(const ValueTree& newRoot);
    void        setupConfigurationProperties();
    void        loadLoaderConfiguration();
    void        loadLoaderLayout();
    XmlElement& loadLayoutXml(String& errorText, String& errorDetails);
    
    class ComponentLookupItem
    {
    public:
        ComponentLookupItem(const String& componentName, 
                            const Identifier& componentType, 
                            bool noStyleOverrideFlag,
                            const String& wtid)
            : name(componentName), type(componentType), noStyleOverride(noStyleOverrideFlag), widgetTemplateId(wtid) {}

        String     name;
        Identifier type;
        bool       noStyleOverride;
        String     widgetTemplateId;
    };

    class ComponentLookupSorter
    {
    public:
        static int compareElements(ComponentLookupItem* first, ComponentLookupItem* second)
        {
            int typeComparison = first->type.toString().compareNatural(second->type.toString());

            if (typeComparison == 0)
                return first->name.compareNatural(second->name);
            else
                return typeComparison;
        }
    };

    OwnedArray<ComponentLookupItem> componentLookup;

    void setupComponentLookup();
    void getComponentNamesFromXml(XmlElement& xml);
    bool componentInLookup(const Identifier& componentType, const String& componentName) const;

    bool parameterNameExists(const String& parameterName) const;
    void generateUniqueParameterNames(ValueTree& parameter, UndoManager* undoManager) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Configuration)
};


#endif  // CONFIGURATION_H_INCLUDED
