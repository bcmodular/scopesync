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

#include "BCMLookAndFeel.h"
#include "BCMSlider.h"
#include "BCMComboBox.h"
#include "../Properties/PropertiesHelper.h"
#include "../Utils/BCMMath.h"
#include "../Core/ScopeSyncGUI.h"
#include "../Resources/ImageLoader.h"
#include "../Configuration/Configuration.h"

void FilmStripImage::initialise()
{
    image          = Image();
    mouseOverImage = Image();
    numFrames      = 0;
    frameWidth     = 0;
    frameHeight    = 0;
    isHorizontal   = false;
}

void FilmStripImage::copyFrom(const FilmStripImage& source)
{
    image          = source.image;
    mouseOverImage = source.mouseOverImage;
    numFrames      = source.numFrames;
    frameWidth     = source.frameWidth;
    frameHeight    = source.frameHeight;
    isHorizontal   = source.isHorizontal;
}

void FilmStripImage::setUp(const String& fileName,       const String& mouseOverFileName, 
                           int           numberOfFrames, bool          fsIsHorizontal, 
                           bool          useImageCache,  const String& layoutDirectory)
{
    Image newImage          = ImageLoader::getInstance()->loadImage(fileName, useImageCache, layoutDirectory);
    Image newMouseOverImage = ImageLoader::getInstance()->loadImage(mouseOverFileName, useImageCache, layoutDirectory);

    if (newImage.isValid())
    {
        if (!(newMouseOverImage.isValid()))
            newMouseOverImage = newImage;

        image          = newImage;
        mouseOverImage = newMouseOverImage;
        numFrames      = numberOfFrames;
        isHorizontal   = fsIsHorizontal;

        if (isHorizontal)
        {
            frameWidth  = image.getWidth() / numFrames;
            frameHeight = image.getHeight();
        }
        else
        {
            frameWidth  = image.getWidth();
            frameHeight = image.getHeight() / numFrames;
        }
    }
}

Image FilmStripImage::getImageAtIndex(int frameIndex, bool isMouseOver)
{
    int srcX;
    int srcY;

    if (!isHorizontal)
    {
        srcX      = 0;
        srcY      = frameIndex * frameHeight;
    }
    else
    {
        srcX      = frameIndex * frameWidth;
        srcY      = 0;
    }

    Image imageToShow;

    if (isMouseOver)
        imageToShow = mouseOverImage;
    else
        imageToShow = image;

    return imageToShow.getClippedImage(Rectangle<int>(srcX, srcY, frameWidth, frameHeight));
}

BCMLookAndFeel::BCMLookAndFeel(bool cacheImages)
{
    id = "default";
    initialise(cacheImages);
    applyProperties();
};

BCMLookAndFeel::BCMLookAndFeel(const XmlElement& lookAndFeelXML, const String& layoutDir, bool cacheImages)
{
    layoutDirectory = layoutDir;
    id = lookAndFeelXML.getStringAttribute("id");
    initialise(cacheImages);
    setValuesFromXml(lookAndFeelXML);
    applyProperties();
};

BCMLookAndFeel::BCMLookAndFeel(const XmlElement& lookAndFeelXML, const BCMLookAndFeel& parentLookAndFeel, const String& layoutDir)
{
    layoutDirectory = layoutDir;
    id = lookAndFeelXML.getStringAttribute("id");
    
    copyProperties(parentLookAndFeel);
    setValuesFromXml(lookAndFeelXML);
    applyProperties();
};

BCMLookAndFeel::~BCMLookAndFeel() {};

const String& BCMLookAndFeel::getId() { return id; };

