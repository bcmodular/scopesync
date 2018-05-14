/**
 * Class to handle the Configuration file and its ValueTree
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

#include "Configuration.h"
#include "../Core/Global.h"
#include "../Core/ScopeSync.h"
#include "../Utils/BCMMisc.h"
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
      scopeSync(ss),
      newFile(file),
      configuration(ss.getConfiguration())
{
    cancelled = true;

    setUsingNativeTitleBar (true);
    
    settings = ValueTree(Ids::configuration);

    setContentOwned(new NewConfigurationEditor(ss, settings, file.getFullPathName(), acm), true);
    
    restoreWindowPosition(posX, posY);

	Component::setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

NewConfigurationWindow::~NewConfigurationWindow() = default;

void NewConfigurationWindow::addConfiguration() const
{
    ValueTree newSettings = configuration.getEmptyConfiguration();
    newSettings.copyPropertiesFrom(settings, nullptr);
    
    scopeSync.addConfiguration(File(newFile), newSettings);
    scopeSync.hideAddConfigurationWindow();
}

void NewConfigurationWindow::cancel() const { scopeSync.hideAddConfigurationWindow(); }

void NewConfigurationWindow::closeButtonPressed() { addConfiguration(); }

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
    : panel(settings, ss.getUndoManager(), ss, acm, true),
      commandManager(acm),
      filePathLabel("File Path"),
      addButton("Add Configuration"),
      cancelButton("Cancel")
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

NewConfigurationEditor::~NewConfigurationEditor() = default;

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

void NewConfigurationEditor::addConfiguration() const
{
    NewConfigurationWindow* parent = dynamic_cast<NewConfigurationWindow*>(getParentComponent());
    parent->addConfiguration();
}

void NewConfigurationEditor::cancel() const
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
	switchToLoader();
    configurationRoot.addListener(this);
}

Configuration::~Configuration()
{
    configurationRoot.removeListener(this);
};

void Configuration::switchToLoader()
{
	loadLoaderConfiguration();
	setConfigurationRoot(loaderConfigurationRoot);
	loadLoaderLayout();
	layoutXml = loaderLayoutXml;
	layoutLoaded = true;
	setChangedFlag(false);
}

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

    configurationRoot.getOrCreateChildWithName(Ids::parameters, nullptr);

	configurationRoot.moveChild(configurationRoot.indexOf(configurationRoot.getChildWithName(Ids::parameters)), 0, nullptr);
    
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
	String xmlToParse(BinaryData::empty_configuration);
    ScopedPointer<XmlElement> xml(XmlDocument::parse(xmlToParse));

    ValueTree newTree (ValueTree::fromXml(*xml));
    newTree.setProperty(Ids::name, "No configuration loaded...", nullptr);
    newTree.setProperty(Ids::readOnly, true, nullptr);
    newTree.setProperty(Ids::excludeFromChooser, true, nullptr);
    newTree.setProperty(Ids::UID, -1, nullptr);

    loaderConfigurationRoot = newTree;
}

ValueTree Configuration::getEmptyConfiguration() const
{
    String xmlToParse(BinaryData::empty_configuration);
	ScopedPointer<XmlElement> xml(XmlDocument::parse(xmlToParse));

    return ValueTree::fromXml(*xml);
}

void Configuration::loadLoaderLayout()
{
    ScopedPointer<XmlElement> configElement;

	String xmlToParse(BinaryData::loader_layout);
    XmlDocument configurationDocument(xmlToParse);
    configElement   = configurationDocument.getDocumentElement();
    loaderLayoutXml = *configElement;
}

String Configuration::getDocumentTitle()
{
    return configurationRoot.getProperty(Ids::name, String());
}

void Configuration::setupConfigurationProperties()
{
    const String filename ("Configuration_" + configurationRoot[Ids::ID].toString());

    PropertiesFile::Options options;
    options.applicationName     = filename;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    properties = new PropertiesFile(options);
}

PropertiesFile& Configuration::getConfigurationProperties() const
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

String Configuration::getConfigurationDirectory() const
{
    if (getFile() != File::nonexistent)
        return getFile().getParentDirectory().getFullPathName();
    else
        return String();
}

int Configuration::getConfigurationUID()
{
	if (configurationRoot.isValid())
	{
		if (configurationRoot.hasProperty(Ids::UID))
			return configurationRoot.getProperty(Ids::UID);
		
		return generateConfigurationUID();
	}

	return 0;
}

int Configuration::generateConfigurationUID()
{
    int uid;

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

bool Configuration::replaceConfiguration(StringRef newFileName)
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
                    userSettings->setLastTimeConfigurationLoaded(newFileName);
                    setMissingDefaultValues();
					migrateFromV102();
    
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

void Configuration::migrateFromV102()
{
	ValueTree parameters = getParameters();

	if (parameters.isValid())
	{
		// Firstly migrate from old scopeCode property to new scopeParamGroup and scopeParamId in v103
		for (int i = 0; i < parameters.getNumChildren(); i++)
		{
			ValueTree parameter(parameters.getChild(i));
        
			int scopeParamGroup = -1;
			int scopeParamId    = -1;

			if (!parameter[Ids::scopeCode].isVoid() && parameter[Ids::scopeCode].toString().isNotEmpty())
			{
				String scopeCode(parameter[Ids::scopeCode].toString());

				if (scopeCode.isNotEmpty())
				{
					const auto scopeOSCParamID(scopeCodeMapper->getScopeOSCParamIDByCode(parameter[Ids::scopeCode].toString())) ;
					scopeParamGroup = scopeOSCParamID.paramGroup;
					scopeParamId    = scopeOSCParamID.paramId;
				}

				DBG("Configuration::migrateFromV102 - migrating scopeCode: " + scopeCode + " to: " + String(scopeParamGroup) + ":" + String(scopeParamId));
				parameter.setProperty(Ids::scopeParamGroup, scopeParamGroup, nullptr);
				parameter.setProperty(Ids::scopeParamId,    scopeParamId, nullptr);
                
                parameter.removeProperty(Ids::scopeCode, nullptr);
			}
		}
	}
}

void Configuration::generateUniqueParameterNames(ValueTree& parameter, UndoManager* undoManager) const
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

void Configuration::addNewParameter(ValueTree& newParameter, const ValueTree& paramValues, int targetIndex, UndoManager* um) const
{
    if (paramValues.isValid())
        newParameter = paramValues.createCopy();
    else
        newParameter = getDefaultParameter().createCopy();

    generateUniqueParameterNames(newParameter, um);

	newParameter.setProperty(Ids::scopeParamGroup, -1, um);
	newParameter.setProperty(Ids::scopeParamId, -1, um);
    
    getParameters().addChild(newParameter, targetIndex, um);
}

void Configuration::updateParameterFromPreset(ValueTree& parameter, const ValueTree& preset, bool overwriteNames, UndoManager* undoManager) const
{
    String name            = parameter.getProperty(Ids::name);
    String shortDesc       = parameter.getProperty(Ids::shortDescription);
    String fullDesc        = parameter.getProperty(Ids::fullDescription);
    String scopeParamGroup = parameter.getProperty(Ids::scopeParamGroup);
	String scopeParamId    = parameter.getProperty(Ids::scopeParamId);

	parameter.copyPropertiesFrom(preset, undoManager);
    parameter.removeProperty(Ids::presetFileName, undoManager);
    parameter.removeProperty(Ids::presetFileLibrarySet, undoManager);
    parameter.removeProperty(Ids::presetFileAuthor, undoManager);
    parameter.removeProperty(Ids::presetFileBlurb, undoManager);
    parameter.removeProperty(Ids::blurb, undoManager);
    parameter.removeProperty(Ids::filePath, undoManager);
    parameter.removeProperty(Ids::fileName, undoManager);

    parameter.removeAllChildren(undoManager);

    ValueTree settings = preset.getChildWithName(Ids::settings).createCopy();
        
    if (settings.isValid())
        parameter.addChild(settings, -1, undoManager);

    parameter.setProperty(Ids::scopeParamGroup, scopeParamGroup, undoManager);
	parameter.setProperty(Ids::scopeParamId, scopeParamId, undoManager);
    
    if (!overwriteNames)
    {
        parameter.setProperty(Ids::name,             name,      undoManager);
        parameter.setProperty(Ids::shortDescription, shortDesc, undoManager);
        parameter.setProperty(Ids::fullDescription,  fullDesc,  undoManager);
    }
    else
    {
        generateUniqueParameterNames(parameter, undoManager);
    }
}

void Configuration::deleteMapping(const Identifier& mappingType, 
                                  ValueTree& mappingToDelete,
                                  UndoManager* um) const
{
    ValueTree mappingRoot = configurationRoot.getChildWithName(Ids::mapping).getChildWithName(getMappingParentId(mappingType));
    
    mappingRoot.removeChild(mappingRoot.indexOf(mappingToDelete), um);
}

void Configuration::addNewMapping(const Identifier& mappingType, 
                                  const String& componentName, 
                                  const String& parameterName, 
                                  ValueTree& newMapping,
                                  int targetIndex, 
                                  UndoManager* um) const
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
    defaultParameter.setProperty(Ids::scopeParamGroup,  -1,            nullptr);
    defaultParameter.setProperty(Ids::scopeParamId,     -1,             nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMin,    0,             nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMax,    2147483647,    nullptr);
    defaultParameter.setProperty(Ids::scopeDBRef,       0,             nullptr);
    defaultParameter.setProperty(Ids::valueType,        0,             nullptr);
    defaultParameter.setProperty(Ids::uiResetValue,     0,             nullptr);
    defaultParameter.setProperty(Ids::uiSkewFactor,     1,             nullptr);
    defaultParameter.setProperty(Ids::skewUIOnly,       false,         nullptr);
    defaultParameter.setProperty(Ids::uiRangeMin,       0,             nullptr);
    defaultParameter.setProperty(Ids::uiRangeMax,       100,           nullptr);
    defaultParameter.setProperty(Ids::uiRangeInterval,  0.0001,        nullptr);
    defaultParameter.setProperty(Ids::uiSuffix,         String(), nullptr);
    
    return defaultParameter;
}

ValueTree Configuration::getDefaultFixedParameter()
{
    ValueTree defaultParameter(Ids::parameter);
    defaultParameter.setProperty(Ids::name,            "PARAM",        nullptr);
    defaultParameter.setProperty(Ids::shortDescription, "Param",       nullptr);
    defaultParameter.setProperty(Ids::fullDescription,  "Parameter",   nullptr);
    defaultParameter.setProperty(Ids::scopeParamGroup,  -1,            nullptr);
    defaultParameter.setProperty(Ids::scopeParamId,     -1,            nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMin,    -2147483647,   nullptr);
    defaultParameter.setProperty(Ids::scopeRangeMax,    2147483647,    nullptr);
    defaultParameter.setProperty(Ids::scopeDBRef,       0,             nullptr);
    defaultParameter.setProperty(Ids::valueType,        0,             nullptr);
    defaultParameter.setProperty(Ids::uiResetValue,     0,             nullptr);
    defaultParameter.setProperty(Ids::uiSkewFactor,     1,             nullptr);
    defaultParameter.setProperty(Ids::skewUIOnly,       false,         nullptr);
    defaultParameter.setProperty(Ids::uiRangeMin,       -2147483647,   nullptr);
    defaultParameter.setProperty(Ids::uiRangeMax,       2147483647,    nullptr);
    defaultParameter.setProperty(Ids::uiRangeInterval,  1,             nullptr);
    defaultParameter.setProperty(Ids::uiSuffix,         String(), nullptr);

    return defaultParameter;
}

bool Configuration::parameterNameExists(const String& parameterName) const
{
    if (getParameters().getChildWithProperty(Ids::name, parameterName).isValid())
        return true;

    return false;
}

void Configuration::deleteStyleOverride(const Identifier& componentType, 
                                        ValueTree& styleOverrideToDelete,
                                        UndoManager* um) const
{
    ValueTree styleOverrideRoot = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));
    
    styleOverrideRoot.removeChild(styleOverrideRoot.indexOf(styleOverrideToDelete), um);
}

void Configuration::deleteAllStyleOverrides(const Identifier& componentType,
                                            const String& widgetTemplateId,
                                            UndoManager* um) const
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
                                     UndoManager*      um) const
{
    ValueTree styleOverrideRoot = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));
    
    if (!newStyleOverride.isValid())
    {
        newStyleOverride = ValueTree(componentType);
        newStyleOverride.setProperty(Ids::lookAndFeelId, String(), um);
    }

    newStyleOverride.setProperty(Ids::name,             componentName, um);
    newStyleOverride.setProperty(Ids::widgetTemplateId, widgetTemplateId, um);
    newStyleOverride.setProperty(Ids::useColourOverrides, true, um);
    styleOverrideRoot.addChild(newStyleOverride, targetIndex, um);
}

void Configuration::addStyleOverrideToAll(const Identifier& componentType,
                                          const String& widgetTemplateId,
                                          ValueTree&    newStyleOverride,
                                          UndoManager*  um) const
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
    userSettings->updateConfigurationLibraryEntry(getFile().getFullPathName(), getFile().getFileName(), configurationRoot);

    ScopedPointer<XmlElement> outputXml = configurationRoot.createXml();

    if (outputXml->writeToFile(getFile(), String(), "UTF-8", 120))
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
    if (property == Ids::uiRangeMin || property == Ids::uiRangeMax || property == Ids::uiSkewMidpoint)
    {
        float minimum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMin);
        float maximum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMax);
        float midpoint   = treeWhosePropertyHasChanged.getProperty(Ids::uiSkewMidpoint);
        float skewFactor = 1.0f;

        if (maximum > minimum && midpoint > minimum && midpoint < maximum)
            skewFactor = static_cast<float>(log(0.5) / log((midpoint - minimum) / (maximum - minimum)));

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

void Configuration::valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) { changed(); }
void Configuration::valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved*/ , int /* oldIndex */) { changed(); }
void Configuration::valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */, int /* oldIndex */, int /* newIndex */) { changed(); }
void Configuration::valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) { changed(); }

