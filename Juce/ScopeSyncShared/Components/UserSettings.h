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

class UserSettings  : public  Component,
                      public  Value::Listener,
                      private ValueTree::Listener,
                      public  ApplicationCommandTarget,
                      public  Timer,
                      public  ChangeListener
{
public:
    UserSettings ();
    ~UserSettings();

    void show(int posX, int posY);
    void hide();

    void   changeListenerCallback(ChangeBroadcaster* source);
    void   rebuildFileLibrary();
    String getLayoutFilename(const String& name, const String& librarySet);
    const ValueTree& getLayoutLibrary() { return layoutLibrary; }
    const ValueTree& getConfigurationLibrary() { return configurationLibrary; }
    void getConfigurationFromFilePath(const String& filePath, ValueTree& configuration);

    void setLastTimeLayoutLoaded(const String& filePath);
    void setLastTimeConfigurationLoaded(const String& filePath);
    void updateConfigurationLibraryEntry(const String& filePath, const ValueTree& sourceValueTree);

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
    ScopedPointer<FileLocationEditorWindow> fileLocationEditorWindow;

    Value     useImageCache;
    Value     tooltipDelayTime;
    ValueTree fileLocations;
    ValueTree layoutLibrary;
    ValueTree configurationLibrary;
    
    void userTriedToCloseWindow() override;
    void paint (Graphics& g) override;
    void resized() override;
    
    void flushLibraryToFile();

    void setupPanel();
    void editFileLocations();
    void updateFileLocations();
    void addLayoutsToLibrary(const File& location);
    void addConfigurationsToLibrary(const File& location);

    void timerCallback();

    void valueChanged(Value& valueThatChanged) override;

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override { updateFileLocations(); };
    void valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) override { updateFileLocations(); };
    void valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved */) override { updateFileLocations(); };
    void valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */) override { updateFileLocations(); };
    void valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) override { updateFileLocations(); };

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    void loadSwatchColours();
    void saveSwatchColours();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserSettings)
};

#endif   // USERSETTINGS_H_INCLUDED
