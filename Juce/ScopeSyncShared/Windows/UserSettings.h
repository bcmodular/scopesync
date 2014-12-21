//==============================================================================
/**
 * User Settings popup handler.
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

#ifndef USERSETTINGS_H_INCLUDED
#define USERSETTINGS_H_INCLUDED

#include <JuceHeader.h>

class FileLocationEditorWindow;
class PresetManagerWindow;

class UserSettings  : public  Component,
                      public  Value::Listener,
                      public  ApplicationCommandTarget,
                      public  Timer,
                      public  ActionBroadcaster
{
public:
    UserSettings ();
    ~UserSettings();

    void show(int posX, int posY);
    void hide();

    ValueTree getFileLocations();
    ValueTree getLayoutLibrary();
    ValueTree getConfigurationLibrary();
    ValueTree getPresetLibrary();
    ValueTree getLayoutFromFilePath(const String& filePath, const ValueTree& layoutLibrary);
    String    getLayoutFilename(const String& name, const String& librarySet);
    ValueTree getConfigurationFromFilePath(const String& filePath, const ValueTree& configurationLibrary);
    String    getConfigurationFilePathFromUID(int uid);
    ValueTree getPresetFileFromFilePath(const String& filePath, const ValueTree& presetLibrary);
    ValueTree getPresetFromNameAndFilePath(const String& name, const String& filePath, const ValueTree& presetLibrary);
    
    void editFileLocations(int posX, int posY);
    void hideFileLocationsWindow();
    
    void showPresetManagerWindow(const String& filePath, int posX, int posY);
    void hidePresetManagerWindow();
    
    void setLastTimeLayoutLoaded(const String& filePath);
    void setLastTimeConfigurationLoaded(const String& filePath);
    void rebuildFileLibrary(bool scanConfigurations = true, bool scanLayouts = true, bool scanPresets = true);
    void updateFileLocations(const ValueTree& fileLocations);
    
    void updateConfigurationLibraryEntry(const String&    filePath,
                                         const String&    fileName,
                                         const ValueTree& sourceValueTree);

    void updatePresetLibraryEntry(const String&    filePath,
                                  const String&    fileName,
                                  const ValueTree& sourceValueTree);

    PropertiesFile* getAppProperties();
    PropertiesFile* getGlobalProperties();

    int  getPropertyIntValue(const String& propertyName, int defaultValue);
    void setPropertyIntValue(const String& propertyName, int newValue);
    bool getPropertyBoolValue(const String& propertyName, bool defaultValue);
    void setPropertyBoolValue(const String& propertyName, bool newValue);

    Array<Colour> swatchColours;

    class ColourSelectorWithSwatches : public ColourSelector
    {
    public:
        ColourSelectorWithSwatches() {}

        int getNumSwatches() const override;
        Colour getSwatchColour (int index) const override;
        void setSwatchColour (int index, const Colour& newColour) const override;
    };

    juce_DeclareSingleton (UserSettings, false)

private:
    ScopedPointer<ApplicationCommandManager> commandManager;
    UndoManager           undoManager;
    ApplicationProperties appProperties;
    ApplicationProperties globalProperties;
    PropertyPanel         propertyPanel;
    TextButton            fileLocationsButton;
    TextButton            presetManagerButton;
    ScopedPointer<FileLocationEditorWindow> fileLocationEditorWindow;
    ScopedPointer<PresetManagerWindow>      presetManagerWindow;
    
    Value     useImageCache;
    Value     tooltipDelayTime;
	Value     autoRebuildLibrary;
	Value     useOSC;
    
    void userTriedToCloseWindow() override;
    void paint (Graphics& g) override;
    void resized() override;
    
    ValueTree getValueTreeFromGlobalProperties(const String& valueTreeToGet);
    
    void setupPanel();
    
    void updateLayoutLibraryEntry(const String&     filePath,
                                  const XmlElement& sourceXmlElement,
                                  ValueTree&        layoutLibrary);
    void updateConfigurationLibraryEntry(const String&    filePath,
                                         const String&    fileName,
                                         const ValueTree& sourceValueTree,
                                               ValueTree& configurationLibrary);
    void updatePresetLibraryEntry(const String&    filePath,
                                  const String&    fileName,
                                  const ValueTree& sourceValueTree,
                                        ValueTree& presetLibrary);
    void updatePresets(const String&    filePath,
                       const String&    fileName,
                       const ValueTree& sourceValueTree,
                             ValueTree& presetLibrary);

    void timerCallback();

    void valueChanged(Value& valueThatChanged) override;

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void loadSwatchColours();
    void saveSwatchColours();

    class RebuildFileLibrary : public ThreadWithProgressWindow
    {
    public:
        RebuildFileLibrary(const ValueTree& locations, 
                           bool scanConfigurations = true, 
                           bool scanLayouts = true, 
                           bool scanPresets = true)
            : ThreadWithProgressWindow("Rebuilding Library...", true, true),
              fileLocations(locations),
              rebuildConfigurations(scanConfigurations),
              rebuildLayouts(scanLayouts),
              rebuildPresets(scanPresets)
        {}

        void run();

    private:
        ValueTree fileLocations;

        bool rebuildConfigurations;
        bool rebuildLayouts;
        bool rebuildPresets;
    
        void updateLayoutLibrary(const Array<File>& layoutFiles);
        void updateConfigurationLibrary(const Array<File>& configurationFiles);
        void updatePresetLibrary(const Array<File>& presetFiles);
        void trimMissingFiles(const Array<File>& activeFiles, ValueTree& libraryToTrim);
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RebuildFileLibrary)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserSettings)
};

#endif   // USERSETTINGS_H_INCLUDED
