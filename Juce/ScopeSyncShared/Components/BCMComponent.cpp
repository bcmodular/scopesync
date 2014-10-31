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
#include "../Configuration/ConfigurationManager.h"

/* =========================================================================
 * SystemErrorBar
 */
class BCMComponent::SystemErrorBar : public Component,
                                     public Button::Listener,
                                     public ChangeBroadcaster
{
public:
    SystemErrorBar(const String& errorText, const String& errorDetails, int width, int height)
        : systemErrorDetailsButton(),
          closeButton()
    {
        systemErrorLabel.setColour(Label::textColourId, Colour::fromString("ffa7aaae"));
        systemErrorLabel.setColour(Label::backgroundColourId, Colour::fromString("ff000000"));
        systemErrorLabel.setColour(Label::outlineColourId, Colour::fromString("ffa7aaae"));
        systemErrorLabel.setText(errorText, dontSendNotification);
        systemErrorLabel.setJustificationType(Justification::centred);
        addAndMakeVisible(systemErrorLabel);
        
        systemErrorDetailsButton.setImages(true, false, false,
            ImageLoader::getInstance()->loadImage("helpOff",  true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOver", true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOn",   true, String::empty), 1.0f, Colours::transparentBlack);

        details = errorDetails;
        addAndMakeVisible(systemErrorDetailsButton);
        systemErrorDetailsButton.setVisible(details.isNotEmpty());
        systemErrorDetailsButton.addListener(this);

        closeButton.setImages(true, false, false,
            ImageLoader::getInstance()->loadImage("helpOff",  true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOver", true, String::empty), 1.0f, Colours::transparentBlack,
            ImageLoader::getInstance()->loadImage("helpOn",   true, String::empty), 1.0f, Colours::transparentBlack);

        addAndMakeVisible(closeButton);
        closeButton.addListener(this);

        setSize(width, height);
    }

    void buttonClicked(Button* buttonThatWasClicked) override
    {
        if (buttonThatWasClicked == &systemErrorDetailsButton)
            showSystemErrorDetails();
        else if (buttonThatWasClicked == &closeButton)
            hideSystemError();
    }

    void setErrorText(const String& errorText, const String& errorDetails)
    {
        systemErrorLabel.setText(errorText, dontSendNotification);
        details = errorDetails;
        systemErrorDetailsButton.setVisible(details.isNotEmpty());
    }

private:
    String      details;
    Label       systemErrorLabel;
    ImageButton systemErrorDetailsButton;
    ImageButton closeButton;

    void resized() override
    {
        Rectangle<int> localBounds(getLocalBounds().reduced(8, 8));
    
        systemErrorDetailsButton.setBounds(localBounds.removeFromRight(18));
        closeButton.setBounds(localBounds.removeFromRight(18));
        systemErrorLabel.setBounds(localBounds.reduced(0, 2));
    }

    class SystemErrorDetailsCallout : public Component
    {
    public:
        SystemErrorDetailsCallout(const String& boxText, SystemErrorBar& parent)
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
        SystemErrorDetailsCallout* errorDetailsBox = new SystemErrorDetailsCallout(details, *this);
        CallOutBox::launchAsynchronously(errorDetailsBox, systemErrorDetailsButton.getScreenBounds(), nullptr);
    }

    void hideSystemError()
    {
        sendChangeMessage();
    }
};

/* =========================================================================
 * EditToolbar
 */
class BCMComponent::EditToolbar : public Component
{
public:
    EditToolbar(ScopeSync& ss, int width, int height)    
    : scopeSync(ss),
      addButton("New"),
      saveButton("Save"),
      saveAsButton("Save As..."),
      undoButton("Undo"),
      redoButton("Redo")
    {
        commandManager = scopeSync.getCommandManager();
        
        setButtonImages(addButton, "newConfigOff", "newConfigOver", "newConfigOn", Colours::transparentBlack);
        addButton.setCommandToTrigger(commandManager, CommandIDs::addConfig, true);
        addAndMakeVisible(addButton);

        setButtonImages(saveButton, "saveOff", "saveOver", "saveOn", Colours::transparentBlack);
        saveButton.setCommandToTrigger(commandManager, CommandIDs::saveConfig, true);
        addAndMakeVisible(saveButton);

        setButtonImages(saveAsButton, "saveAsOff", "saveAsOver", "saveAsOn", Colours::transparentBlack);
        saveAsButton.setCommandToTrigger(commandManager, CommandIDs::saveConfigAs, true);
        addAndMakeVisible(saveAsButton);

        setButtonImages(undoButton, "undoOff", "undoOver", "undoOn", Colours::transparentBlack);
        undoButton.setCommandToTrigger(commandManager, CommandIDs::undo, true);
        addAndMakeVisible(undoButton);

        setButtonImages(redoButton, "redoOff", "redoOver", "redoOn", Colours::transparentBlack);
        redoButton.setCommandToTrigger(commandManager, CommandIDs::redo, true);
        addAndMakeVisible(redoButton);

        setButtonImages(editToolbarShowButton, "closeOff", "closeOver", "closeOn", Colours::transparentBlack);
        editToolbarShowButton.setCommandToTrigger(commandManager, CommandIDs::showHideEditToolbar, true);
        addAndMakeVisible(editToolbarShowButton);

        setSize(width, height);
    }

private:
    ScopeSync&                 scopeSync;
    ApplicationCommandManager* commandManager;
    
    ImageButton addButton;
    ImageButton saveButton;
    ImageButton saveAsButton;
    ImageButton undoButton;
    ImageButton redoButton;
    ImageButton editToolbarShowButton;
    
    void resized() override
    {
        Rectangle<int> toolbar(getLocalBounds().reduced(4, 4));
    
        toolbar.removeFromLeft(8);
        addButton.setBounds(toolbar.removeFromLeft(20).reduced(2, 2));
        toolbar.removeFromLeft(6);
        saveButton.setBounds(toolbar.removeFromLeft(20));
        toolbar.removeFromLeft(6);
        saveAsButton.setBounds(toolbar.removeFromLeft(20));
        toolbar.removeFromLeft(16);
        undoButton.setBounds(toolbar.removeFromLeft(20));
        toolbar.removeFromLeft(6);
        redoButton.setBounds(toolbar.removeFromLeft(20));
        toolbar.removeFromLeft(6);
        editToolbarShowButton.setBounds(toolbar.removeFromLeft(10));
    }

    void paint(Graphics& g) override
    {
        g.drawImageAt(ImageLoader::getInstance()->loadImage("divider", true, String::empty), 91, 8);
    }

    void setButtonImages(ImageButton& button, const String& normalImage, const String& overImage, const String& downImage, const Colour& overlayColour)
    {
        button.setImages(true, true, true,
                         ImageLoader::getInstance()->loadImage(normalImage, true, ""), 1.0f, overlayColour,
                         ImageLoader::getInstance()->loadImage(overImage,   true, ""), 1.0f, overlayColour,
                         ImageLoader::getInstance()->loadImage(downImage,   true, ""), 1.0f, overlayColour, 0);
    }
};

BCMComponent::BCMComponent(ScopeSyncGUI& owner, const String& name, bool isMainComponent)
    : BCMWidget(owner), Component(name), mainComponent(isMainComponent)
{
    setParentWidget(this);
    setWantsKeyboardFocus(true);
    
    if (mainComponent)
        scopeSync.getSystemError().addListener(this);
}

BCMComponent::~BCMComponent()
{
    if (mainComponent)
        scopeSync.getSystemError().removeListener(this);
}

void BCMComponent::applyProperties(XmlElement& componentXML, const String& layoutDir)
{
    // Firstly set up properties for this Component
    ComponentProperties properties(componentXML, *(scopeSyncGUI.defaultComponentProperties));
    
    applyWidgetProperties(properties);
    layoutDirectory = layoutDir;

    backgroundColour = styleOverride.getProperty(Ids::fillColour, properties.backgroundColour);
    
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

    if (mainComponent)
    {
        if (!scopeSync.configurationIsReadOnly())
        {
            editToolbar = new EditToolbar(scopeSyncGUI.getScopeSync(), componentBounds.width, 40);
        
            Rectangle<int> editToolbarBounds = getLocalBounds().removeFromBottom(40).removeFromLeft(226);
        
            bool showEditToolbar = scopeSync.shouldShowEditToolbar();
            DBG("BCMComponent::applyProperties - Show Edit Toolbar: " + String(showEditToolbar));
            
            if (!showEditToolbar)
            {
                editToolbarBounds.translate(-146, 0);
            }
        
            editToolbar->setBounds(editToolbarBounds);
            addAndMakeVisible(editToolbar);
        }

        if (scopeSync.getSystemError().toString().isNotEmpty())
        {
            systemErrorBar = new SystemErrorBar(scopeSync.getSystemError().toString(), 
                                                scopeSync.getSystemErrorDetails().toString(), 
                                                componentBounds.width, 40);
            systemErrorBar->setBounds(getLocalBounds().removeFromBottom(40));
            systemErrorBar->addChangeListener(this);
            addAndMakeVisible(systemErrorBar);
        }
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

void BCMComponent::showHideEditToolbar()
{
    Rectangle<int> editToolbarBounds(getLocalBounds().removeFromBottom(40).removeFromLeft(226));
    
    bool showEditToolbar = scopeSync.shouldShowEditToolbar();
    DBG("BCMComponent::showHideEditToolbar - Show Edit Toolbar: " + String(showEditToolbar));
        
    if (showEditToolbar)
        editToolbarBounds.translate(-146, 0);
    
    Desktop::getInstance().getAnimator().animateComponent(editToolbar, editToolbarBounds, 1.0f, 300, true, 1.0f, 1.0f);
    
    scopeSync.toggleEditToolbar();

    showEditToolbar = scopeSync.shouldShowEditToolbar();
    DBG("BCMComponent::showHideEditToolbar - Show Edit Toolbar (after toggle): " + String(showEditToolbar));
}

void BCMComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == systemErrorBar)
        systemErrorBar = nullptr;
    else
        BCMWidget::changeListenerCallback(source);
}

void BCMComponent::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(scopeSyncGUI.getScopeSync().getSystemError()))
    {
        if (systemErrorBar == nullptr)
        {
            if (valueThatChanged.toString().isNotEmpty())
            {
                systemErrorBar = new SystemErrorBar(valueThatChanged.toString(), 
                                                    scopeSync.getSystemErrorDetails().toString(), 
                                                    componentBounds.width, 40);
                systemErrorBar->setBounds(getLocalBounds().removeFromBottom(40));
                systemErrorBar->addChangeListener(this);
                addAndMakeVisible(systemErrorBar);
            }
        }
        else
        {
            systemErrorBar->setErrorText(valueThatChanged.toString(), scopeSync.getSystemErrorDetails().toString());
        }
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

void BCMComponent::overrideStyle()
{
    ConfigurationManagerCallout* configurationManagerCallout = new ConfigurationManagerCallout(scopeSyncGUI.getScopeSync(), 550, 60);
    configurationManagerCallout->setStyleOverridePanel(styleOverride, Ids::component, getName(), backgroundColour);
    configurationManagerCallout->addChangeListener(this);
    CallOutBox::launchAsynchronously(configurationManagerCallout, getScreenBounds(), nullptr);
}

void BCMComponent::mouseDown(const MouseEvent& event)
{
    DBG("BCMComponent::mouseDown  - component id: " + getComponentID());

    if (event.mods.isPopupMenu())
    {
        //grabKeyboardFocus();
        showPopupMenu();
    }
    else
    {
        Component::mouseDown(event);
    }
}
