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
#include "../Resources/ImageLoader.h"
#include "../Components/BCMLabel.h"
#include "../Components/BCMSlider.h"
#include "../Components/BCMComboBox.h"
#include "../Components/BCMTextButton.h"
#include "../Components/BCMTabbedComponent.h"
#include "../Components/BCMRectangle.h"
#include "../Components/BCMImage.h"
#include "../Properties/ComponentProperties.h"
#include "../Components/UserSettings.h"
#include "../Core/ScopeSyncApplication.h"
#include "../Core/Global.h"
#include "../Properties/TabbedComponentProperties.h"
#include "../Properties/TabProperties.h"
#include "../Properties/SliderProperties.h"
#include "../Properties/LabelProperties.h"
#include "../Properties/TextButtonProperties.h"
#include "../Properties/ComboBoxProperties.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Core/ScopeSync.h"


class EditToolbar : public Component,
                    public Value::Listener,
                    public Button::Listener
{
public:
    EditToolbar(ScopeSync& ss, int width, int height)    
    : scopeSync(ss),
      saveButton("Save"),
      saveAsButton("Save As..."),
      applyChangesButton("Apply Changes"),
      discardChangesButton("Discard All Unsaved Changes"),
      undoButton("Undo"),
      redoButton("Redo"),
      systemErrorDetailsButton()
    {
        commandManager = scopeSync.getCommandManager();
        
        setButtonImages(saveButton, "saveOff", "saveOver", "saveOn", Colours::transparentBlack);
        saveButton.setCommandToTrigger(commandManager, CommandIDs::saveConfig, true);
        addAndMakeVisible(saveButton);

        setButtonImages(saveAsButton, "saveAsOff", "saveAsOver", "saveAsOn", Colours::transparentBlack);
        saveAsButton.setCommandToTrigger(commandManager, CommandIDs::saveConfigAs, true);
        addAndMakeVisible(saveAsButton);

        setButtonImages(applyChangesButton, "confirmOff", "confirmOver", "confirmOn", Colours::transparentBlack);
        applyChangesButton.setCommandToTrigger(commandManager, CommandIDs::applyConfigChanges, true);
        addAndMakeVisible(applyChangesButton);

        setButtonImages(discardChangesButton, "closeOff", "closeOver", "closeOn", Colours::transparentBlack);
        discardChangesButton.setCommandToTrigger(commandManager, CommandIDs::discardConfigChanges, true);
        addAndMakeVisible(discardChangesButton);

        setButtonImages(undoButton, "undoOff", "undoOver", "undoOn", Colours::transparentBlack);
        undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);
        addAndMakeVisible(undoButton);

        setButtonImages(redoButton, "redoOff", "redoOver", "redoOn", Colours::transparentBlack);
        redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);
        addAndMakeVisible(redoButton);

        scopeSync.getSystemError().addListener(this);
        String labelText = scopeSync.getSystemError().toString();
        systemErrorLabel.setColour(Label::textColourId, Colour::fromString("ffa7aaae"));
        systemErrorLabel.setColour(Label::backgroundColourId, Colour::fromString("ff000000"));
        systemErrorLabel.setColour(Label::outlineColourId, Colour::fromString("ffa7aaae"));
        systemErrorLabel.setText(labelText, dontSendNotification);
        systemErrorLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(systemErrorLabel);
        systemErrorLabel.setVisible(labelText.isNotEmpty());

        scopeSync.getSystemErrorDetails().addListener(this);
        systemErrorDetailsButton.setImages(true, false, false,
            ImageLoader::getInstance()->loadImage("helpOff",  true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOver", true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOn",   true, String::empty), 1.0f, Colours::transparentBlack);

        addAndMakeVisible(systemErrorDetailsButton);
        systemErrorDetailsButton.setVisible(scopeSync.getSystemErrorDetails().toString().isNotEmpty());
        systemErrorDetailsButton.addListener(this);

        setSize(width, height);
    }

    ~EditToolbar()
    {
        scopeSync.getSystemError().removeListener(this);    
        scopeSync.getSystemErrorDetails().removeListener(this);
    }

    // Callback for when the systemerror values change
    void valueChanged(Value& valueThatChanged) override
    {
        if (valueThatChanged.refersToSameSourceAs(scopeSync.getSystemError()))
        {
            String text = valueThatChanged.getValue().toString();
            systemErrorLabel.setText(text, dontSendNotification);
            systemErrorLabel.setVisible(text.isNotEmpty());
        }

        if (valueThatChanged.refersToSameSourceAs(scopeSync.getSystemErrorDetails()))
        {
            String text = valueThatChanged.getValue().toString();
            systemErrorDetailsButton.setVisible(text.isNotEmpty());
        }
    }

    void buttonClicked(Button* /* buttonThatWasClicked */) override
    {
        showSystemErrorDetails();
    }