void BCMLookAndFeel::setupColourIds()
{
    sliderColours.set("backgroundcolourid", Slider::backgroundColourId);
    sliderColours.set("thumbcolourid", Slider::thumbColourId);
    sliderColours.set("trackcolourid", Slider::trackColourId);
    sliderColours.set("rotarysliderfillcolourid", Slider::rotarySliderFillColourId);
    sliderColours.set("rotaryslideroutlinecolourid", Slider::rotarySliderOutlineColourId);
    sliderColours.set("textboxtextcolourid", Slider::textBoxTextColourId);
    sliderColours.set("textboxbackgroundcolourid", Slider::textBoxBackgroundColourId);
    sliderColours.set("textboxhighlightcolourid", Slider::textBoxHighlightColourId);
    sliderColours.set("textboxoutlinecolourid", Slider::textBoxOutlineColourId);

    labelColours.set("backgroundcolourid", Label::backgroundColourId);
    labelColours.set("textcolourid", Label::textColourId);
    labelColours.set("outlinecolourid", Label::outlineColourId);

    textButtonColours.set("buttoncolourid", juce::TextButton::buttonColourId);
    textButtonColours.set("buttononcolourid", juce::TextButton::buttonOnColourId);
    textButtonColours.set("textcolouroffid", juce::TextButton::textColourOffId);
    textButtonColours.set("textcolouronid", juce::TextButton::textColourOnId);

    tabbedComponentColours.set("backgroundcolourid", juce::TabbedComponent::backgroundColourId);
    tabbedComponentColours.set("outlinecolourid", juce::TabbedComponent::outlineColourId);

    tabbedButtonBarColours.set("taboutlinecolourid", TabbedButtonBar::tabOutlineColourId);
    tabbedButtonBarColours.set("tabtextcolourid", TabbedButtonBar::tabTextColourId);
    tabbedButtonBarColours.set("frontoutlinecolourid", TabbedButtonBar::frontOutlineColourId);
    tabbedButtonBarColours.set("fronttextcolourid", TabbedButtonBar::frontTextColourId);

    textEditorColours.set("backgroundcolourid", TextEditor::backgroundColourId);
    textEditorColours.set("textcolourid", TextEditor::textColourId);
    textEditorColours.set("highlightcolourid", TextEditor::highlightColourId);
    textEditorColours.set("highlightedtextcolourid", TextEditor::highlightedTextColourId);
    textEditorColours.set("outlinecolourid", TextEditor::outlineColourId);
    textEditorColours.set("focusedoutlinecolourid", TextEditor::focusedOutlineColourId);
    textEditorColours.set("shadowcolourid", TextEditor::shadowColourId);

    comboBoxColours.set("backgroundcolourid", ComboBox::backgroundColourId);
    comboBoxColours.set("textcolourid", ComboBox::textColourId);
    comboBoxColours.set("outlinecolourid", ComboBox::outlineColourId);
    comboBoxColours.set("buttoncolourid", ComboBox::buttonColourId);
    comboBoxColours.set("arrowcolourid", ComboBox::arrowColourId);
    
    tooltipWindowColours.set("backgroundcolourid", TooltipWindow::backgroundColourId);
    tooltipWindowColours.set("textcolourid", TooltipWindow::textColourId);
    tooltipWindowColours.set("outlinecolourid", TooltipWindow::outlineColourId);

    bubbleComponentColours.set("backgroundcolourid", BubbleComponent::backgroundColourId);
    bubbleComponentColours.set("outlinecolourid", BubbleComponent::outlineColourId);
}                       

void BCMLookAndFeel::initialise(bool cacheImages)
{
    useImageCache = cacheImages;

    rotaryBackground              = Image();
    rotaryBackgroundUseFillColour = true;
    textButtonUp                  = Image();
    textButtonDown                = Image();
    textButtonOverUp              = Image();
    textButtonOverDown            = Image();
    linearVerticalThumb           = Image();
    linearVerticalThumbBorder     = 0;
    linearHorizontalThumb         = Image();
    linearHorizontalThumbBorder   = 0;
    popupMenuFontHeight           = 17.0f;
    popupMenuFontStyleFlags       = Font::plain;

    setupColourIds();
};

void BCMLookAndFeel::copyProperties(const BCMLookAndFeel& parentLookAndFeel)
{
    useImageCache = parentLookAndFeel.useImageCache;

    rotary.copyFrom(parentLookAndFeel.rotary);
    linearVerticalBackground.copyFrom(parentLookAndFeel.linearVerticalBackground);
    linearHorizontalBackground.copyFrom(parentLookAndFeel.linearHorizontalBackground);
    
    rotaryBackground               = parentLookAndFeel.rotaryBackground;
    rotaryBackgroundUseFillColour  = parentLookAndFeel.rotaryBackgroundUseFillColour;
    textButtonUp                   = parentLookAndFeel.textButtonUp;
    textButtonDown                 = parentLookAndFeel.textButtonDown;
    textButtonOverUp               = parentLookAndFeel.textButtonOverUp;
    textButtonOverDown             = parentLookAndFeel.textButtonOverDown;
    linearVerticalThumb            = parentLookAndFeel.linearVerticalThumb;
    linearVerticalThumbBorder      = parentLookAndFeel.linearVerticalThumbBorder;
    linearHorizontalThumb          = parentLookAndFeel.linearHorizontalThumb;
    linearHorizontalThumbBorder    = parentLookAndFeel.linearVerticalThumbBorder;
    popupMenuFontHeight            = parentLookAndFeel.popupMenuFontHeight;    
    popupMenuFontStyleFlags        = parentLookAndFeel.popupMenuFontStyleFlags;
    appliesTo                      = parentLookAndFeel.appliesTo;

    setupColourIds();

    for (HashMap<int, String>::Iterator i(parentLookAndFeel.lookAndFeelColours); i.next();)
    {
        lookAndFeelColours.set(i.getKey(), i.getValue());
    }
};

