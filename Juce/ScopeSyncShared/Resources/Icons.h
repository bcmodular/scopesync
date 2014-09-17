/*
  ==============================================================================

    Icons.h
    Created: 15 Sep 2014 10:24:00pm
    Author:  giles

  ==============================================================================
*/

#ifndef ICONS_H_INCLUDED
#define ICONS_H_INCLUDED

#include <JuceHeader.h>

struct Icon
{
    Icon() : path(nullptr) {}
    Icon(const Path& p, Colour c)  : path(&p), colour(c) {}
    Icon(const Path* p, Colour c)  : path(p),  colour(c) {}

    void draw(Graphics& g, const Rectangle<float>& area, bool isCrossedOut) const
    {
        if (path != nullptr)
        {
            g.setColour(colour);

            const RectanglePlacement placement(RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize);
            g.fillPath (*path, placement.getTransformToFit(path->getBounds(), area));

            if (isCrossedOut)
            {
                g.setColour (Colours::red.withAlpha (0.8f));
                g.drawLine ((float)area.getX(), area.getY() + area.getHeight() * 0.2f,
                            (float)area.getRight(), area.getY() + area.getHeight() * 0.8f, 3.0f);
            }
        }
    }

    Icon withContrastingColourTo (Colour background) const
    {
        return Icon (path, background.contrasting(colour, 0.6f));
    }

    const Path* path;
    Colour colour;
};

class Icons
{
public:
    Icons();
    ~Icons();

    Path config, mapping, 
         hostparameters, hostparameter, 
         scopeparameters, scopeparameter,
         sliders, slider, 
         textbuttons, textbutton, 
         labels, label,
         comboboxes, combobox,
         tabbedcomponents, tabbedcomponent;

    juce_DeclareSingleton(Icons, false)

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Icons)
};

#endif  // ICONS_H_INCLUDED
