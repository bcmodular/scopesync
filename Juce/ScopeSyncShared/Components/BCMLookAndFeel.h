/**
 * BCModular's LookAndFeel for ScopeSync (derived from Juce's
 * LookAndFeel_V3). Allows LookAndFeels to be defined from XML.
 * Also has support for fixed or filmstrip images for certain
 * types of Component.
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

#ifndef __BCMLOOKANDFEEL_HEADER__
#define __BCMLOOKANDFEEL_HEADER__

#include <JuceHeader.h>

class BCMLookAndFeel : public LookAndFeel_V3
{
public:
    // In case no lookandfeel element provided, default constructor
    BCMLookAndFeel(bool cacheImages);
    
    // Constructor for use when setting up LookAndFeel object from XML where we
    // are not copying values from a parent
    BCMLookAndFeel(const XmlElement& lookAndFeelXML, const String& configDirectory, bool cacheImages);
    
    // Constructor for use when overriding parent values for a specific LookAndFeel
    BCMLookAndFeel(const XmlElement& lookAndFeelXML, const BCMLookAndFeel& parentLookAndFeel, const String& configDirectory);
    
    ~BCMLookAndFeel();

    // Returns the identifier for a BCMLookAndFeel
    const String& getId();
    
    // Callback for drawing a Rotary Slider. Supports filmstrip images.
    void drawRotarySlider
    (
        Graphics &g,
        int  	x,
        int  	y,
        int  	width,
        int  	height,
        float  	sliderPosProportional,
        float  	rotaryStartAngle,
        float  	rotaryEndAngle,
        Slider  &slider
    );

    // Callback for drawing a Linear Slider's thumb. Supports fixed images.
    void drawLinearSliderThumb
    (
        Graphics& g,
        int 	  x,
        int 	  y,
        int 	  width,
        int 	  height,
        float 	  sliderPos,
        float 	  minSliderPos,
        float 	  maxSliderPos,
        const Slider::SliderStyle sliderStyle,
        Slider&   slider
    );
    
    // Returns the radius of a Slider thumb. Overridden from LookAndFeel_V3's
    // so it can handle situation when images being used for thumb
    int getSliderThumbRadius(Slider& slider);

    // Callback for drawing a Linear Slider's background. Supports filmstrip images.
    void BCMLookAndFeel::drawLinearSliderBackground
    (
        Graphics& g,
        int       x,
        int       y,
        int       width,
        int       height,
        float     sliderPos,
        float     minSliderPos,
        float     maxSliderPos,
        const Slider::SliderStyle style,
        Slider&   slider
    );

    // Callback for drawing a Button's background. Supports fixed images.
    void drawButtonBackground
    (
        Graphics&     g,
        Button&       button,
        const Colour& backgroundColour,
        bool 	      isMouseOverButton,
        bool 	      isButtonDown
    );

    // Callback for creating a Slider's textbox. Overridden to allow control of font and justification
    Label* createSliderTextBox(Slider& slider);
    
    // Returns font for a ComboBox. Overridden to allow control of font
    Font   getComboBoxFont (ComboBox& box);
    
    // Callback for creating a ComboBox's textbox. Overridden to allow control of font
    Label* createComboBoxTextBox(ComboBox& comboBox);
    
    // Returns font for a PopupMenu (also used by ComboBoxes). Overridden to allow control of font
    Font   getPopupMenuFont();

    // Callback for drawing a CallOutBox's background. Overridden to fix performance issue on Windows 8
    void drawCallOutBoxBackground (CallOutBox&, Graphics&, const Path& path, Image& cachedImage) override;

private:
    String id;            // Identifier for a BCMLookAndFeel
    bool   useImageCache; // Flags as to whether the Image Cache should be used
    
    // Variables holding LookAndFeel attributes to be applied on drawing Components
    String rotaryFileName;
    String rotaryMouseOverFileName;
    Image  rotary;
    Image  rotaryMouseOver;
    int    rotaryNumFrames;
    int    rotaryFrameHeight;
    int	   rotaryFrameWidth;
    bool   rotaryIsHorizontal;
    String linearVerticalThumbFileName;
    Image  linearVerticalThumb;
    String linearVerticalThumbMouseOverFileName;
    Image  linearVerticalThumbMouseOver;
    int	   linearVerticalThumbBorder;
    String linearVerticalBackgroundFileName;
    Image  linearVerticalBackground;
    String linearVerticalBackgroundMouseOverFileName;
    Image  linearVerticalBackgroundMouseOver;
    int    linearVerticalBackgroundNumFrames;
    int    linearVerticalBackgroundFrameHeight;
    int	   linearVerticalBackgroundFrameWidth;
    bool   linearVerticalBackgroundIsHorizontal;
    String linearHorizontalThumbFileName;
    Image  linearHorizontalThumb;
    String linearHorizontalThumbMouseOverFileName;
    Image  linearHorizontalThumbMouseOver;
    int	   linearHorizontalThumbBorder;
    String linearHorizontalBackgroundFileName;
    Image  linearHorizontalBackground;
    String linearHorizontalBackgroundMouseOverFileName;
    Image  linearHorizontalBackgroundMouseOver;
    int    linearHorizontalBackgroundNumFrames;
    int    linearHorizontalBackgroundFrameHeight;
    int	   linearHorizontalBackgroundFrameWidth;
    bool   linearHorizontalBackgroundIsHorizontal;
    String textButtonUpFileName;
    String textButtonDownFileName;
    String textButtonOverUpFileName;
    String textButtonOverDownFileName;
    Image  textButtonUp;
    Image  textButtonDown;
    Image  textButtonOverUp;
    Image  textButtonOverDown;
    bool   useTextButtonImage;
    bool   useLinearVerticalSliderImage;
    bool   useLinearHorizontalSliderImage;
    float  popupMenuFontHeight;
    Font::FontStyleFlags popupMenuFontStyleFlags;
    
    // Colour Handling variables
    HashMap<int, String>                        lookAndFeelColours;
    HashMap<String, Slider::ColourIds>          sliderColours;
    HashMap<String, Label::ColourIds>           labelColours;
    HashMap<String, TextButton::ColourIds>      textButtonColours;
    HashMap<String, TabbedComponent::ColourIds> tabbedComponentColours;
    HashMap<String, TabbedButtonBar::ColourIds> tabbedButtonBarColours;
    HashMap<String, TextEditor::ColourIds>      textEditorColours;
    HashMap<String, ComboBox::ColourIds>        comboBoxColours;

    // Directory File Path for the Configuration file. Used for relative path sourcing of Images
    String configurationFileDirectoryPath;

    // Sets up the various HashMaps containing ColourId mapping from XML tags
    void setupColourIds();
    
    // Set sensible defaults for a LookAndFeel
    void initialise(bool cacheImages);

    // Copy the properties from a parent LookAndFeel
    void copyProperties(const BCMLookAndFeel& parentLookAndFeel);
    
    // Override default values using those provided in an XML Element
    void setValuesFromXml(const XmlElement& lookAndFeelXML);
    
    // Utility methods to read information from XML
    void getRotarySliderImagesFromXml(const XmlElement& xml);
    void getLinearVerticalSliderImagesFromXml(const XmlElement& xml);
    void getLinearHorizontalSliderImagesFromXml(const XmlElement& xml);
    void getTextButtonImagesFromXml(const XmlElement& xml);
    void getColoursFromXml(const String& colourSet, const XmlElement& xml);
    
    // Use the properties to set the various LookAndFeel values
    void applyProperties();
    void setRotarySliderImage();
    void setLinearSliderImages();
    void setTextButtonImages();
    
    // Callback to draw the area behind TabbedComponent's Tabs. Overridden to allow
    // the drop-shadow to be disabled
    void drawTabAreaBehindFrontButton(TabbedButtonBar& bar, Graphics& g, int w, int h);

    // Utility method to cut a specific slice from a film-strip image
    Image filmStripIndexImage
    (
        Image& filmStripImage,
        bool   isHorizontal,
        int    frameWidth,
        int    frameHeight,
        int    frameNumber
    );

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMLookAndFeel);
};

#endif