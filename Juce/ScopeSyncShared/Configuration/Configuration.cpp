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

#include "Configuration.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/Global.h"
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"
#include "../Utils/BCMMath.h"
#include "../Windows/UserSettings.h"
#include "../Configuration/ConfigurationPanel.h"

/* =========================================================================
 * NewConfigurationWindow
 */
NewConfigurationWindow::NewConfigurationWindow(int posX, int posY,
                                               ScopeSync& ss,
                                               const File& file,
                                               ApplicationCommandManager* acm)
    : DocumentWindow("New Configuration",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      newFile(file),
      configuration(ss.getConfiguration())
{
    cancelled = true;

    setUsingNativeTitleBar (true);
    
    settings = ValueTree(Ids::configuration);

    setContentOwned(new NewConfigurationEditor(ss, settings, file.getFullPathName(), acm), true);
    
    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

NewConfigurationWindow::~NewConfigurationWindow() {}

void NewConfigurationWindow::addConfiguration()
{
    cancelled = false;
    sendChangeMessage();
}

ValueTree NewConfigurationWindow::getSettings()
{
    bool includeScopeLocal = settings.getProperty(Ids::includeScopeLocal, true);

    ValueTree newSettings = configuration.getEmptyConfiguration(includeScopeLocal);

    newSettings.copyPropertiesFrom(settings, nullptr);
    newSettings.removeProperty(Ids::includeScopeLocal, nullptr);

    return newSettings;
}

void NewConfigurationWindow::cancel() { sendChangeMessage(); }

void NewConfigurationWindow::closeButtonPressed() { sendChangeMessage(); }

void NewConfigurationWindow::restoreWindowPosition(int posX, int posY)
{
    setCentrePosition(posX, posY);
}

/* =========================================================================
 * NewConfigurationEditor
 */
NewConfigurationEditor::NewConfigurationEditor(ScopeSync& ss,
                                               ValueTree& settings,
                                               const String& filePath,
                                               ApplicationCommandManager* acm)
    : commandManager(acm),
      addButton("Add Configuration"),
      cancelButton("Cancel"),
      filePathLabel("File Path"),
      panel(settings, ss.getUndoManager(), ss, acm, true)
{
    commandManager->registerAllCommandsForTarget(this);

    addButton.setCommandToTrigger(commandManager, CommandIDs::addConfig, true);
    addAndMakeVisible(addButton);

    cancelButton.setCommandToTrigger(commandManager, CommandIDs::cancel, true);
    addAndMakeVisible(cancelButton);

    filePathLabel.setText(filePath, dontSendNotification);
    filePathLabel.setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(filePathLabel);

    addAndMakeVisible(panel);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 600, 400);
}

NewConfigurationEditor::~NewConfigurationEditor() {}

void NewConfigurationEditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}
    
void NewConfigurationEditor::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromBottom(30));
    
    addButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    cancelButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    filePathLabel.setBounds(localBounds.removeFromBottom(30).reduced(3, 3));
    panel.setBounds(localBounds.reduced(4, 4));
}

void NewConfigurationEditor::addConfiguration()
{
    NewConfigurationWindow* parent = dynamic_cast<NewConfigurationWindow*>(getParentComponent());
    parent->addConfiguration();
}

void NewConfigurationEditor::cancel()
{
    NewConfigurationWindow* parent = dynamic_cast<NewConfigurationWindow*>(getParentComponent());
    parent->cancel();
}

void NewConfigurationEditor::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::addConfig,
                             CommandIDs::cancel
                             };
    
    commands.addArray(ids, numElementsInArray (ids));
}

void NewConfigurationEditor::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addConfig:
        result.setInfo("Add Configuration", "Create a new Configuration", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::cancel:
        result.setInfo("Cancel", "Cancel current action", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('q', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool NewConfigurationEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addConfig:  addConfiguration(); break;
        case CommandIDs::cancel:     cancel(); break;
        default:                     return false;
    }

    return true;
}

ApplicationCommandTarget* NewConfigurationEditor::getNextCommandTarget()
{
    return nullptr;
}

/* =========================================================================
 * Configuration
 */
Configuration::Configuration(): FileBasedDocument(configurationFileExtension,
                                                      String ("*") + configurationFileExtension,
                                                      "Choose a Configuration file to load",
                                                      "Save Configuration"),
                                                  configurationRoot("configuration"),
                                                  layoutXml(Ids::layout),
                                                  loaderLayoutXml(Ids::layout)
{
    lastFailedFile = File();
    loadLoaderConfiguration();
    setConfigurationRoot(loaderConfigurationRoot);
    loadLoaderLayout();
    layoutXml = loaderLayoutXml;
    layoutLoaded = true;
    setChangedFlag(false);
    configurationRoot.addListener(this);
}

Configuration::~Configuration()
{
    configurationRoot.removeListener(this);
};

void Configuration::setConfigurationRoot(const ValueTree& newRoot)
{
    configurationRoot.removeListener(this);
    configurationRoot = newRoot;
    configurationRoot.addListener(this);
    setupConfigurationProperties();
}

void Configuration::setMissingDefaultValues()
{
    // Make sure we have a correct UID for this Configuration
    generateConfigurationUID();

    if (!(configurationRoot.hasProperty(Ids::ID)))
        configurationRoot.setProperty(Ids::ID, createAlphaNumericUID(), nullptr);

    configurationRoot.getOrCreateChildWithName(Ids::hostParameters, nullptr);
    configurationRoot.getOrCreateChildWithName(Ids::scopeParameters, nullptr);

    ValueTree mapping(configurationRoot.getOrCreateChildWithName(Ids::mapping, nullptr));
    mapping.getOrCreateChildWithName(Ids::sliders, nullptr);
    mapping.getOrCreateChildWithName(Ids::textButtons, nullptr);
    mapping.getOrCreateChildWithName(Ids::labels, nullptr);
    mapping.getOrCreateChildWithName(Ids::comboBoxes, nullptr);
    mapping.getOrCreateChildWithName(Ids::tabbedComponents, nullptr);

    ValueTree styleOverrides(configurationRoot.getOrCreateChildWithName(Ids::styleOverrides, nullptr));
    styleOverrides.getOrCreateChildWithName(Ids::components, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::sliders, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::textButtons, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::labels, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::comboBoxes, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::tabbedComponents, nullptr);
    styleOverrides.getOrCreateChildWithName(Ids::tabs, nullptr);
}

const char* Configuration::configurationFileExtension = ".configuration";

void Configuration::loadLoaderConfiguration()
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(emptyWithScopeLocalConfiguration));

    ValueTree newTree (ValueTree::fromXml(*xml));
    newTree.setProperty(Ids::name, "No configuration loaded...", nullptr);
    newTree.setProperty(Ids::readOnly, true, nullptr);
    newTree.setProperty(Ids::excludeFromChooser, true, nullptr);
    newTree.setProperty(Ids::UID, 0, nullptr);

    loaderConfigurationRoot = newTree;
}

