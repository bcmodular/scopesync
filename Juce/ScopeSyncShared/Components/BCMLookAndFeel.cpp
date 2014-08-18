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
#include "ImageLoader.h"

BCMLookAndFeel::BCMLookAndFeel(bool cacheImages)
{
    id = "default";
    initialise(cacheImages);
    applyProperties();
};

BCMLookAndFeel::BCMLookAndFeel(const XmlElement& lookAndFeelXML, const String& configDirectory, bool cacheImages)
{
    configurationFileDirectoryPath = configDirectory;
    id = lookAndFeelXML.getStringAttribute("id");
    initialise(cacheImages);
    setValuesFromXml(lookAndFeelXML);
    applyProperties();
};

BCMLookAndFeel::BCMLookAndFeel(const XmlElement& lookAndFeelXML, const BCMLookAndFeel& parentLookAndFeel, const String& configDirectory)
{
    configurationFileDirectoryPath = configDirectory;
    id = lookAndFeelXML.getStringAttribute("id");
    //DBG("BCMLookAndFeel::BCMLookAndFeel - Creating " + id);

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
}                       

void BCMLookAndFeel::initialise(bool cacheImages)
{
    useImageCache = cacheImages;

    useTextButtonImage                     = false;
    useLinearVerticalSliderImage           = false;
    useLinearHorizontalSliderImage         = false;
    rotaryFileName                         = String::empty;
    rotaryNumFrames                        = 0;
    rotaryFrameWidth                       = 0;
    rotaryFrameHeight                      = 0;
    rotaryIsHorizontal                     = false;
    textButtonUpFileName                   = String::empty;
    textButtonDownFileName                 = String::empty;
    textButtonOverUpFileName               = String::empty;
    textButtonOverDownFileName             = String::empty;
    linearVerticalThumbFileName            = String::empty;
    linearVerticalThumbBorder              = 0;
    linearVerticalBackgroundFileName       = String::empty;
    linearVerticalBackgroundNumFrames      = 0;
    linearVerticalBackgroundFrameHeight    = 0;
    linearVerticalBackgroundFrameWidth     = 0;
    linearVerticalBackgroundIsHorizontal   = false;
    linearHorizontalThumbFileName          = String::empty;
    linearHorizontalThumbBorder            = 0;
    linearHorizontalBackgroundFileName     = String::empty;
    linearHorizontalBackgroundNumFrames    = 0;
    linearHorizontalBackgroundFrameHeight  = 0;
    linearHorizontalBackgroundFrameWidth   = 0;
    linearHorizontalBackgroundIsHorizontal = false;
    popupMenuFontHeight                    = 17.0f;
    popupMenuFontStyleFlags                = Font::plain;

    setupColourIds();
};

