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
#include "../Utils/BCMMisc.h"
#include "../Utils/BCMMath.h"

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
    loadLoaderLayout();
    layoutLoaded = false;
    setChangedFlag(false);
    configurationRoot = loaderConfigurationRoot;
    configurationRoot.addListener(this);
}

Configuration::~Configuration()
{
    configurationRoot.removeListener(this);
};

void Configuration::setMissingDefaultValues()
{
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
}

const char* Configuration::configurationFileExtension = ".configuration";

void Configuration::loadLoaderConfiguration()
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(loaderConfiguration));

    ValueTree newTree (ValueTree::fromXml(*xml));
    loaderConfigurationRoot = newTree;
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

PropertiesFile& Configuration::getConfigurationProperties ()
{
    if (properties == nullptr)
        setupConfigurationProperties();

    return *properties;
}

String Configuration::getDirectory()
{
    if (getFile() != File::nonexistent)
        return getFile().getParentDirectory().getFullPathName();
    else
        return String::empty;
}

Result Configuration::loadDocument(const File& file)
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(file));

    if (xml == nullptr || !(xml->hasTagName(Ids::configuration)))
        return Result::fail("Not a valid ScopeSync Configuration");

    ValueTree newTree (ValueTree::fromXml(*xml));

    if (!newTree.hasType(Ids::configuration))
        return Result::fail("The document contains errors and couldn't be parsed");

    configurationRoot = newTree;
    
    layoutLoaded = false;

    setChangedFlag(false);
    
    setMissingDefaultValues();

    DBG("Needs Saving: " + String(hasChangedSinceSaved()));
    return Result::ok();
}