ValueTree Configuration::getParameters() const
{
    return configurationRoot.getChildWithName(Ids::parameters);
}

ValueTree Configuration::getMapping() const
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
    layoutDirectory = String();
    layoutXml       = loaderLayoutXml;
    layoutLoaded    = false;
    
    String layoutName       = configurationRoot.getProperty(Ids::layoutName,     String()).toString();
    String layoutLibrarySet = configurationRoot.getProperty(Ids::layoutLibrarySet, String()).toString();

    String layoutFilename = userSettings->getLayoutFilename(layoutName, layoutLibrarySet);

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
        String componentName    = child->getStringAttribute("name", String());
        String componentType    = child->getTagName();
        bool   noStyleOverride  = child->getBoolAttribute("nostyleoverride", false);
        String widgetTemplateId = child->getStringAttribute("wtid", String());
        
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

bool Configuration::componentInLookup(const Identifier& componentType, const String& componentName) const
{
    for (int i = 0; i < componentLookup.size(); i++)
    {
        ComponentLookupItem* cli = componentLookup[i];
        
        if (cli->type == componentType && cli->name == componentName)
            return true;
    }

    return false;
}

void Configuration::setupParameterLists(StringArray& parameterDescriptions, Array<var>& parameterNames, bool discreteOnly) const
{
    ValueTree parameters = getParameters();
    
    for (int i = 0; i < parameters.getNumChildren(); i++)
    {
        ValueTree parameter(parameters.getChild(i));
        
        if (!discreteOnly || int(parameter[Ids::valueType]) == 1)
        {
            parameterDescriptions.add(parameter[Ids::name].toString() + " (" + parameter[Ids::fullDescription].toString() + ")");
            parameterNames.add(parameters.getChild(i)[Ids::name]);
        }
    }
}

void Configuration::setupSettingLists(const String& parameterName, StringArray& settingNames, Array<var>& settingValues) const
{
    ValueTree parameter(getParameters().getChildWithProperty(Ids::name, parameterName));

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
    else                                   return String();
}

ValueTree Configuration::getStyleOverride(const Identifier& componentType, const String& componentName) const
{
    ValueTree componentStyleOverrides = configurationRoot.getChildWithName(Ids::styleOverrides).getChildWithName(getMappingParentId(componentType));

    // DBG("Configuration::getStyleOverride - componentType: " + String(componentType) + ", componentName: " + componentName + ", overrides valid: " + String(componentStyleOverrides.isValid()));
    return componentStyleOverrides.getChildWithProperty(Ids::name, componentName);
}

