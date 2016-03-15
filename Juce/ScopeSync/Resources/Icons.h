/**
 * Classes relating to vector-based Icons for use in the ScopeSync
 * UIs
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

#ifndef ICONS_H_INCLUDED
#define ICONS_H_INCLUDED

#include <JuceHeader.h>

struct Icon
{
    Icon() : path(nullptr) {}
    Icon(const Path& p, Colour c)  : path(&p), colour(c) {}
    Icon(const Path* p, Colour c)  : path(p),  colour(c) {}

    void draw(Graphics& g, const juce::Rectangle<float>& area, bool isCrossedOut) const
    {
        if (path != nullptr)
        {
            g.setColour(colour);

            const RectanglePlacement placement(RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize);
            g.fillPath (*path, placement.getTransformToFit(path->getBounds(), area));

            if (isCrossedOut)
            {
                g.setColour (Colours::red.withAlpha (0.8f));
                g.drawLine (static_cast<float>(area.getX()), area.getY() + area.getHeight() * 0.2f,
                            static_cast<float>(area.getRight()), area.getY() + area.getHeight() * 0.8f, 3.0f);
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
         parameters, parameter, 
         sliders, slider, 
         textbuttons, textbutton, 
         labels, label,
         comboboxes, combobox,
         tabbedcomponents, tabbedcomponent,
         styleoverrides, components, component;

    juce_DeclareSingleton(Icons, false)

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Icons)
};

#endif  // ICONS_H_INCLUDED