void BCMLookAndFeel::setValuesFromXml(const XmlElement& lookAndFeelXML)
{
    bool firstAppliesTo = true;

    // Grab values set in XML
    forEachXmlChildElement(lookAndFeelXML, child)
    {
        if (child->hasTagName("images"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                if (subChild->hasTagName("slider"))
                {
                    forEachXmlChildElement(*subChild, subSubChild)
                    {
                             if (subSubChild->hasTagName("rotary"))           getRotarySliderImagesFromXml(*subSubChild);
                        else if (subSubChild->hasTagName("linearhorizontal")) getLinearHorizontalSliderImagesFromXml(*subSubChild);
                        else if (subSubChild->hasTagName("linearvertical"))   getLinearVerticalSliderImagesFromXml(*subSubChild);
                    }
                }
                else if (subChild->hasTagName("textbutton")) getTextButtonImagesFromXml(*subChild);
            }
        }
        else if (child->hasTagName("colours"))
        {
            forEachXmlChildElement(*child, subChild)
                getColoursFromXml(subChild->getTagName(), *subChild);
        }
        else if (child->hasTagName("fonts"))
        {
            forEachXmlChildElement(*child, subChild)
            {
                if (subChild->hasTagName("popupmenu"))
                {
                    getFontFromXml(*subChild, popupMenuFontHeight, popupMenuFontStyleFlags);
                }
            }
        }
        else if (child->hasTagName("appliesto"))
        {
            // Wipe the inherited list if we've had specific
            // items supplied at this level
            if (firstAppliesTo)
            {
                appliesTo.clear();
                firstAppliesTo = false;
            }
            
            String componentTypeString = child->getStringAttribute("componenttype");

            if (componentTypeString.equalsIgnoreCase("none"))
            {
                appliesTo.add(Ids::none);
            }                
            else
            {
                Identifier componentType = Configuration::getComponentTypeId(componentTypeString);
                
                if (componentType.isValid())
                    appliesTo.add(componentType);
            }
        }
    }
}

void BCMLookAndFeel::overrideImageIfValid(Image& imageToOverride, const String& fileName)
{
    if (fileName.isNotEmpty())
    {
        Image newImage = ImageLoader::getInstance()->loadImage(fileName, useImageCache, layoutDirectory);
        
        if (newImage.isValid())
            imageToOverride = newImage;
    }
}

void BCMLookAndFeel::setupFilmStripImageFromXml(const XmlElement& xml, FilmStripImage& filmStripImage)
{
    String fileName          = xml.getStringAttribute("filename",          String::empty);
    String mouseOverFileName = xml.getStringAttribute("mouseoverfilename", String::empty);
    int    numFrames         = xml.getIntAttribute   ("numframes",         filmStripImage.numFrames);
    bool   isHorizontal      = xml.getBoolAttribute  ("ishorizontal",      filmStripImage.isHorizontal);
    
    if (fileName.isNotEmpty())
    {
        if (mouseOverFileName.isEmpty())
            mouseOverFileName = fileName;

        filmStripImage.setUp(fileName, mouseOverFileName, numFrames, isHorizontal, useImageCache, layoutDirectory);
    }
}

void BCMLookAndFeel::getRotarySliderImagesFromXml(const XmlElement& xml)
{
    XmlElement* child = xml.getChildByName("image");

    if (child != nullptr)
        setupFilmStripImageFromXml(*child, rotary);

    overrideImageIfValid(rotaryBackground, xml.getStringAttribute("backgroundfilename", String::empty));
    rotaryBackgroundUseFillColour = xml.getBoolAttribute("backgroundusefillcolour", true);
}

