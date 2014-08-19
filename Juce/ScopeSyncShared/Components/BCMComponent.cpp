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

BCMComponent::BCMComponent(ScopeSyncGUI& owner, const String& name) : Component(name), gui(owner) {}

BCMComponent::~BCMComponent() {}

void BCMComponent::applyProperties(XmlElement& componentXML, const String& configDirectory)
{
    // Firstly set up properties for this Component
    ComponentProperties properties(componentXML, *(gui.defaultComponentProperties));
    
    setComponentID(properties.id);
    
    configurationFileDirectoryPath = configDirectory;

    backgroundColour = properties.backgroundColour;

    componentBounds = properties.bounds;
    
    if (properties.backgroundImageFileName.isNotEmpty())
    {
        bool useImageCache = gui.getScopeSync().getAppProperties().getBoolValue("useimagecache", true);

        backgroundImage = ImageLoader::getInstance()->loadImage(properties.backgroundImageFileName, useImageCache, configurationFileDirectoryPath);
        
        if (componentBounds.width == 0 || componentBounds.height == 0)
        {
            if (backgroundImage.isValid())
            {
                componentBounds.width  = backgroundImage.getWidth();
                componentBounds.height = backgroundImage.getHeight();
            }
        }
    }

    BCM_SET_BOUNDS
    
    BCM_SET_LOOK_AND_FEEL

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

void BCMComponent::paint(Graphics& g)
{
    g.fillAll(Colour::fromString(backgroundColour));

    if (backgroundImage.isValid())
    {
        g.setOpacity(1.0f);
    
        g.drawImageWithin(backgroundImage, 0, 0, getWidth(), getHeight(), backgroundImagePlacement);
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
        
        String name = getName() + ":" + String(subComponents.size());

        addAndMakeVisible(subComponent = new BCMComponent(gui, name));

        subComponent->applyProperties(subComponentXML, configurationFileDirectoryPath);
        subComponents.add(subComponent);
    }
}

void BCMComponent::setupTabbedComponent(XmlElement& tabbedComponentXML)
{
    if (showInThisContext(tabbedComponentXML))
    {
        TabbedComponentProperties tabbedComponentProperties(tabbedComponentXML, *(gui.defaultTabbedComponentProperties));
        BCMTabbedComponent*       tabbedComponent;

        // Setup Tabbed Component object
        addAndMakeVisible(tabbedComponent = new BCMTabbedComponent(tabbedComponentProperties.tabBarOrientation, gui));

        tabbedComponent->applyProperties(tabbedComponentProperties);

        gui.addTabbedComponent(tabbedComponent);
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
        TabProperties tabProperties(tabXML, *(gui.defaultTabProperties));
        XmlElement*   componentXML = nullptr;

        componentXML = tabXML.getChildByName("component");
        
        if (componentXML != nullptr && showInThisContext(*componentXML))
        {
            String name = getName() + ":" + tabProperties.name + ":" + String(tabbedComponent.getNumTabs());

            BCMComponent* subComponent;

            tabbedComponent.addTab(
                tabProperties.name,
                Colour::fromString(tabProperties.backgroundColour),
                subComponent = new BCMComponent(gui, name),
                true,
                tabProperties.idx
            );

            subComponent->applyProperties(*componentXML, configurationFileDirectoryPath);
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
        SliderProperties sliderProperties(sliderXML, *(gui.defaultSliderProperties));
        BCMSlider*       slider;

        // Setup slider object
        addAndMakeVisible(slider = new BCMSlider(sliderProperties.name, gui));
        slider->addListener(this);

        slider->applyProperties(sliderProperties);
        sliders.add(slider);
    }
}

void BCMComponent::setupLabel(XmlElement& labelXML)
{
    if (showInThisContext(labelXML))
    {
        LabelProperties labelProperties(labelXML, *(gui.defaultLabelProperties));
        BCMLabel* label;

        String labelName = labelProperties.name;
        String labelText = labelProperties.text;

        // Setup label object
        addAndMakeVisible(label = new BCMLabel(labelName, labelText));

        label->applyProperties(labelProperties, gui);
        labels.add(label);
    }
}

void BCMComponent::setupTextButton(XmlElement& textButtonXML)
{
    if (showInThisContext(textButtonXML))
    {
        TextButtonProperties textButtonProperties(textButtonXML, *(gui.defaultTextButtonProperties));
        BCMTextButton* textButton;
    
        addAndMakeVisible (textButton = new BCMTextButton(gui, textButtonProperties.name));

        textButton->applyProperties(textButtonProperties);
        textButtons.add(textButton);
    }
}

void BCMComponent::setupComboBox(XmlElement& comboBoxXML)
{
    if (showInThisContext(comboBoxXML))
    {
        ComboBoxProperties comboBoxProperties(comboBoxXML, *(gui.defaultComboBoxProperties));
        BCMComboBox* comboBox;

        addAndMakeVisible (comboBox = new BCMComboBox(comboBoxProperties.name));
        comboBox->addListener(this);
    
        comboBox->applyProperties(gui, comboBoxProperties);
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
        gui.getScopeSync().setParameterFromGUI(*(bcmSlider->getParameter()), value);
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
        gui.getScopeSync().setParameterFromGUI(*(bcmComboBox->getParameter()), (float)selectedIndex);
    }
}

void BCMComponent::sliderDragStarted(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        gui.getScopeSync().beginParameterChangeGesture(*(bcmSlider->getParameter()));
    }
}

void BCMComponent::sliderDragEnded(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        gui.getScopeSync().endParameterChangeGesture(*(bcmSlider->getParameter()));
    }
}