private:
    ScopeSync&                 scopeSync;
    ApplicationCommandManager* commandManager;
    Label                      systemErrorLabel;
    ImageButton                systemErrorDetailsButton;
    
    ImageButton saveButton;
    ImageButton saveAsButton;
    ImageButton applyChangesButton;
    ImageButton discardChangesButton;
    ImageButton undoButton;
    ImageButton redoButton;
    
    void resized() override
    {
        Rectangle<int> toolbar(getLocalBounds().reduced(8, 8));
    
        saveButton.setBounds(toolbar.removeFromLeft(40));
        saveAsButton.setBounds(toolbar.removeFromLeft(40));
        toolbar.removeFromLeft(16);
        applyChangesButton.setBounds(toolbar.removeFromLeft(40));
        discardChangesButton.setBounds(toolbar.removeFromLeft(40));
        toolbar.removeFromLeft(16);
        undoButton.setBounds(toolbar.removeFromLeft(40));
        redoButton.setBounds(toolbar.removeFromLeft(40));
        toolbar.removeFromLeft(8);
        systemErrorDetailsButton.setBounds(toolbar.removeFromRight(18));
        systemErrorLabel.setBounds(toolbar.reduced(0, 2));
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::darkgrey);

        g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 94, 8);
        g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 188, 8);
    }

    void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour)
    {
        button.setImages(true, true, true,
                         ImageLoader::getInstance()->loadImage(normalImage, true, ""), 1.0f, overlayColour,
                         ImageLoader::getInstance()->loadImage(overImage,   true, ""), 1.0f, overlayColour,
                         ImageLoader::getInstance()->loadImage(downImage,   true, ""), 1.0f, overlayColour, 0);
    }

    class SystemErrorDetailsCallout : public Component
    {
    public:
        SystemErrorDetailsCallout(const String& boxText, EditToolbar& parent)
            : errorDetailsBox("Callout box")
        {
            errorDetailsBox.setText(boxText);
            errorDetailsBox.setMultiLine(true, true);
            errorDetailsBox.setReadOnly(true);
            errorDetailsBox.setCaretVisible(false);
            errorDetailsBox.setScrollbarsShown(true);
            errorDetailsBox.setLookAndFeel(&parent.getLookAndFeel());
            addAndMakeVisible(errorDetailsBox);
            errorDetailsBox.setBounds(getLocalBounds());
            setSize(500, 75);
        }

    private:
        void resized() override { errorDetailsBox.setBounds(getLocalBounds()); }
        TextEditor errorDetailsBox;
    };

    void showSystemErrorDetails()
    {
        SystemErrorDetailsCallout* errorDetailsBox = new SystemErrorDetailsCallout(scopeSync.getSystemErrorDetails().getValue(), *this);
        CallOutBox::launchAsynchronously(errorDetailsBox, systemErrorDetailsButton.getScreenBounds(), nullptr);
    }
};

BCMComponent::BCMComponent(ScopeSyncGUI& owner, const String& name, bool showEditToolbar)
    : BCMWidget(owner, this), Component(name), editToolbarShown(showEditToolbar)
{
    setWantsKeyboardFocus(true);
}

BCMComponent::~BCMComponent() {}

