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

#ifndef BCMCOMPONENT_H_INCLUDED
#define BCMCOMPONENT_H_INCLUDED

#include <JuceHeader.h>
#include "../Resources/ImageLoader.h"

class ComponentProperties;
class BCMSlider;
class BCMComboBox;
class BCMLabel;
class BCMTextButton;
class BCMTabbedComponent;
class BCMGraphic;
class BCMRectangle;
class BCMImage;
class EditToolbar;
#include "../Core/ScopeSyncGUI.h"
#include "../Components/BCMWidget.h"
#include "../Core/BCMParameterController.h"

class BCMComponent : public BCMWidget,
                     public Component,
					 public Slider::Listener,
					 public ComboBox::Listener,
                     public Value::Listener                                     
{
public:
    BCMComponent(ScopeSyncGUI& owner, BCMParameterController& pc, const String& name, bool showEditToolbar = false);
    ~BCMComponent();

    void applyProperties(XmlElement& componentXML, const String& layoutDir);
    const Identifier getComponentType() const override;
    
    // Returns width of the BCMComponent
	int getWidth() const;
    
    // Returns height of the BCMComponent
	int getHeight() const;
    
    // Called when the Juce graphics system redraws
    void paint(Graphics& g) override;

    // Callback for when a user starts to move a Slider
    void sliderDragStarted(Slider *) override;
    
    // Callback for when a user stops moving a Slider
    void sliderDragEnded(Slider *) override;
    
    // Callback for when Slider's value has been changed
    void sliderValueChanged(Slider* sliderThatWasMoved) override;

    // Callback for when ComboBox's value has been changed
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    void hideSystemErrorBar();

    // Callback for when the systemerror values change
    void valueChanged(Value& valueThatChanged) override;

    void showHideEditToolbar() const;

private:
    class EditToolbar;
    class SystemErrorBar;

    ScopedPointer<EditToolbar>     editToolbar;               // Toolbar containing edit buttons
    ScopedPointer<SystemErrorBar>  systemErrorBar;            // System error display bar
    OwnedArray<BCMSlider>          sliders;                   // BCMSliders owned by this Component
    OwnedArray<BCMLabel>           labels;                    // BCMLabels owned by this Component
    OwnedArray<BCMComboBox>        comboBoxes;                // BCMComboBoxes owned by this Component
    OwnedArray<BCMTextButton>      textButtons;               // BCMTextButtons owned by this Component
    OwnedArray<BCMComponent>       subComponents;             // BCMComponents owned by this Component
    OwnedArray<BCMTabbedComponent> tabbedComponents;          // BCMTabbedComponents owned by this Component
    OwnedArray<BCMGraphic>         graphics;                  // BCMGraphics owned by this Component
    String                         backgroundColour;          // Background colour
    Image                          backgroundImage;           // Image to display on the background
    RectanglePlacement             backgroundImagePlacement;  // Alignment of background image

    BCMParameterController& parameterController;
	SharedResourcePointer<ImageLoader> imageLoader;

    // Directory Path for the Layout file. Used for relative path sourcing of Images
    String layoutDirectory;
    
    // Indicates whether this is the "Main" Component, i.e. should toolbars be displayed etc.
    bool mainComponent;

    // Indicates whether a Component should be shown in the current context (plugin|scopefx)
	static bool showInThisContext(XmlElement& xml);

    // Loop through children of this component, creating widgets
    void setupContent(XmlElement& xml);

    // Parse any <standardcontent> elements and substitute in from
    // the built-in content
    void setupStandardContent(XmlElement& contentXML);

    // Create a new BCMSlider
    void setupSlider(XmlElement& sliderXML);
    
    // Create a new BCMLabel
    void setupLabel(XmlElement& labelXML);
    
    // Create a new BCMTextButton
    void setupTextButton(XmlElement& textButtonXML);
    
    // Create a new BCMComboBox
    void setupComboBox(XmlElement& comboBoxXML);
    
    // Create a new child BCMComponent
    void setupSubComponent(XmlElement& subComponentXML);
    
    // Create a new BCMTabbedComponent
    void setupTabbedComponent(XmlElement& tabbedComponentXML);
    
    // Add a BCMTab to a BCMTabbedComponent
    void setupTab(XmlElement& tabXML, TabbedComponent& tabbedComponent) const;

    // Draw a BCMRectangle on the canvas
	static void drawBCMRectangle(Graphics& g, BCMRectangle& rectangle);
    
    // Draw a BCMImage on the canvas
    void drawBCMImage(Graphics& g, BCMImage& image) const;

    // To handle the right-click menu
    void mouseDown(const MouseEvent& event);

    // Open the specific Style Override Panel for components as appropriate
    void overrideStyle() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMComponent);
};

#endif  // BCMCOMPONENT_H_INCLUDED