bool Configuration::replaceConfiguration(const String& newFileName)
{
    bool offerCancel = false;

    if (ScopeSyncApplication::inPluginContext())
        offerCancel = true;

    if (!(File::isAbsolutePath(newFileName)))
    {
        if (saveIfNeededAndUserAgrees(offerCancel) == savedOk)
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
            if (saveIfNeededAndUserAgrees(offerCancel) == savedOk)
            {
                Result result = loadFrom(newFile, false);

                if (result.wasOk())
                {
                    lastFailedFile = File();
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

void Configuration::addNewParameter(const ValueTree& paramValues, int targetIndex, ParameterTarget parameterTarget, UndoManager* um)
{
    ValueTree newParameter;

    if (paramValues.isValid())
        newParameter = paramValues.createCopy();
    else
        newParameter = getDefaultParameter().createCopy();

    String parameterNameBase = newParameter.getProperty(Ids::name);
    if (parameterNameBase.isEmpty())
        parameterNameBase = getDefaultParameter().getProperty(Ids::name);

    String newParameterName;
    int settingNum = 1;
    
    for (;;)
    {
        newParameterName = parameterNameBase + String(settingNum);
        
        if (parameterNameExists(newParameterName))
        {
            settingNum++;
            continue;
        }
        else
        {
            String newShortDescription = newParameter.getProperty(Ids::shortDescription).toString();
            
            if (newShortDescription.isEmpty())
                newShortDescription = getDefaultParameter().getProperty(Ids::shortDescription);

            newShortDescription += " " + String(settingNum);

            String newFullDescription  = newParameter.getProperty(Ids::fullDescription).toString();
            
            if (newFullDescription.isEmpty())
                newFullDescription = getDefaultParameter().getProperty(Ids::fullDescription);

            newFullDescription +=  " " + String(settingNum);

            newParameter.setProperty(Ids::name,             newParameterName, um);
            newParameter.setProperty(Ids::shortDescription, newShortDescription, um);
            newParameter.setProperty(Ids::fullDescription,  newShortDescription, um);

            newParameter.setProperty(Ids::scopeSync,  -1, um);
            newParameter.setProperty(Ids::scopeLocal, -1, um);

            break;
        }
    }

    if (parameterTarget == host)
    {
        getHostParameters().addChild(newParameter, targetIndex, um);
    }
    else
    {
        getScopeParameters().addChild(newParameter, targetIndex, um);
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

Result Configuration::saveDocument (const File& /* file */)
{
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
    String layoutFilename = configurationRoot.getProperty(Ids::layoutFilename, String::empty).toString();

    if (layoutFilename.isEmpty())
    {
        DBG("ScopeSync::loadLayoutFile - No layout filename supplied, returning Loader");
        layoutXml    = loaderLayoutXml;
        layoutLoaded = false;
        return layoutXml;
    }
     
    File layoutFile = getFile().getSiblingFile(layoutFilename);
        
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
        DBG("Problem reading Configuration's Layout File: " + layoutDocument.getLastParseError());
        
        layoutXml = loaderLayoutXml;
    }

    setupComponentNameArrays();

    return layoutXml;
}

StringArray& Configuration::getComponentNames(const Identifier& componentType)
{
    String e1, e2;

    if (!layoutLoaded)
        loadLayoutXml(e1, e2);

         if (componentType == Ids::slider)
        return sliderNames;
    else if (componentType == Ids::label)
        return labelNames;
    else if (componentType == Ids::comboBox)
        return comboBoxNames;
    else if (componentType == Ids::textButton)
        return textButtonNames;
    else
        return tabbedComponentNames;
}

void Configuration::setupComponentNameArrays()
{
    sliderNames.clear();
    labelNames.clear();
    textButtonNames.clear();
    tabbedComponentNames.clear();
    comboBoxNames.clear();

    getComponentNames(layoutXml);
    
    tidyUpComponentArray(sliderNames);
    tidyUpComponentArray(labelNames);
    tidyUpComponentArray(textButtonNames);
    tidyUpComponentArray(tabbedComponentNames);
    tidyUpComponentArray(comboBoxNames);
}

void Configuration::tidyUpComponentArray(StringArray& arrayToTidy)
{
    arrayToTidy.removeEmptyStrings();
    arrayToTidy.removeDuplicates(true);
    arrayToTidy.sortNatural();
}
    
void Configuration::getComponentNames(XmlElement& xml)
{
    forEachXmlChildElement(xml, child)
    {
             if (child->hasTagName("slider"))
            sliderNames.add(child->getStringAttribute(Ids::name, String::empty));
        else if (child->hasTagName("label"))
            labelNames.add(child->getStringAttribute(Ids::name, String::empty));
        else if (child->hasTagName("textbutton"))
            textButtonNames.add(child->getStringAttribute(Ids::name, String::empty));
        else if (child->hasTagName("tabbedcomponent"))
        {
            tabbedComponentNames.add(child->getStringAttribute(Ids::name, String::empty));
            getComponentNames(*child);
        }
        else if (child->hasTagName("combobox"))
            comboBoxNames.add(child->getStringAttribute(Ids::name, String::empty));
        else
            getComponentNames(*child);
    }
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
    else                                            return Ids::labels;
}

const String Configuration::loaderConfiguration =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"\n"
"<configuration name=\"No configuration loaded...\" ID=\"NqZmNe\">\n"
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
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"host\" name=\"chooseconfiguration\" "
"tooltip=\"Load Configuration\">\n"
"        <bounds x=\"347\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"scope\" name=\"LoadConfig\" tooltip="
"\"Load Configuration\">\n"
"        <bounds x=\"347\" y=\"9\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:settings_button\" name=\"showusersettings\" id=\"showusersettings\" text=\""
"\" tooltip=\"Open User Settings panel\">\n"
"        <bounds relativerectangle=\"right - 32, 9, parent.width - 1, top + 21\"></bounds>\n"
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
"      <label lfid=\"system:systemerror\" name=\"SystemError\" text=\"\">\n"
"        <justification horizontallycentred=\"true\" />\n"
"        <bounds relativerectangle=\"125, parent.height-35, parent.width-143, parent.height-15\"></boun"
"ds>\n"
"      </label>\n"
"      <textbutton lfid=\"system:systemerror\" name=\"systemerrormoreinfo\" id=\"systemerrormoreinfo\" text"
"=\"\" tooltip=\"Click for more information\">\n"
"        <bounds relativerectangle=\"right - 18, parent.height-35, parent.width-125, parent.height-15\""
"></bounds>\n"
"      </textbutton>\n"
"    </component>\n"
"  </component>\n"
"</layout>";