void BCMComponent::applyProperties(XmlElement& componentXML, const String& layoutDir)
{
    // Firstly set up properties for this Component
    ComponentProperties properties(componentXML, *(scopeSyncGUI.defaultComponentProperties));
    
    applyWidgetProperties(properties);
    layoutDirectory = layoutDir;
    backgroundColour = properties.backgroundColour;
    
    if (properties.backgroundImageFileName.isNotEmpty())
    {
        bool useImageCache = UserSettings::getInstance()->getPropertyBoolValue("useimagecache", true);

        backgroundImage = ImageLoader::getInstance()->loadImage(properties.backgroundImageFileName, useImageCache, layoutDirectory);
        
        if (componentBounds.width == 0 || componentBounds.height == 0)
        {
            if (backgroundImage.isValid())
            {
                componentBounds.width  = backgroundImage.getWidth();
                componentBounds.height = backgroundImage.getHeight();
            }
        }
    }

    // Then loop through child component elements
    forEachXmlChildElement(componentXML, child)
    {
             if (child->hasTagName("component"))       setupSubComponent(*child);
        else if (child->hasTagName("slider"))          setupSlider(*child);
        else if (child->hasTagName("label"))           setupLabel(*child);
        else if (child->hasTagName("textbutton"))      setupTextButton(*child);
        else if (child->hasTagName("tabbedcomponent")) setupTabbedComponent(*child);
        else if (child->hasTagName("combobox"))        setupComboBox(*child);
        else if (child->hasTagName("rectangle"))       graphics.add(new BCMRectangle(*child));
        else if (child->hasTagName("image"))           graphics.add(new BCMImage(*child));
    }

    if (editToolbarShown)
    {
        editToolbar = new EditToolbar(scopeSyncGUI.getScopeSync(), componentBounds.width, 40);
        Rectangle<int> localBounds(getLocalBounds());
        editToolbar->setBounds(getLocalBounds().removeFromBottom(40));
        addAndMakeVisible(editToolbar);
    }
}

const Identifier BCMComponent::getComponentType() const { return Ids::component; };
    
void BCMComponent::paint(Graphics& g)
{
    g.fillAll(Colour::fromString(backgroundColour));

    if (backgroundImage.isValid())
    {
        g.setOpacity(1.0f);
        g.drawImageWithin(backgroundImage, 0, 0, getWidth(), getHeight(), backgroundImagePlacement);
    }

    for (int i = 0; i < graphics.size(); i++)
    {
        BCMRectangle* rectangle = dynamic_cast<BCMRectangle*>(graphics[i]);
    
        if (rectangle)
        {
            drawBCMRectangle(g, *rectangle);
            continue;
        }

        BCMImage* image = dynamic_cast<BCMImage*>(graphics[i]);
        
        if (image)
        {
            drawBCMImage(g, *image);
            continue;
        }
    }
}

void BCMComponent::drawBCMRectangle(Graphics& g, BCMRectangle& rectangle)
{
    if (rectangle.cornerSize >= 1.0f)
    {
        // We're drawing a rounded rectangle
        g.setColour(Colour::fromString(rectangle.fillColour));
        g.fillRoundedRectangle((float)rectangle.bounds.x, (float)rectangle.bounds.y, (float)rectangle.bounds.width, (float)rectangle.bounds.height, rectangle.cornerSize);

        if (rectangle.outlineThickness > 0.0f)
        {
            g.setColour(Colour::fromString(rectangle.outlineColour));
            g.drawRoundedRectangle((float)rectangle.bounds.x, (float)rectangle.bounds.y, (float)rectangle.bounds.width, (float)rectangle.bounds.height, 
                                    rectangle.cornerSize, rectangle.outlineThickness);
        }
    }
    else
    {
        // We're drawing a regular rectangle
        g.setColour(Colour::fromString(rectangle.fillColour));
        g.fillRect(rectangle.bounds.x, rectangle.bounds.y, rectangle.bounds.width, rectangle.bounds.height);

        if (rectangle.outlineThickness > 0.0f)
        {
            g.setColour(Colour::fromString(rectangle.outlineColour));
            g.drawRect(rectangle.bounds.x, rectangle.bounds.y, rectangle.bounds.width, rectangle.bounds.height, roundDoubleToInt(rectangle.outlineThickness));
        }
    }
}