void BCMLookAndFeel::getLinearHorizontalSliderImagesFromXml(const XmlElement& xml)
{
    overrideImageIfValid(linearHorizontalThumb,          xml.getStringAttribute("thumbfilename", String::empty));
    overrideImageIfValid(linearHorizontalThumbMouseOver, xml.getStringAttribute("thumbmouseoverfilename", String::empty));
    linearHorizontalThumbBorder                        = xml.getIntAttribute   ("thumbborder", linearHorizontalThumbBorder);
    
    XmlElement* child = xml.getChildByName("backgroundimage");

    if (child != nullptr)
        setupFilmStripImageFromXml(*child, linearHorizontalBackground);
}

void BCMLookAndFeel::getLinearVerticalSliderImagesFromXml(const XmlElement& xml)
{
    overrideImageIfValid(linearVerticalThumb,          xml.getStringAttribute("thumbfilename", String::empty));
    overrideImageIfValid(linearVerticalThumbMouseOver, xml.getStringAttribute("thumbmouseoverfilename", String::empty));
    linearVerticalThumbBorder                        = xml.getIntAttribute   ("thumbborder", linearHorizontalThumbBorder);
    
    XmlElement* child = xml.getChildByName("backgroundimage");

    if (child != nullptr)
        setupFilmStripImageFromXml(*child, linearVerticalBackground);
}

void BCMLookAndFeel::getTextButtonImagesFromXml(const XmlElement& xml)
{
    overrideImageIfValid(textButtonUp,       xml.getStringAttribute("upfilename", String::empty));
    overrideImageIfValid(textButtonDown,     xml.getStringAttribute("downfilename", String::empty));
    overrideImageIfValid(textButtonOverUp,   xml.getStringAttribute("mouseoverupfilename", String::empty));
    overrideImageIfValid(textButtonOverDown, xml.getStringAttribute("mouseoverdownfilename", String::empty));
}
    
void BCMLookAndFeel::getColoursFromXml(const String& colourSet, const XmlElement& xml)
{
    // DBG("BCMLookAndFeel::getColoursFromXml - xml input = " + xml.createDocument(""));

    for (int i = 0; i < xml.getNumAttributes(); i++)
    {
        String colourId    = xml.getAttributeName(i).toLowerCase();
        String colourValue = xml.getAttributeValue(i);
    
             if (colourSet == "slider" && sliderColours.contains(colourId))
            lookAndFeelColours.set(sliderColours[colourId], colourValue);
        else if (colourSet == "label" && labelColours.contains(colourId))
            lookAndFeelColours.set(labelColours[colourId], colourValue);
        else if (colourSet == "textbutton" && textButtonColours.contains(colourId))
            lookAndFeelColours.set(textButtonColours[colourId], colourValue);
        else if (colourSet == "tabbedcomponent" && tabbedComponentColours.contains(colourId))
            lookAndFeelColours.set(tabbedComponentColours[colourId], colourValue);
        else if (colourSet == "tabbar" && tabbedButtonBarColours.contains(colourId))
            lookAndFeelColours.set(tabbedButtonBarColours[colourId], colourValue);
        else if (colourSet == "texteditor" && textEditorColours.contains(colourId))
            lookAndFeelColours.set(textEditorColours[colourId], colourValue);
        else if (colourSet == "combobox" && comboBoxColours.contains(colourId))
            lookAndFeelColours.set(comboBoxColours[colourId], colourValue);
        else if (colourSet == "tooltip" && tooltipWindowColours.contains(colourId))
        {
            DBG("BCMLookAndFeel::getColoursFromXml - tooltip: " + String(tooltipWindowColours[colourId]) + ":" + String(colourValue));
            lookAndFeelColours.set(tooltipWindowColours[colourId], colourValue);
        }
        else if (colourSet == "bubblecomponent" && bubbleComponentColours.contains(colourId))
        {
            DBG("BCMLookAndFeel::getColoursFromXml - bubblecomponent: " + String(bubbleComponentColours[colourId]) + ":" + String(colourValue));
            lookAndFeelColours.set(bubbleComponentColours[colourId], colourValue);
        }
    }
}

void BCMLookAndFeel::applyProperties()
{
    for (HashMap<int, String>::Iterator i(lookAndFeelColours); i.next();)
    {
        setColour(i.getKey(), Colour::fromString(i.getValue()));
    }
}