void BCMLookAndFeel::copyProperties(const BCMLookAndFeel& parentLookAndFeel)
{
    useImageCache = parentLookAndFeel.useImageCache;

    useTextButtonImage                     = parentLookAndFeel.useTextButtonImage;
    useLinearVerticalSliderImage           = parentLookAndFeel.useLinearVerticalSliderImage;
    useLinearHorizontalSliderImage         = parentLookAndFeel.useLinearHorizontalSliderImage;
    rotaryFileName                         = parentLookAndFeel.rotaryFileName;
    rotaryNumFrames                        = parentLookAndFeel.rotaryNumFrames;
    rotaryFrameWidth                       = parentLookAndFeel.rotaryFrameWidth;
    rotaryFrameHeight                      = parentLookAndFeel.rotaryFrameHeight;
    rotaryIsHorizontal                     = parentLookAndFeel.rotaryIsHorizontal;
    textButtonUpFileName                   = parentLookAndFeel.textButtonUpFileName;
    textButtonDownFileName                 = parentLookAndFeel.textButtonDownFileName;
    textButtonOverUpFileName               = parentLookAndFeel.textButtonOverUpFileName;
    textButtonOverDownFileName             = parentLookAndFeel.textButtonOverDownFileName;
    linearVerticalThumbFileName            = parentLookAndFeel.linearVerticalThumbFileName;
    linearVerticalThumbBorder              = parentLookAndFeel.linearVerticalThumbBorder;
    linearVerticalBackgroundFileName       = parentLookAndFeel.linearVerticalBackgroundFileName;
    linearVerticalBackgroundNumFrames      = parentLookAndFeel.linearVerticalBackgroundNumFrames;
    linearVerticalBackgroundFrameHeight    = parentLookAndFeel.linearVerticalBackgroundFrameHeight;
    linearVerticalBackgroundFrameWidth     = parentLookAndFeel.linearVerticalBackgroundFrameWidth;
    linearVerticalBackgroundIsHorizontal   = parentLookAndFeel.linearVerticalBackgroundIsHorizontal;
    linearHorizontalThumbFileName          = parentLookAndFeel.linearHorizontalThumbFileName;
    linearHorizontalThumbBorder            = parentLookAndFeel.linearVerticalThumbBorder;
    linearHorizontalBackgroundFileName     = parentLookAndFeel.linearHorizontalBackgroundFileName;
    linearHorizontalBackgroundNumFrames    = parentLookAndFeel.linearHorizontalBackgroundNumFrames;
    linearHorizontalBackgroundFrameHeight  = parentLookAndFeel.linearHorizontalBackgroundFrameHeight;
    linearHorizontalBackgroundFrameWidth   = parentLookAndFeel.linearHorizontalBackgroundFrameWidth;
    linearHorizontalBackgroundIsHorizontal = parentLookAndFeel.linearHorizontalBackgroundIsHorizontal;
    popupMenuFontHeight                    = parentLookAndFeel.popupMenuFontHeight;    
    popupMenuFontStyleFlags                = parentLookAndFeel.popupMenuFontStyleFlags;

    setupColourIds();

    for (HashMap<int, String>::Iterator i(parentLookAndFeel.lookAndFeelColours); i.next();)
    {
        lookAndFeelColours.set(i.getKey(), i.getValue());
    }
};

void BCMLookAndFeel::setValuesFromXml(const XmlElement& lookAndFeelXML)
{
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
    }
};

void BCMLookAndFeel::getRotarySliderImagesFromXml(const XmlElement& xml)
{
    rotaryFileName     = xml.getStringAttribute("filename",     rotaryFileName);
    rotaryNumFrames    = xml.getIntAttribute   ("numframes",    rotaryNumFrames);
    rotaryIsHorizontal = xml.getBoolAttribute  ("ishorizontal", rotaryIsHorizontal);
}

void BCMLookAndFeel::getLinearHorizontalSliderImagesFromXml(const XmlElement& xml)
{
    linearHorizontalThumbFileName          = xml.getStringAttribute("thumbfilename",          linearHorizontalThumbFileName);
    linearHorizontalThumbBorder            = xml.getIntAttribute   ("thumbborder",            linearHorizontalThumbBorder);
    linearHorizontalBackgroundFileName     = xml.getStringAttribute("backgroundfilename",     linearHorizontalBackgroundFileName);
    linearHorizontalBackgroundNumFrames    = xml.getIntAttribute   ("backgroundnumframes",    linearHorizontalBackgroundNumFrames);
    linearHorizontalBackgroundIsHorizontal = xml.getBoolAttribute  ("backgroundishorizontal", linearHorizontalBackgroundIsHorizontal);
}

void BCMLookAndFeel::getLinearVerticalSliderImagesFromXml(const XmlElement& xml)
{
    linearVerticalThumbFileName          = xml.getStringAttribute("thumbfilename",          linearVerticalThumbFileName);
    linearVerticalThumbBorder            = xml.getIntAttribute   ("thumbborder",            linearVerticalThumbBorder);
    linearVerticalBackgroundFileName     = xml.getStringAttribute("backgroundfilename",     linearVerticalBackgroundFileName);
    linearVerticalBackgroundNumFrames    = xml.getIntAttribute   ("backgroundnumframes",    linearVerticalBackgroundNumFrames);
    linearVerticalBackgroundIsHorizontal = xml.getBoolAttribute  ("backgroundishorizontal", linearVerticalBackgroundIsHorizontal);
}

void BCMLookAndFeel::getTextButtonImagesFromXml(const XmlElement& xml)
{
    textButtonUpFileName       = xml.getStringAttribute("upfilename",            textButtonUpFileName);
    textButtonDownFileName     = xml.getStringAttribute("downfilename",          textButtonDownFileName);
    textButtonOverUpFileName   = xml.getStringAttribute("mouseoverupfilename",   textButtonOverUpFileName);
    textButtonOverDownFileName = xml.getStringAttribute("mouseoverdownfilename", textButtonOverDownFileName);
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
    }
}

