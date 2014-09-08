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
#include "BCMParameter.h"
#include "../Utils/BCMMath.h"
#include "../Components/UserSettings.h"
#include "../Components/ConfigurationManager.h"
#include "Global.h"

const int ScopeSyncGUI::timerFrequency = 100;

ScopeSyncGUI::ScopeSyncGUI(ScopeSync& owner) : scopeSync(owner)
{
    createGUI(false);
    scopeSync.setGUIReload(false);
    startTimer(timerFrequency);
}

ScopeSyncGUI::~ScopeSyncGUI()
{
    stopTimer();
}

void ScopeSyncGUI::hideUserSettings()
{
    ScopeSyncApplication::hideUserSettings();
}

void ScopeSyncGUI::showUserSettings()
{
    ScopeSyncApplication::showUserSettings(mainComponent->getScreenBounds().getX(), mainComponent->getScreenBounds().getY());
}

void ScopeSyncGUI::showConfigurationManager()
{
    if (configurationManager == nullptr)
    {
        configurationManager = new ConfigurationManager(*this);
        configurationManager->setName("Configuration Manager");
        configurationManager->setOpaque(true);
        configurationManager->setVisible(true);
        configurationManager->setBounds(mainComponent->getScreenBounds().getX(), mainComponent->getScreenBounds().getY(), configurationManager->getWidth(), configurationManager->getHeight());
        configurationManager->addToDesktop(ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasDropShadow, nullptr);
        configurationManager->setAlwaysOnTop(true);
    }

    configurationManager->toFront(true);
}

void ScopeSyncGUI::chooseConfiguration()
{
    File configurationFileDirectory = scopeSync.getConfigurationDirectory();
    
    FileChooser fileChooser("Please select the configuration file you want to load...",
                            configurationFileDirectory,
                            "*.configuration");
    
    if (fileChooser.browseForFileToOpen())
    {
        scopeSync.changeConfiguration(fileChooser.getResult().getFullPathName(), false);
    }    
}

BCMParameter* ScopeSyncGUI::getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping)
{
    ValueTree componentMapping = deviceMapping.getChildWithName(componentTypeId);

    if (componentMapping.isValid())
    {
        for (int i = 0; i < componentMapping.getNumChildren(); i++)
        {
            String mappedComponentName = componentMapping.getChild(i).getProperty(Ids::name);

            if (componentName.equalsIgnoreCase(mappedComponentName))
            {
                mapping = componentMapping.getChild(i);
                String mapTo = mapping.getProperty(Ids::mapTo).toString();
                return scopeSync.getParameterByName(mapTo);
            }
        }
    }
    
    return nullptr;
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

void ScopeSyncGUI::createGUI(bool forceReload)
{
    configurationManager = nullptr;
    mainComponent        = nullptr;
    tabbedComponents.clearQuick();

    deviceMapping = scopeSync.getMapping();
    
    scopeSync.clearBCMLookAndFeels();

    // Firstly add the system LookAndFeels
    bool useImageCache = UserSettings::getInstance()->getAppProperties()->getBoolValue("useimagecache", true);
    ScopedPointer<XmlElement> systemLookAndFeels = scopeSync.getSystemLookAndFeels();
    
    setupLookAndFeels(*systemLookAndFeels, useImageCache);

    String errorText;
    String errorDetails;

    XmlElement& layoutXml = scopeSync.getLayout(errorText, errorDetails, forceReload);

    if (errorText.isNotEmpty())
        scopeSync.setSystemError(errorText, errorDetails);
    
    // Then override with any layout-specific ones
    XmlElement* child = layoutXml.getChildByName("lookandfeels");

    if (child)
        setupLookAndFeels(*child, useImageCache);

    child = layoutXml.getChildByName("defaults");

    if (child)
        setupDefaults(*child);
    else
    {
        XmlElement defaults("defaults");
        setupDefaults(defaults);
    }

    child = layoutXml.getChildByName("component");

    if (child)
        createComponent(*child);
    else
    {
        XmlElement component("component");
        createComponent(component);
    }

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
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
        String configurationFileDirectory = scopeSync.getConfigurationDirectory();
        
        String id = lookAndFeelXML.getStringAttribute("id");
        DBG("ScopeSyncGUI::setupLookAndFeel: Setting up LookAndFeel: id = " + id);

        BCMLookAndFeel* bcmLookAndFeel;

        if (lookAndFeelXML.hasAttribute("parentid"))
        {
            String parentid = lookAndFeelXML.getStringAttribute("parentid");
            BCMLookAndFeel* parentBCMLookAndFeel = scopeSync.getBCMLookAndFeelById(parentid);

            if (parentBCMLookAndFeel != nullptr)
            {
                DBG("ScopeSyncGUI::setupLookAndFeel: Found parent: id = " + parentid);
                bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, *parentBCMLookAndFeel, configurationFileDirectory);
            }
            else
                bcmLookAndFeel = new BCMLookAndFeel(lookAndFeelXML, configurationFileDirectory, useImageCache);
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
    const String& configurationFileDirectory = scopeSync.getConfigurationDirectory();
        
    addAndMakeVisible(mainComponent = new BCMComponent(*this, "MainComp"));
    
    mainComponent->applyProperties(componentXML, configurationFileDirectory);
    return;
}

void ScopeSyncGUI::timerCallback()
{
    if (scopeSync.guiNeedsReloading())
    {
        createGUI(true);
        scopeSync.setGUIReload(false);
    }
}
