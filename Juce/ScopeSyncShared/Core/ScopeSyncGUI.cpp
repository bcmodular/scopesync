/**
 * GUI controller for the ScopeSync system. Hosted by different parent
 * objects depending on the AppContext (PluginGUI or ScopeFXGUI
 * respectively).
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

#include "ScopeSyncGUI.h"
#include "../Utils/BCMMath.h"
#include "../Components/UserSettings.h"

const int ScopeSyncGUI::timerFrequency = 100;

const Identifier ScopeSyncGUI::mappingId              = "mapping";
const Identifier ScopeSyncGUI::mappingParamId         = "parameter";
const Identifier ScopeSyncGUI::mappingParamNameId     = "name";
const Identifier ScopeSyncGUI::mappingMapToId         = "mapto";
const Identifier ScopeSyncGUI::mappingTypeId          = "type";
const Identifier ScopeSyncGUI::mappingDisplayTypeId   = "displaytype";
const Identifier ScopeSyncGUI::mappingCustomDisplayId = "customdisplay";
const Identifier ScopeSyncGUI::mappingSettingDownId   = "settingdown";
const Identifier ScopeSyncGUI::mappingSettingUpId     = "settingup";
const Identifier ScopeSyncGUI::mappingComponentNameId = "name";
const Identifier ScopeSyncGUI::mappingSliderId        = "slider";
const Identifier ScopeSyncGUI::mappingLabelId         = "label";
const Identifier ScopeSyncGUI::mappingComboBoxId      = "combobox";
const Identifier ScopeSyncGUI::mappingTextButtonId    = "textbutton";
const Identifier ScopeSyncGUI::mappingRadioGroupId    = "radiogroup";

ScopeSyncGUI::ScopeSyncGUI(ScopeSync& owner) : scopeSync(owner)
{
    createGUI();
    scopeSync.setGUIReload(false);
    startTimer(timerFrequency);
}

ScopeSyncGUI::~ScopeSyncGUI()
{
    stopTimer();
}

void ScopeSyncGUI::showUserSettings()
{
    DialogWindow::LaunchOptions launchOptions;
    launchOptions.content.setOwned(new UserSettings(scopeSync));
    launchOptions.componentToCentreAround      = mainComponent;
    launchOptions.dialogTitle                  = "User Settings";
    launchOptions.dialogBackgroundColour       = Colours::darkgrey;
    launchOptions.escapeKeyTriggersCloseButton = true;
    launchOptions.useNativeTitleBar            = false;
    launchOptions.resizable                    = false;

    launchOptions.launchAsync();
}

void ScopeSyncGUI::chooseConfiguration()
{
    File configurationFile(scopeSync.getConfigurationFilePath().getValue());
    File configurationFileDirectory = configurationFile.getParentDirectory();

    FileChooser fileChooser ("Please select the configuration file you want to load...",
                             configurationFileDirectory,
                             "*.configuration");

    if (fileChooser.browseForFileToOpen())
    {
        File configurationFile (fileChooser.getResult());
        scopeSync.setConfigurationFilePath(configurationFile.getFullPathName(), false);
    }
}

void ScopeSyncGUI::getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping, int& paramIdx)
{
    ValueTree componentMapping = deviceMapping.getChildWithName(componentTypeId);

    if (componentMapping.isValid())
        mapping = componentMapping.getChildWithProperty(mappingComponentNameId, componentName);
    else
        return;

    if (mapping.isValid())
    {
        String mapTo = mapping.getProperty(mappingMapToId).toString();
        paramIdx = scopeSync.getParameterIdxByName(mapTo);
    }
}

void ScopeSyncGUI::addTabbedComponent(BCMTabbedComponent* tabbedComponent)
{
    DBG("ScopeSyncGUI::addTabbedComponent - " + tabbedComponent->getName());
    tabbedComponents.add(tabbedComponent);
}
    
void ScopeSyncGUI::getTabbedComponentsByName(const String& name, Array<BCMTabbedComponent*>& tabbedComponentArray)
{
    tabbedComponentArray.clear();

    int numTabbedComponents = tabbedComponents.size();

    for (int i = 0; i < numTabbedComponents; i++)
    {
        String componentName = tabbedComponents[i]->getName();

        if (name.equalsIgnoreCase(componentName))
            tabbedComponentArray.add(tabbedComponents[i]);
    }
}

void ScopeSyncGUI::createGUI()
{
    mainComponent = nullptr;
    tabbedComponents.clearQuick();

    loadMapping();

    XmlElement* layoutXml = scopeSync.getConfiguration().getChildByName("layout");
    
    scopeSync.clearBCMLookAndFeels();

    // Firstly add the system LookAndFeels
    bool useImageCache = scopeSync.getAppProperties().getBoolValue("useimagecache", true);
    ScopedPointer<XmlElement> systemLookAndFeels = scopeSync.getSystemLookAndFeels();
    
    setupLookAndFeels(*systemLookAndFeels, useImageCache);

    // Then override with any layout-specific ones
    XmlElement* child = layoutXml->getChildByName("lookandfeels");

    if (child)
        setupLookAndFeels(*child, useImageCache);

    child = layoutXml->getChildByName("defaults");

    if (child)
        setupDefaults(*child);
    else
    {
        XmlElement defaults("defaults");
        setupDefaults(defaults);
    }

    child = layoutXml->getChildByName("component");

    if (child)
        createComponent(*child);
    else
    {
        XmlElement component("component");
        createComponent(component);
    }

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
}

void ScopeSyncGUI::loadMapping()
{
    XmlElement* mappingXml = scopeSync.getConfiguration().getChildByName("mapping");
    
    deviceMapping = ValueTree(mappingId);

    ValueTree sliderMapping(mappingSliderId);
    ValueTree labelMapping(mappingLabelId);
    ValueTree comboBoxMapping(mappingComboBoxId);
    ValueTree textButtonMapping(mappingTextButtonId);

    forEachXmlChildElement(*mappingXml, child)
    {
        String name = child->getStringAttribute(mappingComponentNameId, String::empty);

        if (name.isEmpty())
            continue;

        XmlElement* parameter = child->getChildByName(mappingParamId);
        String parameterName;

        if (parameter)
            parameterName = parameter->getStringAttribute(mappingParamNameId, String::empty);
        else
            continue;

        if (parameterName.isEmpty())
            continue;

        Identifier xmlId = child->getTagName();
        
        ValueTree mappingComponent(xmlId);
        mappingComponent.setProperty(mappingComponentNameId, name, nullptr);
        mappingComponent.setProperty(mappingMapToId, parameterName, nullptr);

        String type = parameter->getStringAttribute(mappingTypeId, "standard");
        mappingComponent.setProperty(mappingTypeId, type, nullptr);

        if (xmlId == mappingSliderId)
            sliderMapping.addChild(mappingComponent, -1, nullptr);
        else if (xmlId == mappingLabelId)
            labelMapping.addChild(mappingComponent, -1, nullptr);
        else if (xmlId == mappingComboBoxId)
            comboBoxMapping.addChild(mappingComponent, -1, nullptr);    
        else if (xmlId == mappingTextButtonId)
        {
            String displayType = parameter->getStringAttribute(mappingDisplayTypeId, "standard");
            mappingComponent.setProperty(mappingDisplayTypeId, displayType, nullptr);

            String customDisplay = parameter->getStringAttribute(mappingCustomDisplayId, "custom");
            mappingComponent.setProperty(mappingCustomDisplayId, customDisplay, nullptr);

            String settingDown = parameter->getStringAttribute(mappingSettingDownId, "__NO_SETTING__");
            mappingComponent.setProperty(mappingSettingDownId, settingDown, nullptr);

            String settingUp = parameter->getStringAttribute(mappingSettingUpId, "__NO_SETTING__");
            mappingComponent.setProperty(mappingSettingUpId, settingUp, nullptr);

            String radioGroupString = parameter->getStringAttribute(mappingRadioGroupId, "__NO_SETTING__").toLowerCase();
            if (radioGroupString.equalsIgnoreCase("__NO_SETTING__")) mappingComponent.setProperty(mappingRadioGroupId, radioGroupString.hashCode(), nullptr);            
            
            textButtonMapping.addChild(mappingComponent, -1, nullptr);
        }

        if (xmlId != mappingLabelId)
        {
            // For components other than Labels, automatically map a Label
            // with the same component name
            ValueTree mappingLabel = ValueTree(mappingLabelId);

            mappingLabel.setProperty(mappingComponentNameId, name, nullptr);
            mappingLabel.setProperty(mappingMapToId, parameterName, nullptr);
            mappingLabel.setProperty(mappingTypeId, "standard", nullptr);

            labelMapping.addChild(mappingLabel, -1, nullptr);
        }
    }

    deviceMapping.addChild(sliderMapping, -1, nullptr);
    deviceMapping.addChild(labelMapping, -1, nullptr);
    deviceMapping.addChild(comboBoxMapping, -1, nullptr);
    deviceMapping.addChild(textButtonMapping, -1, nullptr);
    
    DBG("ScopeSyncGUI::loadMapping - Full mapping: " + deviceMapping.toXmlString());
}

void ScopeSyncGUI::setupLookAndFeels(XmlElement& lookAndFeelsXML, bool useImageCache)
{
    forEachXmlChildElement(lookAndFeelsXML, child)
    {
        if (child->hasTagName("lookandfeel"))
        {
            setupLookAndFeel(*child, useImageCache);
        }
    }

    return;
}

void ScopeSyncGUI::setupLookAndFeel(XmlElement& lookAndFeelXML, bool useImageCache)
{
    if (lookAndFeelXML.hasAttribute("id"))
    {
        File configurationFile(scopeSync.getConfigurationFilePath().getValue());
        String configurationFileDirectory = configurationFile.getParentDirectory().getFullPathName();

        String id = lookAndFeelXML.getStringAttribute("id");
        DBG("ScopeSyncGUI::setupLookAndFeel: Setting up LookAndFeel: id = " + id);

        BCMLookAndFeel* bcmLookAndFeel;

        if (lookAndFeelXML.hasAttribute("parentid"))
        {
            String parentid = lookAndFeelXML.getStringAttribute("parentid");
            BCMLookAndFeel* parentBCMLookAndFeel = scopeSync.getBCMLookAndFeelById(parentid);

            bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, *parentBCMLookAndFeel, configurationFileDirectory);
        }
        else
        {
            bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, configurationFileDirectory, useImageCache);
        }

        if (bcmLookAndFeel != nullptr)
        {
            scopeSync.addBCMLookAndFeel(bcmLookAndFeel);
        }
        else
        {
            DBG("ScopeSyncGUI::setupLookAndFeel: Failed to set up LookAndFeel id: " + id);
        }
    }
    else
    {
        DBG("ScopeSyncGUI::setupLookAndFeel: Must supply an id for a LookAndFeel tag");
    }
    return;
}

void ScopeSyncGUI::setupDefaults(XmlElement& defaultsXML)
{
    XmlElement* sliderXML          = defaultsXML.getChildByName("slider");
    XmlElement* labelXML           = defaultsXML.getChildByName("label");
    XmlElement* textButtonXML      = defaultsXML.getChildByName("textbutton");
    XmlElement* comboBoxXML        = defaultsXML.getChildByName("combobox");
    XmlElement* componentXML       = defaultsXML.getChildByName("component");
    XmlElement* tabbedComponentXML = defaultsXML.getChildByName("tabbedcomponent");
    XmlElement* tabXML             = defaultsXML.getChildByName("tab");

    if (sliderXML != nullptr)
        defaultSliderProperties = new SliderProperties(*sliderXML);
    else
        defaultSliderProperties = new SliderProperties();
    
    if (labelXML != nullptr)
        defaultLabelProperties = new LabelProperties(*labelXML);
    else
        defaultLabelProperties = new LabelProperties();
    
    if (textButtonXML != nullptr)
        defaultTextButtonProperties = new TextButtonProperties(*textButtonXML);
    else
        defaultTextButtonProperties = new TextButtonProperties();
    
    if (comboBoxXML != nullptr)
        defaultComboBoxProperties = new ComboBoxProperties(*comboBoxXML);
    else
        defaultComboBoxProperties = new ComboBoxProperties();
    
    if (componentXML != nullptr)
        defaultComponentProperties = new ComponentProperties(*componentXML);
    else
        defaultComponentProperties = new ComponentProperties();
    
    if (tabbedComponentXML != nullptr)
        defaultTabbedComponentProperties = new TabbedComponentProperties(*tabbedComponentXML);
    else
        defaultTabbedComponentProperties = new TabbedComponentProperties();
    
    if (tabXML != nullptr)
        defaultTabProperties = new TabProperties(*tabXML);
    else
        defaultTabProperties = new TabProperties();
}

void ScopeSyncGUI::createComponent(XmlElement& componentXML)
{
    File configurationFile(scopeSync.getConfigurationFilePath().getValue());
    String configurationFileDirectory = configurationFile.getParentDirectory().getFullPathName();

    addAndMakeVisible(mainComponent = new BCMComponent(componentXML, *this, configurationFileDirectory));
    return;
}

void ScopeSyncGUI::timerCallback()
{
    if (scopeSync.guiNeedsReloading())
    {
        createGUI();
        scopeSync.setGUIReload(false);
    }
}