ValueTree Configuration::getEmptyConfiguration(bool includeScopeLocal)
{
    String xmlToParse;

    if (includeScopeLocal)
        xmlToParse = emptyWithScopeLocalConfiguration;
    else
        xmlToParse = emptyConfiguration;

    ScopedPointer<XmlElement> xml(XmlDocument::parse(xmlToParse));

    return ValueTree::fromXml(*xml);
}

void Configuration::loadLoaderLayout()
{
    ScopedPointer<XmlElement> configElement;

    XmlDocument configurationDocument(loaderLayout);
    configElement   = configurationDocument.getDocumentElement();
    loaderLayoutXml = *configElement;
}

String Configuration::getDocumentTitle()
{
    return configurationRoot.getProperty(Ids::name, String::empty);
}

void Configuration::setupConfigurationProperties()
{
    const String filename ("Configuration_" + String(configurationRoot[Ids::ID]));

    PropertiesFile::Options options;
    options.applicationName     = filename;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    properties = new PropertiesFile(options);
}

PropertiesFile& Configuration::getConfigurationProperties()
{
    return *properties;
}

Result Configuration::loadDocument(const File& file)
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(file));

    if (xml == nullptr || !(xml->hasTagName(Ids::configuration)))
        return Result::fail("Not a valid ScopeSync Configuration");

    ValueTree newTree (ValueTree::fromXml(*xml));

    if (!newTree.hasType(Ids::configuration))
        return Result::fail("The document contains errors and couldn't be parsed");

    setConfigurationRoot(newTree);
    
    layoutLoaded = false;

    return Result::ok();
}

String Configuration::getConfigurationDirectory()
{
    if (getFile() != File::nonexistent)
        return getFile().getParentDirectory().getFullPathName();
    else
        return String::empty;
}

int Configuration::getConfigurationUID()
{
    if (configurationRoot.hasProperty(Ids::UID))
        return configurationRoot.getProperty(Ids::UID);
    else
        return generateConfigurationUID();
}

int Configuration::generateConfigurationUID()
{
    int uid = 0;

    String stringToHash = configurationRoot.getProperty(Ids::name);
    stringToHash += configurationRoot.getProperty(Ids::librarySet).toString();
    stringToHash += getFile().getFileName();
    stringToHash += configurationRoot.getProperty(Ids::author).toString();

    uid = stringToHash.hashCode();
    
    int currentUID = configurationRoot.getProperty(Ids::UID);

    if (currentUID != uid)
        configurationRoot.setProperty(Ids::UID, uid, nullptr);

    return uid;
}

void Configuration::createConfiguration(const File& newFile, const ValueTree& initialSettings)
{
    if (saveIfNeededAndUserAgrees(true) == savedOk)
    {
        setFile(newFile);
        lastFailedFile = File();

        setConfigurationRoot(initialSettings);
        
        setMissingDefaultValues();

        layoutLoaded = false;
    }
}

bool Configuration::replaceConfiguration(const String& newFileName)
{
    if (!(File::isAbsolutePath(newFileName)))
    {
        if (saveIfNeededAndUserAgrees(true) == savedOk)
        {
            setChangedFlag(false);
            layoutXml = loaderLayoutXml;
            configurationRoot = loaderConfigurationRoot;
            setFile(File());
            lastFailedFile = File();
            setChangedFlag(false);
            return true;
        }
    }
    else if (File(newFileName) != getFile())
    {
        File newFile(newFileName);

        lastFailedFile = newFile;
    
        if (newFile.existsAsFile())
        {
            if (saveIfNeededAndUserAgrees(true) == savedOk)
            {
                Result result = loadFrom(newFile, false);

                if (result.wasOk())
                {
                    lastFailedFile = File();
                    UserSettings::getInstance()->setLastTimeConfigurationLoaded(newFileName);
                    setMissingDefaultValues();
    
                    return true;
                }
                else
                {
                    lastError        = "Problem loading configuration file";
                    lastErrorDetails = newFileName + " - " + result.getErrorMessage();
                    return false;
                }
            }
        } 
        else
        {
            lastError = "Can't find configuration file";
            lastErrorDetails = newFileName;
            return false;
        }
    }

    return true;
}