void BCMLookAndFeel::drawRotarySlider
(
    Graphics& g,
    int  	  x,
    int  	  y,
    int  	  width,
    int  	  height,
    float  	  sliderPosProportional,
    float  	  rotaryStartAngle,
    float  	  rotaryEndAngle,
    Slider&   slider
)
{
    if (rotary.numFrames > 0)
    {
        int   frameIndex = (int)(sliderPosProportional * (rotary.numFrames - 1));
        Image indexImage = rotary.getImageAtIndex(frameIndex, slider.isMouseOverOrDragging());
        
        g.drawImageWithin(indexImage, x, y, width, height, RectanglePlacement::doNotResize);
    }
    else
    {
        // Invalid slider image, so just use the standard draw routine
        LookAndFeel_V3::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
    }
};

void BCMLookAndFeel::drawLinearSliderThumb
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
)
{
    if (linearVerticalThumb.isValid() && (sliderStyle == Slider::LinearVertical))
    {
        g.setOpacity(1.0f);

        Image image;

        if (slider.isMouseOverOrDragging())
            image = linearVerticalThumbMouseOver;
        else
            image = linearVerticalThumb;

        g.drawImageAt(image,
                      (int)(x + (width / 2.0f) - (linearVerticalThumb.getWidth() / 2.0f)),
                      (int)(sliderPos - (linearVerticalThumb.getHeight() / 2.0f)));
    }
    else if (linearHorizontalThumb.isValid() && (sliderStyle == Slider::LinearHorizontal))
    {
        Image image;

        if (slider.isMouseOverOrDragging())
            image = linearHorizontalThumbMouseOver;
        else
            image = linearHorizontalThumb;

        g.setOpacity(1.0f);
        g.drawImageAt(image,
                      (int)(sliderPos - (linearHorizontalThumb.getWidth() / 2.0f)),
                      (int)(y + (height / 2.0f) - (linearHorizontalThumb.getHeight() / 2.0f)));
    }
    else
    {
        LookAndFeel_V3::drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, sliderStyle, slider);
    }
}