void BCMLookAndFeel::applyProperties()
{
    setRotarySliderImage();
    setTextButtonImages();
    setLinearSliderImages();

    for (HashMap<int, String>::Iterator i(lookAndFeelColours); i.next();)
    {
        //DBG("BCMLookAndFeel::applyProperties - Colour Set = " + String(i.getKey()) + ", " + i.getValue());

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
    if (rotaryNumFrames > 0)
    {
        int frameIndex = (int)(sliderPosProportional * (rotaryNumFrames - 1));

        Image indexImage = filmStripIndexImage(rotary, rotaryIsHorizontal, rotaryFrameWidth, rotaryFrameHeight, frameIndex);

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
    if (useLinearVerticalSliderImage && (sliderStyle == Slider::LinearVertical))
    {
        g.setOpacity(1.0f);
        g.drawImageAt(linearVerticalThumb,
                      (int)(x + (width / 2.0f) - (linearVerticalThumb.getWidth() / 2.0f)),
                      (int)(sliderPos - (linearVerticalThumb.getHeight() / 2.0f)));
    }
    else if (useLinearHorizontalSliderImage && (sliderStyle == Slider::LinearHorizontal))
    {
        g.setOpacity(1.0f);
        g.drawImageAt(linearHorizontalThumb,
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
    if (useLinearVerticalSliderImage && (slider.getSliderStyle() == Slider::LinearVertical))
        return jmax(linearVerticalThumb.getWidth() / 2, linearVerticalThumb.getHeight() / 2) + linearVerticalThumbBorder;
    else if (useLinearHorizontalSliderImage && (slider.getSliderStyle() == Slider::LinearHorizontal))
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
    if (style == Slider::LinearVertical && linearVerticalBackgroundNumFrames > 0)
    {
        int frameIndex = (int)((slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum()) * (linearVerticalBackgroundNumFrames - 1));

        Image indexImage = filmStripIndexImage(linearVerticalBackground, linearVerticalBackgroundIsHorizontal, linearVerticalBackgroundFrameWidth, linearVerticalBackgroundFrameHeight, frameIndex);
        g.setOpacity(1.0f);
        g.drawImageAt(indexImage, (int)(x + (width / 2.0f) - (indexImage.getWidth() / 2.0f)), y);
    }
    else if (style == Slider::LinearHorizontal && linearHorizontalBackgroundNumFrames > 0)
    {
        int frameIndex = (int)((slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum()) * (linearHorizontalBackgroundNumFrames - 1));

        Image indexImage = filmStripIndexImage(linearHorizontalBackground, linearHorizontalBackgroundIsHorizontal, linearHorizontalBackgroundFrameWidth, linearHorizontalBackgroundFrameHeight, frameIndex);

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

    if (useTextButtonImage)
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

void BCMLookAndFeel::setRotarySliderImage()
{
    if (rotaryFileName.isNotEmpty())
    {
        rotary = ImageLoader::getInstance()->loadImage(rotaryFileName, useImageCache, configurationFileDirectoryPath);

        if (rotary.isValid())
        {
            if (rotaryIsHorizontal)
            {
                rotaryFrameHeight = rotary.getHeight();
                rotaryFrameWidth  = rotary.getWidth() / rotaryNumFrames;
            }
            else
            {
                rotaryFrameHeight = rotary.getHeight() / rotaryNumFrames;
                rotaryFrameWidth  = rotary.getWidth();
            }
        }
        else
            rotaryNumFrames = 0;
    }
    else
        rotaryNumFrames = 0;
};

void BCMLookAndFeel::setLinearSliderImages()
{
    useLinearVerticalSliderImage = true;
    useLinearHorizontalSliderImage = true;

    // Set up Image and values for Vertical Slider Thumbs
    if (linearVerticalThumbFileName.isNotEmpty())
        linearVerticalThumb = ImageLoader::getInstance()->loadImage(linearVerticalThumbFileName, useImageCache, configurationFileDirectoryPath);
    else
        useLinearVerticalSliderImage = false;

    // Set up Image and values for Horizontal Slider Thumbs
    if (linearHorizontalThumbFileName.isNotEmpty())
        linearHorizontalThumb = ImageLoader::getInstance()->loadImage(linearHorizontalThumbFileName, useImageCache, configurationFileDirectoryPath);
    else
        useLinearHorizontalSliderImage = false;
    
    // Set up Image and values for Vertical Slider Backgrounds
    if (linearVerticalBackgroundFileName.isNotEmpty())
    {
        linearVerticalBackground = ImageLoader::getInstance()->loadImage(linearVerticalBackgroundFileName, useImageCache, configurationFileDirectoryPath);

        if (linearVerticalBackground.isValid())
        {
            if (linearVerticalBackgroundIsHorizontal)
            {
                linearVerticalBackgroundFrameHeight = linearVerticalBackground.getHeight();
                linearVerticalBackgroundFrameWidth = linearVerticalBackground.getWidth() / linearVerticalBackgroundNumFrames;
            }
            else
            {
                linearVerticalBackgroundFrameHeight = linearVerticalBackground.getHeight() / linearVerticalBackgroundNumFrames;
                linearVerticalBackgroundFrameWidth = linearVerticalBackground.getWidth();
            }
        }
        else
            linearVerticalBackgroundNumFrames = 0;
    }
    else
        linearVerticalBackgroundNumFrames = 0;
    
    // Set up Image and values for Horizontal Slider Backgrounds
    if (linearHorizontalBackgroundFileName.isNotEmpty())
    {
        linearHorizontalBackground = ImageLoader::getInstance()->loadImage(linearHorizontalBackgroundFileName, useImageCache, configurationFileDirectoryPath);

        if (linearHorizontalBackground.isValid())
        {
            if (linearHorizontalBackgroundIsHorizontal)
            {
                linearHorizontalBackgroundFrameHeight = linearHorizontalBackground.getHeight();
                linearHorizontalBackgroundFrameWidth  = linearHorizontalBackground.getWidth() / linearHorizontalBackgroundNumFrames;
            }
            else
            {
                linearHorizontalBackgroundFrameHeight = linearHorizontalBackground.getHeight() / linearHorizontalBackgroundNumFrames;
                linearHorizontalBackgroundFrameWidth  = linearHorizontalBackground.getWidth();
            }
        }
        else
            linearHorizontalBackgroundNumFrames = 0;
    }
    else
        linearHorizontalBackgroundNumFrames = 0;
};

void BCMLookAndFeel::setTextButtonImages()
{
    // Assume that because this has been called that filenames have been supplied.
    // However, if any of them are empty, give up on trying to display images for TextButtons
    useTextButtonImage = true;

    if (textButtonUpFileName.isNotEmpty())
        textButtonUp = ImageLoader::getInstance()->loadImage(textButtonUpFileName, useImageCache, configurationFileDirectoryPath);
    else
        useTextButtonImage = false;

    if (textButtonDownFileName.isNotEmpty())
        textButtonDown = ImageLoader::getInstance()->loadImage(textButtonDownFileName, useImageCache, configurationFileDirectoryPath);
    else
        useTextButtonImage = false;

    if (textButtonOverUpFileName.isNotEmpty())
        textButtonOverUp = ImageLoader::getInstance()->loadImage(textButtonOverUpFileName, useImageCache, configurationFileDirectoryPath);
    else
        useTextButtonImage = false;

    if (textButtonOverDownFileName.isNotEmpty())
        textButtonOverDown = ImageLoader::getInstance()->loadImage(textButtonOverDownFileName, useImageCache, configurationFileDirectoryPath);
    else
        useTextButtonImage = false;
};

Image BCMLookAndFeel::filmStripIndexImage
(
    Image& filmStripImage,
    bool   isHorizontal,
    int    frameWidth,
    int    frameHeight,
    int    frameIndex
)
{
    int srcX;
    int srcY;

    if (! isHorizontal)
    {
        srcX      = 0;
        srcY      = frameIndex * frameHeight;
    }
    else
    {
        srcX      = frameIndex * frameWidth;
        srcY      = 0;
    }

    return filmStripImage.getClippedImage(Rectangle<int>(srcX, srcY, frameWidth, frameHeight));
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