void Configuration::generateUniqueParameterNames(ValueTree& parameter, UndoManager* undoManager)
{
    String parameterNameBase = parameter.getProperty(Ids::name);

    if (parameterNameBase.isEmpty())
        parameterNameBase = getDefaultParameter().getProperty(Ids::name);

    String parameterName;
    int    settingNum = 1;
    
    for (;;)
    {
        parameterName = parameterNameBase + String(settingNum);
        
        if (parameterNameExists(parameterName))
        {
            settingNum++;
            continue;
        }
        else
        {
            String newShortDescription = parameter.getProperty(Ids::shortDescription).toString();
            
            if (newShortDescription.isEmpty())
                newShortDescription = getDefaultParameter().getProperty(Ids::shortDescription);

            newShortDescription += " " + String(settingNum);

            String newFullDescription  = parameter.getProperty(Ids::fullDescription).toString();
            
            if (newFullDescription.isEmpty())
                newFullDescription = getDefaultParameter().getProperty(Ids::fullDescription);

            newFullDescription +=  " " + String(settingNum);

            parameter.setProperty(Ids::name,             parameterName,       undoManager);
            parameter.setProperty(Ids::shortDescription, newShortDescription, undoManager);
            parameter.setProperty(Ids::fullDescription,  newShortDescription, undoManager);

            break;
        }
    }
}

void Configuration::addNewParameter(ValueTree& newParameter, const ValueTree& paramValues, int targetIndex, ParameterTarget parameterTarget, UndoManager* um)
{
    if (paramValues.isValid())
        newParameter = paramValues.createCopy();
    else
        newParameter = getDefaultParameter().createCopy();

    generateUniqueParameterNames(newParameter, um);

    newParameter.setProperty(Ids::scopeSync,  -1, um);
    newParameter.setProperty(Ids::scopeLocal, -1, um);

    if (parameterTarget == host)
        getHostParameters().addChild(newParameter, targetIndex, um);
    else
        getScopeParameters().addChild(newParameter, targetIndex, um);
}

void Configuration::updateParameterFromPreset(ValueTree& parameter, const ValueTree& preset, bool overwriteNames, UndoManager* undoManager)
{
    String name       = parameter.getProperty(Ids::name);
    String shortDesc  = parameter.getProperty(Ids::shortDescription);
    String fullDesc   = parameter.getProperty(Ids::fullDescription);
    int    scopeSync  = parameter.getProperty(Ids::scopeSync);
    int    scopeLocal = parameter.getProperty(Ids::scopeLocal);

    parameter.copyPropertiesFrom(preset, undoManager);
    parameter.removeProperty(Ids::presetFileName, undoManager);
    parameter.removeProperty(Ids::presetFileLibrarySet, undoManager);
    parameter.removeProperty(Ids::presetFileAuthor, undoManager);
    parameter.removeProperty(Ids::presetFileBlurb, undoManager);
    parameter.removeProperty(Ids::filePath, undoManager);
    parameter.removeProperty(Ids::fileName, undoManager);

    parameter.removeAllChildren(undoManager);

    ValueTree settings = preset.getChildWithName(Ids::settings).createCopy();
        
    if (settings.isValid())
        parameter.addChild(settings, -1, undoManager);

    parameter.setProperty(Ids::scopeSync,        scopeSync,  undoManager);
    parameter.setProperty(Ids::scopeLocal,       scopeLocal, undoManager);

    if (!overwriteNames)
    {
        parameter.setProperty(Ids::name,             name,       undoManager);
        parameter.setProperty(Ids::shortDescription, shortDesc,  undoManager);
        parameter.setProperty(Ids::fullDescription,  fullDesc,   undoManager);
    }
    else
    {
        generateUniqueParameterNames(parameter, undoManager);
    }
}

void Configuration::deleteMapping(const Identifier& mappingType, 
                                  ValueTree& mappingToDelete,
                                  UndoManager* um)
{
    ValueTree mappingRoot = configurationRoot.getChildWithName(Ids::mapping).getChildWithName(getMappingParentId(mappingType));
    
    mappingRoot.removeChild(mappingRoot.indexOf(mappingToDelete), um);
}

void Configuration::addNewMapping(const Identifier& mappingType, 
                                  const String& componentName, 
                                  const String& parameterName, 
                                  ValueTree& newMapping,
                                  int targetIndex, 
                                  UndoManager* um)
{
    ValueTree mappingRoot = configurationRoot.getChildWithName(Ids::mapping).getChildWithName(getMappingParentId(mappingType));
    
    newMapping = ValueTree(mappingType);
    newMapping.setProperty(Ids::name, componentName, um);
    newMapping.setProperty(Ids::mapTo, parameterName, um);

    mappingRoot.addChild(newMapping, targetIndex, um);
}

ValueTree Configuration::getDefaultParameter()
{
    ValueTree defaultParameter(Ids::parameter);
    defaultParameter.setProperty(Ids::name,             "PARAM",       nullptr);
    defaultParameter.setProperty(Ids::shortDescription, "Param",       nullptr);
    defaultParameter.setProperty(Ids::fullDescription,  "Parameter",   nullptr);
    defaultParameter.setProperty(Ids::scopeSync,        -1,            nullptr);
    defaultParameter.setProperty(Ids::scopeLocal,       -1,            nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMin,    0,             nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMax,    2147483647,    nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMinFlt, 0,             nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMaxFlt, 1,             nullptr);
    defaultParameter.setProperty(Ids::scopeDBRef,       0,             nullptr);
    defaultParameter.setProperty(Ids::valueType,        0,             nullptr);
    defaultParameter.setProperty(Ids::uiResetValue,     0,             nullptr);
    defaultParameter.setProperty(Ids::uiSkewFactor,     1,             nullptr);
    defaultParameter.setProperty(Ids::skewUIOnly,       false,         nullptr);
    defaultParameter.setProperty(Ids::uiRangeMin,       0,             nullptr);
    defaultParameter.setProperty(Ids::uiRangeMax,       100,           nullptr);
    defaultParameter.setProperty(Ids::uiRangeInterval,  0.0001,        nullptr);
    defaultParameter.setProperty(Ids::uiSuffix,         String::empty, nullptr);
    
    return defaultParameter;
}