void BCMComponent::drawBCMImage(Graphics& g, BCMImage& image)
{
    bool useImageCache = UserSettings::getInstance()->getPropertyBoolValue("useimagecache", true);

    Image loadedImage = ImageLoader::getInstance()->loadImage(image.fileName, useImageCache, layoutDirectory);

    if (loadedImage.isValid())
    {
        if (image.opacity < 1.0f)
            g.setColour(Colours::black.withAlpha(image.opacity));
        else
            g.setColour(Colours::black);
    
        if (image.stretchMode == BCMImage::stretchedToFit)
        {
            g.drawImage(loadedImage, image.bounds.x, image.bounds.y, image.bounds.width, image.bounds.height,
                        0, 0, loadedImage.getWidth(), loadedImage.getHeight());
        }
        else
        {
            RectanglePlacement::Flags placement = RectanglePlacement::centred;

            if (image.stretchMode == BCMImage::maintainAspectOnlyReduce)
                placement = (RectanglePlacement::Flags)(placement | RectanglePlacement::onlyReduceInSize);

            g.drawImageWithin(loadedImage, image.bounds.x, image.bounds.y, image.bounds.width, image.bounds.height, placement, false);
        }
    }
}

bool BCMComponent::showInThisContext(XmlElement& xml)
{
    String displayContext = xml.getStringAttribute("displaycontext");
    
    if (displayContext.equalsIgnoreCase("scope") && ScopeSyncApplication::inPluginContext())
        return false;
    else if (displayContext.equalsIgnoreCase("host") && ScopeSyncApplication::inScopeFXContext())
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

        addAndMakeVisible(subComponent = new BCMComponent(scopeSyncGUI, name));

        subComponent->applyProperties(subComponentXML, layoutDirectory);
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
        addAndMakeVisible(tabbedComponent = new BCMTabbedComponent(tabbedComponentProperties.tabBarOrientation, scopeSyncGUI));

        tabbedComponent->applyProperties(tabbedComponentProperties);

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
            String name = getName() + ":" + tabProperties.name + ":" + String(tabbedComponent.getNumTabs());

            BCMComponent* subComponent;

            tabbedComponent.addTab(
                tabProperties.name,
                Colour::fromString(tabProperties.backgroundColour),
                subComponent = new BCMComponent(scopeSyncGUI, name),
                true,
                tabProperties.idx
            );

            subComponent->applyProperties(*componentXML, layoutDirectory);
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
        SliderProperties sliderProperties(scopeSyncGUI, sliderXML, *(scopeSyncGUI.defaultSliderProperties));
        BCMSlider*       slider;

        // Setup slider object
        addAndMakeVisible(slider = new BCMSlider(sliderProperties.name, scopeSyncGUI));
        slider->addListener(this);

        slider->applyProperties(sliderProperties);
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
        addAndMakeVisible(label = new BCMLabel(labelName, labelText, scopeSyncGUI));

        label->applyProperties(labelProperties);
        labels.add(label);
    }
}

void BCMComponent::setupTextButton(XmlElement& textButtonXML)
{
    if (showInThisContext(textButtonXML))
    {
        TextButtonProperties textButtonProperties(textButtonXML, *(scopeSyncGUI.defaultTextButtonProperties));
        BCMTextButton* textButton;
    
        addAndMakeVisible (textButton = new BCMTextButton(scopeSyncGUI, textButtonProperties.name));

        textButton->applyProperties(textButtonProperties);
        textButtons.add(textButton);
    }
}

void BCMComponent::setupComboBox(XmlElement& comboBoxXML)
{
    if (showInThisContext(comboBoxXML))
    {
        ComboBoxProperties comboBoxProperties(comboBoxXML, *(scopeSyncGUI.defaultComboBoxProperties));
        BCMComboBox* comboBox;

        addAndMakeVisible (comboBox = new BCMComboBox(comboBoxProperties.name, scopeSyncGUI));
        comboBox->addListener(this);
    
        comboBox->applyProperties(comboBoxProperties);
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
        scopeSyncGUI.getScopeSync().setParameterFromGUI(*(bcmSlider->getParameter()), value);
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
        scopeSyncGUI.getScopeSync().setParameterFromGUI(*(bcmComboBox->getParameter()), (float)selectedIndex);
    }
}

void BCMComponent::sliderDragStarted(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        scopeSyncGUI.getScopeSync().beginParameterChangeGesture(bcmSlider->getParameter());
    }
}

void BCMComponent::sliderDragEnded(Slider* slider)
{
    BCMSlider* bcmSlider = dynamic_cast<BCMSlider*>(slider);

    if (bcmSlider && bcmSlider->hasParameter())
    {
        scopeSyncGUI.getScopeSync().endParameterChangeGesture(bcmSlider->getParameter());
    }
}
