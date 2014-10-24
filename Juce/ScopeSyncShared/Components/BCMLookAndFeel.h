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

class FilmStripImage
{
public:
    int    numFrames;
    Image  image;
    Image  mouseOverImage;
    bool   isHorizontal;
    int    frameWidth;
    int    frameHeight;

    FilmStripImage() { initialise(); }
    void initialise();
    void copyFrom(const FilmStripImage& source);
    void setUp(const String& fileName,       const String& mouseOverFileName, 
               int           numberOfFrames, bool          fsIsHorizontal, 
               bool          useImageCache,  const String& layoutDirectory);

    // Cut a specific slice from a film-strip image
    Image getImageAtIndex(int frameIndex, bool isMouseOver);
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilmStripImage);
};

class BCMLookAndFeel : public LookAndFeel_V3
{
public:
    // In case no lookandfeel element provided, default constructor
    BCMLookAndFeel(bool cacheImages);
    
    // Constructor for use when setting up LookAndFeel object from XML where we
    // are not copying values from a parent
    BCMLookAndFeel(const XmlElement& lookAndFeelXML, const String& layoutDir, bool cacheImages);
    
    // Constructor for use when overriding parent values for a specific LookAndFeel
    BCMLookAndFeel(const XmlElement& lookAndFeelXML, const BCMLookAndFeel& parentLookAndFeel, const String& layoutDirectory);
    
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

    void drawRotaryFillBackground(Graphics& g, int x, int y, int width, int height, Slider& slider);
    void drawRotaryOutlineBackground(Graphics& g, int x, int y, int width, int height, Slider& slider);

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
    
    // Callback for creating a Slider's inc/dec buttons. Overridden to pick up overridden colours
    Button* createSliderButton(Slider&, bool isIncrement) override;

    // Returns font for a ComboBox. Overridden to allow control of font
    Font   getComboBoxFont (ComboBox& box);
    
    // Callback for creating a ComboBox's textbox. Overridden to allow control of font
    Label* createComboBoxTextBox(ComboBox& comboBox);
    
    // Returns font for a PopupMenu (also used by ComboBoxes). Overridden to allow control of font
    Font   getPopupMenuFont();

    // Callback for drawing a CallOutBox's background. Overridden to fix performance issue on Windows 8
    void drawCallOutBoxBackground (CallOutBox&, Graphics&, const Path& path, Image& cachedImage) override;

    // Returns a result to indicate whether the BCMLookAndFeel applies to the supplied Component Type
    // 0: Doesn't apply, 1: Applies specifically, 2: Applies generally
    int  appliesToComponentType(const Identifier& componentType);

private:
    String id;            // Identifier for a BCMLookAndFeel
    bool   useImageCache; // Flags as to whether the Image Cache should be used
    
    Array<Identifier> appliesTo; // Array of component types the BCMLookAndFeel is relevant to (used to
                                 // restrict drop-down lists for Style Overrides)

    // Variables holding LookAndFeel attributes to be applied on drawing Components
    FilmStripImage rotary;
    Image rotaryFillBackground;
    Image rotaryOutlineBackground;
    bool  rotaryBackgroundFillBehind;
    bool  rotaryBackgroundUseFillColour;
    
    Image  linearVerticalThumb;
    Image  linearVerticalThumbMouseOver;
    int	   linearVerticalThumbBorder;
    
    FilmStripImage linearVerticalBackground;
    
    Image  linearHorizontalThumb;
    Image  linearHorizontalThumbMouseOver;
    int	   linearHorizontalThumbBorder;
    
    FilmStripImage linearHorizontalBackground;
    
    Image  textButtonUp;
    Image  textButtonDown;
    Image  textButtonOverUp;
    Image  textButtonOverDown;
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
    HashMap<String, TooltipWindow::ColourIds>   tooltipWindowColours;
    HashMap<String, BubbleComponent::ColourIds> bubbleComponentColours;
    HashMap<String, PopupMenu::ColourIds>       popupMenuColours;
    
    // Directory Path for the Layout file. Used for relative path sourcing of Images
    String layoutDirectory;

    // Sets up the various HashMaps containing ColourId mapping from XML tags
    void setupColourIds();
    
    // Set sensible defaults for a LookAndFeel
    void initialise(bool cacheImages);

    // Copy the properties from a parent LookAndFeel
    void copyProperties(const BCMLookAndFeel& parentLookAndFeel);
    
    // Override default values using those provided in an XML Element
    void setValuesFromXml(const XmlElement& lookAndFeelXML);
    
    // Utility methods to read information from XML
    void overrideImageIfValid(Image& imageToOverride, const String& fileName);
    void setupFilmStripImageFromXml(const XmlElement& xml, FilmStripImage& filmStripImage);
    void getRotarySliderImagesFromXml(const XmlElement& xml);
    void getLinearVerticalSliderImagesFromXml(const XmlElement& xml);
    void getLinearHorizontalSliderImagesFromXml(const XmlElement& xml);
    void getTextButtonImagesFromXml(const XmlElement& xml);
    void getColoursFromXml(const String& colourSet, const XmlElement& xml);
    
    // Use the properties to set the various LookAndFeel values
    void applyProperties();
    
    // Callback to draw the area behind TabbedComponent's Tabs. Overridden to allow
    // the drop-shadow to be disabled
    void drawTabAreaBehindFrontButton(TabbedButtonBar& bar, Graphics& g, int w, int h);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMLookAndFeel);
};

#endif