bool Configuration::parameterNameExists(const String& parameterName)
{
    if (getHostParameters().getChildWithProperty(Ids::name, parameterName).isValid())
        return true;

    if (getScopeParameters().getChildWithProperty(Ids::name, parameterName).isValid())
        return true;

    return false;
}

void Configuration::deleteStyleOverride(const Identifier& componentType, 
                                        ValueTree& styleOverrideToDelete,
                                        UndoManager* um)
{
    ValueTree styleOverrideRoot = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));
    
    styleOverrideRoot.removeChild(styleOverrideRoot.indexOf(styleOverrideToDelete), um);
}

void Configuration::deleteAllStyleOverrides(const Identifier& componentType,
                                            const String& widgetTemplateId,
                                            UndoManager* um)
{
    ValueTree styleOverrideRoot = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));
    
    for (int i = styleOverrideRoot.getNumChildren() - 1; i >= 0 ; i--)
    {
        if (styleOverrideRoot.getChild(i).getProperty(Ids::widgetTemplateId).toString().equalsIgnoreCase(widgetTemplateId))
            styleOverrideRoot.removeChild(styleOverrideRoot.getChild(i), um);
    }
}

void Configuration::addStyleOverride(const Identifier& componentType,
                                     const String&     componentName,
                                     const String&     widgetTemplateId,
                                     ValueTree&        newStyleOverride,
                                     int               targetIndex,
                                     UndoManager*      um)
{
    ValueTree styleOverrideRoot = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));
    
    if (!newStyleOverride.isValid())
    {
        newStyleOverride = ValueTree(componentType);
        newStyleOverride.setProperty(Ids::lookAndFeelId, String::empty, um);
    }

    newStyleOverride.setProperty(Ids::name,             componentName, um);
    newStyleOverride.setProperty(Ids::widgetTemplateId, widgetTemplateId, um);
    newStyleOverride.setProperty(Ids::useColourOverrides, true, um);
    styleOverrideRoot.addChild(newStyleOverride, targetIndex, um);
}

void Configuration::addStyleOverrideToAll(const Identifier& componentType,
                                          const String& widgetTemplateId,
                                          ValueTree&    newStyleOverride,
                                          UndoManager*  um)
{
    deleteAllStyleOverrides(componentType, widgetTemplateId, um);

    for (int i = 0; i < componentLookup.size(); i++)
    {
        if (   !componentLookup[i]->noStyleOverride
            && componentLookup[i]->type == componentType
            && componentLookup[i]->widgetTemplateId.equalsIgnoreCase(widgetTemplateId))
        {
            DBG("Configuration::addStyleOverrideToAll - name: " + componentLookup[i]->name + ", wtid: " + widgetTemplateId);
            ValueTree styleOverride(newStyleOverride.createCopy());
            addStyleOverride(componentType, componentLookup[i]->name, widgetTemplateId, styleOverride, -1, um);
        }
    }
}

Result Configuration::saveDocument (const File& /* file */)
{
    generateConfigurationUID();
    UserSettings::getInstance()->updateConfigurationLibraryEntry(getFile().getFullPathName(), getFile().getFileName(), configurationRoot);

    ScopedPointer<XmlElement> outputXml = configurationRoot.createXml();

    if (outputXml->writeToFile(getFile(), String::empty, "UTF-8", 120))
        return Result::ok();
    else
        return Result::fail("Failed to save configuration file");
}

static File lastDocumentOpened;

File Configuration::getLastDocumentOpened()                   { return lastDocumentOpened; }
void Configuration::setLastDocumentOpened (const File& file)  { lastDocumentOpened = file; }

FileBasedDocument::SaveResult Configuration::saveIfNeededAndUserAgrees(bool offerCancelOption)
{
    if (!(hasChangedSinceSaved()) || getFile() == File::nonexistent)
        return savedOk;

    int result;

    if (offerCancelOption)
    {
        result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                                   TRANS("Loading New Configuration..."),
                                                   TRANS("Do you want to save the changes to \"DCNM\"?")
                                                    .replace ("DCNM", getDocumentTitle()),
                                                   TRANS("Save"),
                                                   TRANS("Discard changes"),
                                                   TRANS("Cancel"));
    }
    else
    {
        bool buttonState = AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon,
                                                   TRANS("Loading New Configuration..."),
                                                   TRANS("Do you want to save the changes to \"DCNM\"?")
                                                    .replace ("DCNM", getDocumentTitle()),
                                                   TRANS("Save"),
                                                   TRANS("Discard changes"));
        if (buttonState)
            result = 1;
        else
            result = 2;
    }

    if (result == 1)  // save changes
        return save(true, true);

    if (result == 2)  // discard changes
        return savedOk;

    return userCancelledSave;
}

