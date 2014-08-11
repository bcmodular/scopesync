/**
 * The BCModular version of Juce's Component, which adds the ability
 * to be created from an XML definition, as well as being tied into
 * the ScopeSync parameter system. Manages the memory allocation
 * for all sub-components and also processes various UI callbacks.
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

#include "BCMComponent.h"
#include "ImageLoader.h"
#include "../Components/BCMLabel.h"
#include "../Components/BCMSlider.h"
#include "../Components/BCMComboBox.h"
#include "../Components/BCMTextButton.h"
#include "../Components/BCMTabbedComponent.h"
#include "../Properties/ComponentProperties.h"
#include "../Core/ScopeSyncGUI.h"

BCMComponent::BCMComponent(XmlElement& componentXML, ScopeSyncGUI& owner, const String& configDirectory) : scopeSyncGUI(owner)
{
    // Firstly set up properties for this Component
    ComponentProperties properties(componentXML, *(scopeSyncGUI.defaultComponentProperties));
    
    configurationFileDirectoryPath = configDirectory;

    width  = properties.width;
    height = properties.height;
    x      = properties.x;
    y      = properties.y;

    if (properties.backgroundImageFileName.isNotEmpty())
    {
        bool useImageCache = scopeSyncGUI.getScopeSync().getAppProperties().getBoolValue("useimagecache", true);

        backgroundImage = ImageLoader::getInstance()->loadImage(properties.backgroundImageFileName, useImageCache, configurationFileDirectoryPath);
        
        if (width == 0 || height == 0)
        {
            if (backgroundImage.isValid())
            {
                width = backgroundImage.getWidth();
                height = backgroundImage.getHeight();
            }
        }
    }

    backgroundColour = properties.backgroundColour;

    setLookAndFeel(scopeSyncGUI.getScopeSync().getBCMLookAndFeelById(properties.bcmLookAndFeelId));

    setBounds(x, y, width, height);

    // Then loop through child component elements
    forEachXmlChildElement(componentXML, child)
    {
             if (child->hasTagName("component"))       setupSubComponent(*child);
        else if (child->hasTagName("slider"))          setupSlider(*child);
        else if (child->hasTagName("label"))           setupLabel(*child);
        else if (child->hasTagName("textbutton"))      setupTextButton(*child);
        else if (child->hasTagName("tabbedcomponent")) setupTabbedComponent(*child);
        else if (child->hasTagName("combobox"))        setupComboBox(*child);
    }
}

BCMComponent::~BCMComponent() {}

void BCMComponent::paint(Graphics& g)
{
    g.fillAll(Colour::fromString(backgroundColour));

    if (backgroundImage.isValid())
    {
        g.setOpacity(1.0f);
    
        g.drawImageWithin(backgroundImage, 0, 0, width, height, backgroundImagePlacement);
    }
}

bool BCMComponent::showInThisContext(XmlElement& xml)
{
    String displayContext = xml.getStringAttribute("displaycontext");
    
    if (displayContext.equalsIgnoreCase("scope") && ScopeSync::inPluginContext())
        return false;
    else if (displayContext.equalsIgnoreCase("host") && ScopeSync::inScopeFXContext())
        return false;
    else
        return true;
}

void BCMComponent::setupSubComponent(XmlElement& subComponentXML)
{
    if (showInThisContext(subComponentXML))
    {
        BCMComponent* subComponent;
    
        addAndMakeVisible(subComponent = new BCMComponent(subComponentXML, scopeSyncGUI, configurationFileDirectoryPath));
        subComponents.add(subComponent);
    }
}

void BCMComponent::setupTabbedComponent(XmlElement& tabbedComponentXML)
{
    if (showInThisContext(tabbedComponentXML))
    {
        TabbedComponentProperties tabbedComponentProperties(tabbedComponentXML, *(scopeSyncGUI.defaultTabbedComponentProperties));
        BCMTabbedComponent*       tabbedComponent;

        // Setup Tabbed Component object
        addAndMakeVisible(tabbedComponent = new BCMTabbedComponent(tabbedComponentProperties, scopeSyncGUI, tabbedComponentProperties.tabBarOrientation));
        scopeSyncGUI.addTabbedComponent(tabbedComponent);
        tabbedComponents.add(tabbedComponent);
    
        forEachXmlChildElementWithTagName(tabbedComponentXML, child, "tab")
        {
            setupTab(*child, *tabbedComponent);
        }
    }
}

void BCMComponent::setupTab(XmlElement& tabXML, TabbedComponent& tabbedComponent)
{
    if (showInThisContext(tabXML))
    {
        TabProperties tabProperties(tabXML, *(scopeSyncGUI.defaultTabProperties));
        XmlElement*   componentXML = nullptr;

        componentXML = tabXML.getChildByName("component");
        
        if (componentXML != nullptr && showInThisContext(*componentXML))
        {
            tabbedComponent.addTab(
                tabProperties.name,
                Colour::fromString(tabProperties.backgroundColour),
                new BCMComponent(*componentXML, scopeSyncGUI, configurationFileDirectoryPath),
                true,
                tabProperties.idx
            );
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "No component found for tab: " + tabProperties.name);
        }
    }
}

void BCMComponent::setupSlider(XmlElement& sliderXML)
{
    if (showInThisContext(sliderXML))
    {
        SliderProperties sliderProperties(sliderXML, *(scopeSyncGUI.defaultSliderProperties));
        BCMSlider*       slider;

        // Setup slider object
        addAndMakeVisible(slider = new BCMSlider(sliderProperties, scopeSyncGUI, sliderProperties.name));
        slider->addListener(this);

        sliders.add(slider);
    }
}

void BCMComponent::setupLabel(XmlElement& labelXML)
{
    if (showInThisContext(labelXML))
    {
        LabelProperties labelProperties(labelXML, *(scopeSyncGUI.defaultLabelProperties));
        BCMLabel* label;

        String labelName = labelProperties.name;
        String labelText = labelProperties.text;

        // Setup label object
        addAndMakeVisible(label = new BCMLabel(labelProperties, scopeSyncGUI, labelName, labelText));

        labels.add(label);
    }
}

void BCMComponent::setupTextButton(XmlElement& textButtonXML)
{
    if (showInThisContext(textButtonXML))
    {
        TextButtonProperties textButtonProperties(textButtonXML, *(scopeSyncGUI.defaultTextButtonProperties));
        BCMTextButton* textButton;
    
        addAndMakeVisible (textButton = new BCMTextButton(textButtonProperties, scopeSyncGUI, textButtonProperties.name));

        textButtons.add(textButton);
    }
}

void BCMComponent::setupComboBox(XmlElement& comboBoxXML)
{
    if (showInThisContext(comboBoxXML))
    {
        ComboBoxProperties comboBoxProperties(comboBoxXML, *(scopeSyncGUI.defaultComboBoxProperties));
        BCMComboBox* comboBox;

        addAndMakeVisible (comboBox = new BCMComboBox(comboBoxProperties, scopeSyncGUI, comboBoxProperties.name));
        comboBox->addListener(this);
    
        comboBoxes.add(comboBox);
    }
}

void BCMComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(sliderThatWasMoved);
    String name = sliderThatWasMoved->getName();
    float value = (float)sliderThatWasMoved->getValue();

    DBG("BCMComponent::sliderValueChanged: " + name + ", orig value: " + String(value));
        
    if (bcmSlider && bcmSlider->hasParameter())
    {
        scopeSyncGUI.getScopeSync().setParameterFromGUI(bcmSlider->getParamIdx(), value);
    }
    return;
}

void BCMComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    BCMComboBox* bcmComboBox = dynamic_cast<BCMComboBox*>(comboBoxThatHasChanged);
    String name          = comboBoxThatHasChanged->getName();
    String itemSelected  = comboBoxThatHasChanged->getText();
    int    selectedIndex = comboBoxThatHasChanged->getSelectedItemIndex();
    
    //DBG("BCMComponent::comboBoxChanged: " + name + ", selectedIndex: " + String(selectedIndex) + ", itemSelected: " + itemSelected);
        
    if (bcmComboBox && bcmComboBox->hasParameter())
    {
        scopeSyncGUI.getScopeSync().setParameterFromGUI(bcmComboBox->getParamIdx(), (float)selectedIndex);
    }
}

void BCMComponent::sliderDragStarted(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        scopeSyncGUI.getScopeSync().beginParameterChangeGesture(bcmSlider->getParamIdx());
    }
}

void BCMComponent::sliderDragEnded(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        scopeSyncGUI.getScopeSync().endParameterChangeGesture(bcmSlider->getParamIdx());
    }
}