int BCMLookAndFeel::getSliderThumbRadius (Slider& slider)
{
    if (linearVerticalThumb.isValid() && (slider.getSliderStyle() == Slider::LinearVertical))
        return jmax(linearVerticalThumb.getWidth() / 2, linearVerticalThumb.getHeight() / 2) + linearVerticalThumbBorder;
    else if (linearHorizontalThumb.isValid() && (slider.getSliderStyle() == Slider::LinearHorizontal))
        return jmax(linearHorizontalThumb.getWidth() / 2, linearHorizontalThumb.getHeight() / 2) + linearHorizontalThumbBorder;
    else
        return LookAndFeel_V3::getSliderThumbRadius(slider);
}

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
)
{
    if (style == Slider::LinearVertical && linearVerticalBackground.numFrames > 0)
    {
        int   frameIndex = (int)((slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum()) * (linearVerticalBackground.numFrames - 1));
        Image indexImage = linearVerticalBackground.getImageAtIndex(frameIndex, slider.isMouseOverOrDragging());
        
        g.setOpacity(1.0f);
        g.drawImageAt(indexImage, (int)(x + (width / 2.0f) - (indexImage.getWidth() / 2.0f)), y);
    }
    else if (style == Slider::LinearHorizontal && linearHorizontalBackground.numFrames > 0)
    {
        int   frameIndex = (int)((slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum()) * (linearHorizontalBackground.numFrames - 1));
        Image indexImage = linearHorizontalBackground.getImageAtIndex(frameIndex, slider.isMouseOverOrDragging());
        
        g.setOpacity(1.0f);
        g.drawImageAt(indexImage, x, (int)(y + (height / 2.0f) - (indexImage.getHeight() / 2.0f)));
    }
    else
    {
        LookAndFeel_V3::drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void BCMLookAndFeel::drawButtonBackground
(
    Graphics&     g,
    Button&       button,
    const Colour& backgroundColour,
    bool 	      isMouseOverButton,
    bool 	      isButtonDown
)
{
    bool toggleState = button.getToggleState();

    if (textButtonUp.isValid() && textButtonDown.isValid() && textButtonOverDown.isValid() && textButtonOverUp.isValid())
    {
        Image buttonImage;

        if (isMouseOverButton)
        {
            if (isButtonDown || toggleState)
                buttonImage = textButtonOverDown;
            else
                buttonImage = textButtonOverUp;
        }
        else
        {
            if (isButtonDown || toggleState)
                buttonImage = textButtonDown;
            else
                buttonImage = textButtonUp;
        }

        g.drawImage(buttonImage, 0, 0, buttonImage.getWidth(), buttonImage.getHeight(), 0, 0, buttonImage.getWidth(), buttonImage.getHeight());
    }
    else
        LookAndFeel_V3::drawButtonBackground(g, button, backgroundColour, isMouseOverButton, isButtonDown);
}

Label* BCMLookAndFeel::createSliderTextBox(Slider& slider)
{
    Label*      label     = LookAndFeel_V2::createSliderTextBox(slider);
    BCMSlider*  bcmSlider = dynamic_cast<BCMSlider*>(&slider);
    
    if (bcmSlider)
    {
        label->setJustificationType(bcmSlider->justificationFlags);
        label->setFont(Font(bcmSlider->fontHeight, bcmSlider->fontStyleFlags));
    }

    return label;
}

Font BCMLookAndFeel::getComboBoxFont (ComboBox& box)
{
    BCMComboBox* bcmComboBox = dynamic_cast<BCMComboBox*>(&box);
    
    if (bcmComboBox)
        return Font (jmin (bcmComboBox->fontHeight, box.getHeight() * 0.85f), bcmComboBox->fontStyleFlags);
    else
        return Font (jmin (15.0f, box.getHeight() * 0.85f));
}

Label* BCMLookAndFeel::createComboBoxTextBox(ComboBox& comboBox)
{
    Label*       label       = LookAndFeel_V2::createComboBoxTextBox(comboBox);
    BCMComboBox* bcmComboBox = dynamic_cast<BCMComboBox*>(&comboBox);
    
    if (bcmComboBox)
        label->setFont(Font(bcmComboBox->fontHeight, bcmComboBox->fontStyleFlags));

    return label;
}

Font BCMLookAndFeel::getPopupMenuFont()
{
    return Font (popupMenuFontHeight, popupMenuFontStyleFlags);
}

void BCMLookAndFeel::drawCallOutBoxBackground(CallOutBox& /* box */, Graphics& g,
                                              const Path& path, Image& /* cachedImage */)
{
    g.setColour(Colour::greyLevel(0.23f).withAlpha(1.0f));
    g.fillPath(path);

    g.setColour(Colours::white.withAlpha(1.0f));
    g.strokePath(path, PathStrokeType(2.0f));
}

int BCMLookAndFeel::appliesToComponentType(const Identifier& componentType)
{
    // Applies generally
    if (appliesTo.size() == 0)
        return 2;

    // Applies specifically
    if (appliesTo.indexOf(Ids::none) == -1 && appliesTo.indexOf(componentType) > -1)
        return 1;

    // Doesn't apply
    return 0;
}

void BCMLookAndFeel::drawTabAreaBehindFrontButton (TabbedButtonBar& bar, Graphics& g, const int w, const int h)
{
    const float shadowSize = 0.15f;

    Rectangle<int> shadowRect, line;
    ColourGradient gradient (Colours::black.withAlpha (bar.isEnabled() ? 0.08f : 0.04f), 0, 0,
                             Colours::transparentBlack, 0, 0, false);

    switch (bar.getOrientation())
    {
        case TabbedButtonBar::TabsAtLeft:
            gradient.point1.x = (float) w;
            gradient.point2.x = w * (1.0f - shadowSize);
            shadowRect.setBounds ((int) gradient.point2.x, 0, w - (int) gradient.point2.x, h);
            line.setBounds (w - 1, 0, 1, h);
            break;

        case TabbedButtonBar::TabsAtRight:
            gradient.point2.x = w * shadowSize;
            shadowRect.setBounds (0, 0, (int) gradient.point2.x, h);
            line.setBounds (0, 0, 1, h);
            break;

        case TabbedButtonBar::TabsAtTop:
            gradient.point1.y = (float) h;
            gradient.point2.y = h * (1.0f - shadowSize);
            shadowRect.setBounds (0, (int) gradient.point2.y, w, h - (int) gradient.point2.y);
            line.setBounds (0, h - 1, w, 1);
            break;

        case TabbedButtonBar::TabsAtBottom:
            gradient.point2.y = h * shadowSize;
            shadowRect.setBounds (0, 0, w, (int) gradient.point2.y);
            line.setBounds (0, 0, w, 1);
            break;

        default: break;
    }

    NamedValueSet&   barProperties = bar.getProperties();

    bool showDropShadow = barProperties.getWithDefault("showdropshadow", true);
    
    if (showDropShadow)
    {
        g.setGradientFill (gradient);
        g.fillRect (shadowRect.expanded (2, 2));
    }

    g.setColour (bar.findColour (TabbedButtonBar::tabOutlineColourId));
    g.fillRect (line);
}