void Configuration::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
    if (property == Ids::scopeRangeMin)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::scopeRangeMinFlt, newValue, nullptr);
    }
    else if (property == Ids::scopeRangeMax)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::scopeRangeMaxFlt, newValue, nullptr);
    }
    else if (property == Ids::intValue)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::value, newValue, nullptr);
    }
    else if (property == Ids::uiRangeMin || property == Ids::uiRangeMax || property == Ids::uiSkewMidpoint)
    {
        float minimum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMin);
        float maximum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMax);
        float midpoint   = treeWhosePropertyHasChanged.getProperty(Ids::uiSkewMidpoint);
        float skewFactor = 1.0f;

        if (maximum > minimum && midpoint > minimum && midpoint < maximum)
            skewFactor = (float)(log(0.5) / log((midpoint - minimum) / (maximum - minimum)));

        treeWhosePropertyHasChanged.setProperty(Ids::uiSkewFactor, skewFactor, nullptr);
    }
    else if (property == Ids::name && treeWhosePropertyHasChanged.getParent().getParent().hasType(Ids::styleOverrides))
    {
        String     componentName = treeWhosePropertyHasChanged.getProperty(Ids::name);
        Identifier componentType = treeWhosePropertyHasChanged.getType();
        DBG("Configuration::valueTreePropertyChanged - Style Override's Component Name has changed to: " + componentName);

        for (int i = 0; i < componentLookup.size(); i++)
        {
            if (   componentLookup[i]->name == componentName
                && componentLookup[i]->type == componentType)
            {
                String widgetTemplateId = componentLookup[i]->widgetTemplateId;
                treeWhosePropertyHasChanged.setProperty(Ids::widgetTemplateId, widgetTemplateId, nullptr);
                DBG("Configuration::valueTreePropertyChanged - Updated widgetTemplateId to: " + widgetTemplateId);
            }
        }
    }

    changed();
}

void Configuration::valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */)     { changed(); }
void Configuration::valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved*/ ) { changed(); }
void Configuration::valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */)                 { changed(); }
void Configuration::valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */)                            { changed(); }

ValueTree Configuration::getHostParameters()
{
    return configurationRoot.getChildWithName(Ids::hostParameters);
}

ValueTree Configuration::getScopeParameters()
{
    return configurationRoot.getChildWithName(Ids::scopeParameters);
}

ValueTree Configuration::getMapping()
{
    if (configurationRoot.getChildWithName(Ids::mapping).isValid())
        return configurationRoot.getChildWithName(Ids::mapping);
    else
        return loaderConfigurationRoot.getChildWithName(Ids::mapping);
}

XmlElement& Configuration::getLayout(String& errorText, String& errorDetails, bool forceReload)
{
    if (layoutLoaded && !forceReload)
        return layoutXml;
    else
        return loadLayoutXml(errorText, errorDetails);
}

XmlElement& Configuration::loadLayoutXml(String& errorText, String& errorDetails)
{
    layoutDirectory = String::empty;
    layoutXml       = loaderLayoutXml;
    layoutLoaded    = false;
    
    String layoutName       = configurationRoot.getProperty(Ids::layoutName,     String::empty).toString();
    String layoutLibrarySet = configurationRoot.getProperty(Ids::layoutLibrarySet, String::empty).toString();

    String layoutFilename = UserSettings::getInstance()->getLayoutFilename(layoutName, layoutLibrarySet);

    if (layoutFilename.isEmpty())
    {
        if (configurationRoot.getProperty(Ids::name).toString() != "No configuration loaded...")
        {
            errorText    = "Layout not found, using default layout";
            errorDetails = "No layout filename found in library for layout: '" + layoutName + "', in library set: '" + layoutLibrarySet + "'. Check settings in Configuration.";
        }

        return layoutXml;
    }
    
    File layoutFile;

    if (File::isAbsolutePath(layoutFilename) && File(layoutFilename) != File::nonexistent)
    {
        layoutFile = File(layoutFilename);
        
        DBG("ScopeSync::loadLayoutFile - Trying to load: " + layoutFile.getFullPathName());

        XmlDocument               layoutDocument(layoutFile);
        ScopedPointer<XmlElement> loadedLayoutXml = layoutDocument.getDocumentElement();

        if (loadedLayoutXml != nullptr)
        {
            if (loadedLayoutXml->hasTagName(Ids::layout))
            {
                // No XSD validation header
                layoutXml    = *loadedLayoutXml;
                layoutLoaded = true;
            }
            else
            {
                // Look for a layout element at the 2nd level down instead
                XmlElement* subXml = loadedLayoutXml->getChildByName(Ids::layout);
            
                if (subXml != nullptr)
                {
                    layoutXml    = *subXml;
                    layoutLoaded = true;
                }
            }
        }
    
        if (!layoutLoaded)
        {
            errorText    = "Problem reading Configuration's Layout File";
            errorDetails = layoutDocument.getLastParseError();
            return layoutXml;
        }
    }
    else
    {
        errorText    = "Invalid layout filename, using default layout";
        errorDetails = layoutFilename + " is not a valid layout filename";
        return layoutXml;
    }

    layoutDirectory = layoutFile.getParentDirectory().getFullPathName();

    setupComponentLookup();
    return layoutXml;
}

StringArray Configuration::getComponentNames(const Identifier& componentType, bool checkStyleOverrideFlag)
{
    String e1, e2;

    if (!layoutLoaded)
        loadLayoutXml(e1, e2);

    StringArray componentNames;

    for (int i = 0; i < componentLookup.size(); i++)
    {
        ComponentLookupItem* cli = componentLookup[i];

        if ((checkStyleOverrideFlag && cli->noStyleOverride) || componentType != cli->type)
            continue;
        else
            componentNames.add(cli->name);
    }

    return componentNames;
}

