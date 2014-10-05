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

class Configuration : public FileBasedDocument, public ValueTree::Listener
{
public:
    Configuration();
    ~Configuration();

    // Overridden methods for FileBasedDocument
    String getDocumentTitle() override;
    Result loadDocument(const File& file) override;
    Result saveDocument(const File& file) override;
    File   getLastDocumentOpened() override;
    void   setLastDocumentOpened(const File& file) override;
    
    void          setLastFailedFile(const File& file) { lastFailedFile = file; };
    const File&   getLastFailedFile()                 { return lastFailedFile; };
    const String& getLastError()                      { return lastError; };
    const String& getLastErrorDetails()               { return lastErrorDetails; };
    SaveResult    saveIfNeededAndUserAgrees(bool offerCancelOption);

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved) override;
    void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved) override;
    void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

    ValueTree    getHostParameters();
    ValueTree    getScopeParameters();
    ValueTree    getConfigurationRoot() { return configurationRoot; };
    ValueTree    getMapping();
    XmlElement&  getLayout(String& errorText, String& errorDetails, bool forceReload);

    PropertiesFile& getConfigurationProperties();
    
    String getDirectory();

    bool replaceConfiguration(const String& newFileName);

    enum ParameterTarget { host, scopeLocal };
    void addNewParameter(const ValueTree& paramValues, int targetIndex, ParameterTarget parameterTarget, UndoManager* um);

    void deleteMapping(const Identifier& mappingType, 
                       ValueTree& mappingToDelete,
                       UndoManager* um);
    void addNewMapping(const Identifier& mappingType,
                       const String& componentName, 
                       const String& parameterName, 
                       ValueTree& newMapping,
                       int targetIndex, 
                       UndoManager* um);

    StringArray& getComponentNames(const Identifier& componentType);
    void setupParameterLists(StringArray& parameterDescriptions, Array<var>& parameterNames, bool discreteOnly);
    void setupSettingLists(const String& parameterName, StringArray& settingNames, Array<var>& settingValues);

private:
    ValueTree  configurationRoot;
    ValueTree  loaderConfigurationRoot;
    XmlElement layoutXml;       // Layout XML loaded from definition file(s)
    XmlElement loaderLayoutXml; // Default Layout XML

    StringArray sliderNames;
    StringArray labelNames;
    StringArray comboBoxNames;
    StringArray tabbedComponentNames;
    StringArray textButtonNames;

    ScopedPointer<PropertiesFile> properties;

    File       lastFailedFile;
    String     lastError;
    String     lastErrorDetails;

    bool       layoutLoaded;
    
    static const char*  configurationFileExtension;
    static const String loaderConfiguration;
    static const String loaderLayout;

    void        setupConfigurationProperties();
    void        loadLoaderConfiguration();
    void        loadLoaderLayout();
    XmlElement& loadLayoutXml(String& errorText, String& errorDetails);
    
    void        setMissingDefaultValues();

    void        setupComponentNameArrays();
    void        tidyUpComponentArray(StringArray& arrayToTidy);
    void        getComponentNames(XmlElement& xml);

    static ValueTree getDefaultParameter();
    bool             parameterNameExists(const String& parameterName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Configuration)
};


#endif  // CONFIGURATION_H_INCLUDED