void Configuration::setupComponentLookup()
{
    componentLookup.clear();
    
    XmlElement* topLevelComponent = layoutXml.getChildByName("component");

    if (topLevelComponent != nullptr)
        getComponentNamesFromXml(*topLevelComponent);
    
    ComponentLookupSorter sorter;
    componentLookup.sort(sorter);
}
    
void Configuration::getComponentNamesFromXml(XmlElement& xml)
{
    forEachXmlChildElement(xml, child)
    {
        String componentName    = child->getStringAttribute("name", String::empty);
        String componentType    = child->getTagName();
        bool   noStyleOverride  = child->getBoolAttribute("nostyleoverride", false);
        String widgetTemplateId = child->getStringAttribute("wtid", String::empty);
        
        if (componentName.isNotEmpty() 
            && 
            (  componentType.equalsIgnoreCase("slider")
            || componentType.equalsIgnoreCase("label")  
            || componentType.equalsIgnoreCase("textbutton")
            || componentType.equalsIgnoreCase("combobox")
            || componentType.equalsIgnoreCase("tabbedcomponent")
            || componentType.equalsIgnoreCase("component"))
            &&
            !componentInLookup(getComponentTypeId(componentType), componentName))
        {
            ComponentLookupItem* cli = new ComponentLookupItem(componentName, getComponentTypeId(componentType), noStyleOverride, widgetTemplateId);
            componentLookup.add(cli);

            if (componentType.equalsIgnoreCase("tabbedcomponent") || componentType.equalsIgnoreCase("component"))
                getComponentNamesFromXml(*child);
        }
        else
            getComponentNamesFromXml(*child);
    }
}

bool Configuration::componentInLookup(const Identifier& componentType, const String& componentName)
{
    for (int i = 0; i < componentLookup.size(); i++)
    {
        ComponentLookupItem* cli = componentLookup[i];
        
        if (cli->type == componentType && cli->name == componentName)
            return true;
    }

    return false;
}

void Configuration::setupParameterLists(StringArray& parameterDescriptions, Array<var>& parameterNames, bool discreteOnly)
{
    ValueTree hostParameters = getHostParameters();
    
    for (int i = 0; i < hostParameters.getNumChildren(); i++)
    {
        ValueTree parameter(hostParameters.getChild(i));
        
        if (!discreteOnly || int(parameter[Ids::valueType]) == 1)
        {
            parameterDescriptions.add(parameter[Ids::name].toString() + " (" + parameter[Ids::fullDescription].toString() + ")");
            parameterNames.add(hostParameters.getChild(i)[Ids::name]);
        }
    }
    
    ValueTree scopeParameters = getScopeParameters();
    
    for (int i = 0; i < scopeParameters.getNumChildren(); i++)
    {
        ValueTree parameter(scopeParameters.getChild(i));
        
        if (!discreteOnly || int(parameter[Ids::valueType]) == 1)
        {
            parameterDescriptions.add(parameter[Ids::name].toString() + " (" + parameter[Ids::fullDescription].toString() + ")");
            parameterNames.add(scopeParameters.getChild(i)[Ids::name]);
        }
    }
}

void Configuration::setupSettingLists(const String& parameterName, StringArray& settingNames, Array<var>& settingValues)
{
    ValueTree parameter(getHostParameters().getChildWithProperty(Ids::name, parameterName));

    if (!(parameter.isValid()))
        parameter = getScopeParameters().getChildWithProperty(Ids::name, parameterName);

    if (parameter.isValid())
    {
        ValueTree settings(parameter.getChildWithName(Ids::settings));
        if (settings.isValid())
        {
            for (int i = 0; i < settings.getNumChildren(); i++)
            {
                String settingName = settings.getChild(i).getProperty(Ids::name);
                settingNames.add(settingName);
                settingValues.add(settingName);
            }
        }
    }
}

Identifier Configuration::getMappingParentId(const Identifier& componentType)
{
         if (componentType == Ids::slider)          return Ids::sliders;
    else if (componentType == Ids::comboBox)        return Ids::comboBoxes;
    else if (componentType == Ids::tabbedComponent) return Ids::tabbedComponents;
    else if (componentType == Ids::textButton)      return Ids::textButtons;
    else if (componentType == Ids::label)           return Ids::labels;
    else                                            return Ids::components;
}

Identifier Configuration::getComponentTypeId(const String& typeString)
{
         if (typeString.equalsIgnoreCase("slider"))          return Ids::slider;
    else if (typeString.equalsIgnoreCase("label"))           return Ids::label;
    else if (typeString.equalsIgnoreCase("textbutton"))      return Ids::textButton;
    else if (typeString.equalsIgnoreCase("tabbedcomponent")) return Ids::tabbedComponent;
    else if (typeString.equalsIgnoreCase("combobox"))        return Ids::comboBox;
    else if (typeString.equalsIgnoreCase("component"))       return Ids::component;
    else                                                     return Identifier();
}

String Configuration::getComponentTypeName(const Identifier& type)
{
         if (type == Ids::slider)          return "Slider";
    else if (type == Ids::label)           return "Label";
    else if (type == Ids::comboBox)        return "Combo Box";
    else if (type == Ids::tabbedComponent) return "Tabbed Component";
    else if (type == Ids::textButton)      return "Text Button";
    else if (type == Ids::component)       return "Component";
    else                                   return String::empty;
}

ValueTree Configuration::getStyleOverride(const Identifier& componentType, const String& componentName)
{
    ValueTree componentStyleOverrides = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));

    // DBG("Configuration::getStyleOverride - componentType: " + String(componentType) + ", componentName: " + componentName + ", overrides valid: " + String(componentStyleOverrides.isValid()));
    return componentStyleOverrides.getChildWithProperty(Ids::name, componentName);
}

const String Configuration::emptyConfiguration =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"\n"
"<configuration>\n"
"  <hostParameters/>\n"
"  <scopeParameters/>\n"
"  <mapping>\n"
"    <textButtons/>\n"
"    <sliders/>\n"
"    <labels/>\n"
"    <comboBoxes/>\n"
"    <tabbedComponents/>\n"
"  </mapping>\n"
"</configuration>\n";

const String Configuration::emptyWithScopeLocalConfiguration =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"\n"
"<configuration>\n"
"  <hostParameters/>\n"
"  <scopeParameters>\n"
"    <parameter name=\"CPHost\" shortDescription=\"CPHost\" fullDescription=\"Control Panel - Host\" scopeS"
"ync=\"-1\" scopeLocal=\"12\" scopeRangeMin=\"0\" scopeRangeMax=\"2147483647\"\n"
"               scopeRangeMinFlt=\"0\" scopeRangeMaxFlt=\"1\" scopeDBRef=\"0\" valueType=\"1\" uiResetValue=\""
"0\" uiSkewFactor=\"1\" skewUIOnly=\"false\"\n"
"               uiRangeMin=\"0\" uiRangeMax=\"1\" uiRangeInterval=\"1\" uiSuffix=\"\">\n"
"      <settings>\n"
"        <setting name=\"DISCONNECTED\" value=\"0\" intValue=\"0\"/>\n"
"        <setting name=\"CONNECTED\" value=\"1\" intValue=\"2147483647\"/>\n"
"      </settings>\n"
"    </parameter>\n"
"    <parameter name=\"PatchWindow\" shortDescription=\"PatchWindow\" fullDescription=\"Open Patch Window\""
" scopeSync=\"-1\" scopeLocal=\"13\" scopeRangeMin=\"0\"\n"
"               scopeRangeMax=\"2147483647\" scopeRangeMinFlt=\"0\" scopeRangeMaxFlt=\"1\" scopeDBRef=\"0\" v"
"alueType=\"1\" uiResetValue=\"0\" uiSkewFactor=\"1\"\n"
"               skewUIOnly=\"false\" uiRangeMin=\"0\" uiRangeMax=\"1\" uiRangeInterval=\"1\" uiSuffix=\"\">\n"
"      <settings>\n"
"        <setting name=\"OFF\" value=\"0\" intValue=\"0\"/>\n"
"        <setting name=\"ON\" value=\"1\" intValue=\"2147483647\"/>\n"
"      </settings>\n"
"    </parameter>\n"
"    <parameter name=\"PresetList\" shortDescription=\"PresetList\" fullDescription=\"Open Preset List\" sc"
"opeSync=\"-1\" scopeLocal=\"14\" scopeRangeMin=\"0\"\n"
"               scopeRangeMax=\"2147483647\" scopeRangeMinFlt=\"0\" scopeRangeMaxFlt=\"1\" scopeDBRef=\"0\" v"
"alueType=\"1\" uiResetValue=\"0\" uiSkewFactor=\"1\"\n"
"               skewUIOnly=\"false\" uiRangeMin=\"0\" uiRangeMax=\"1\" uiRangeInterval=\"1\" uiSuffix=\"\">\n"
"      <settings>\n"
"        <setting name=\"OFF\" value=\"0\" intValue=\"0\"/>\n"
"        <setting name=\"ON\" value=\"1\" intValue=\"2147483647\"/>\n"
"      </settings>\n"
"    </parameter>\n"
"    <parameter name=\"LoadConfig\" shortDescription=\"LoadConfig\" fullDescription=\"Load Configuration\" "
"scopeSync=\"-1\" scopeLocal=\"15\" scopeRangeMin=\"0\"\n"
"               scopeRangeMax=\"2147483647\" scopeRangeMinFlt=\"0\" scopeRangeMaxFlt=\"1\" scopeDBRef=\"0\" v"
"alueType=\"1\" uiResetValue=\"0\" uiSkewFactor=\"1\"\n"
"               skewUIOnly=\"false\" uiRangeMin=\"0\" uiRangeMax=\"1\" uiRangeInterval=\"1\" uiSuffix=\"\">\n"
"      <settings>\n"
"        <setting name=\"OFF\" value=\"0\" intValue=\"0\"/>\n"
"        <setting name=\"ON\" value=\"1\" intValue=\"2147483647\"/>\n"
"      </settings>\n"
"    </parameter>\n"
"  </scopeParameters>\n"
"  <mapping>\n"
"    <textButtons>\n"
"      <textButton name=\"CPHost-Disconnected\" mapTo=\"CPHost\" settingDown=\"DISCONNECTED\" type=\"1\" radi"
"oGroup=\"\"/>\n"
"      <textButton name=\"CPHost-Connected\" mapTo=\"CPHost\" settingDown=\"CONNECTED\" type=\"1\" radioGroup"
"=\"\"/>\n"
"      <textButton name=\"PatchWindow\" mapTo=\"PatchWindow\" settingDown=\"ON\" settingUp=\"OFF\" type=\"1\" r"
"adioGroup=\"\"/>\n"
"      <textButton name=\"PresetList\" mapTo=\"PresetList\" settingDown=\"ON\" settingUp=\"OFF\" type=\"1\" rad"
"ioGroup=\"\"/>\n"
"      <textButton name=\"LoadConfig\" mapTo=\"LoadConfig\" settingDown=\"ON\" type=\"0\" radioGroup=\"\"/>\n"
"    </textButtons>\n"
"    <sliders/>\n"
"    <labels/>\n"
"    <comboBoxes/>\n"
"    <tabbedComponents/>\n"
"  </mapping>\n"
"</configuration>\n";

const String Configuration::loaderLayout =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<layout>\n"
"  <!--Main Component-->\n"
"  <component backgroundcolour=\"ff2d3035\" lfid=\"system:default\">\n"
"    <bounds x=\"0\" y=\"0\" width=\"674\" height=\"100\" />\n"
"    <!--Header Section-->\n"
"    <component>\n"
"      <bounds relativerectangle=\"right - parent.width, 1, parent.width, top + 40\"></bounds>\n"
"      <label lfid=\"system:configname\" name=\"configurationname\" text=\"Current Configuration\">\n"
"        <bounds x=\"177\" y=\"11\" width=\"160\" height=\"17\"></bounds>\n"
"        <font bold=\"true\" height=\"12\"></font>\n"
"      </label>\n"
"      <textbutton lfid=\"system:new_config_button\" name=\"newconfiguration\" tooltip=\"New Configuration"
"\">\n"
"        <bounds x=\"347\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:load_config_button\" name=\"chooseconfiguration\" tooltip=\"Load Configur"
"ation\">\n"
"        <bounds x=\"373\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:reload_button\" name=\"reloadconfiguration\" tooltip=\"Reload Current Con"
"figuration\">\n"
"        <bounds x=\"399\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:showconfigurationmanager_button\" name=\"showconfigurationmanager\" text"
"=\"\" tooltip=\"Open Configuration Manager panel\">\n"
"        <bounds x=\"425\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:about_box\" name=\"showaboutbox\" id=\"showaboutbox\" text=\"\" tooltip=\"Sho"
"w About Box\">\n"
"        <bounds relativerectangle=\"right - 32, 9, parent.width - 1, top + 21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:settings_button\" name=\"showusersettings\" id=\"showusersettings\" text=\""
"\" tooltip=\"Open User Settings panel\">\n"
"        <bounds relativerectangle=\"right - 32, showaboutbox.top, showaboutbox.left - 5, top + 21\"></"
"bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:snapshot_button\" name=\"snapshot\" text=\"\" tooltip=\"Send Snapshot of Cu"
"rrent Control Values\">\n"
"        <bounds relativerectangle=\"right - 32, showusersettings.top, showusersettings.left - 5, top "
"+ 21\"></bounds>\n"
"      </textbutton>\n"
"      <component backgroundcolour=\"00000000\" backgroundimage=\"scopeSyncLogo\">\n"
"        <bounds x=\"13\" y=\"11\" width=\"151\" height=\"16\"></bounds>\n"
"      </component>\n"
"      <!--Shows Patch and Preset Buttons for BC Modular shell based on CP<->Host connection state-->"
"\n"
"      <tabbedcomponent displaycontext=\"scope\" name=\"CP-Host Connection\" showdropshadow=\"false\">\n"
"        <bounds relativerectangle=\"right - 58, 6, parent.width - 70, top + 32\"></bounds>\n"
"        <tabbar orientation=\"right\" depth=\"0\"></tabbar>\n"
"        <tab idx=\"1\" name=\"DISCONNECTED\">\n"
"          <component>\n"
"            <!--No Controls-->\n"
"          </component>\n"
"        </tab>\n"
"        <tab idx=\"2\" name=\"CONNECTED\">\n"
"          <component>\n"
"            <textbutton lfid=\"system:patch_button\" name=\"PatchWindow\">\n"
"              <bounds x=\"0\" y=\"2\" width=\"27\" height=\"21\"></bounds>\n"
"            </textbutton>\n"
"            <textbutton lfid=\"system:presets_button\" name=\"PresetList\">\n"
"              <bounds x=\"33\" y=\"2\" width=\"19\" height=\"21\"></bounds>\n"
"            </textbutton>\n"
"          </component>\n"
"        </tab>\n"
"      </tabbedcomponent>\n"
"      <component displaycontext=\"scope\" backgroundcolour=\"00000000\">\n"
"        <bounds relativerectangle=\"right - 2, 0, parent.width - 132, top + 2\"></bounds>\n"
"        <!--CP<->Host Disconnected-->\n"
"        <textbutton name=\"CPHost-Disconnected\">\n"
"          <bounds x=\"3\" y=\"3\" width=\"1\" height=\"1\"></bounds>\n"
"          <choosetab tabbedcomponent=\"CP-Host Connection\" tabname=\"DISCONNECTED\"></choosetab>\n"
"        </textbutton>\n"
"        <!--CP<->Host Connected-->\n"
"        <textbutton name=\"CPHost-Connected\">\n"
"          <bounds x=\"3\" y=\"3\" width=\"1\" height=\"1\"></bounds>\n"
"          <choosetab tabbedcomponent=\"CP-Host Connection\" tabname=\"CONNECTED\"></choosetab>\n"
"        </textbutton>\n"
"      </component>\n"
"    </component>\n"
"    <!--Background-->\n"
"    <component backgroundcolour=\"55000000\" backgroundimage=\"Skins/B-Control/background.png\">\n"
"      <bounds relativerectangle=\"right - parent.width, 41, parent.width, parent.height\" />\n"
"    </component>\n"
"  </component>\n"
"</layout>";
