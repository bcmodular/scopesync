/**
 * GUI controller for the ScopeSync system. Hosted by different parent
 * objects depending on the AppContext (PluginGUI or ScopeFXGUI
 * respectively).
 *
 * Responsible for creating the top-level main Component for the
 * ScopeSync GUI, as well as owning parameter mapping and a set
 * of pointers to BCMTabbedComponents allowing tabs to be
 * changed by buttons at any level of the Component hierarchy
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

#ifndef SCOPESYNCGUI_H_INCLUDED
#define SCOPESYNCGUI_H_INCLUDED

class BCMComponent;
class BCMTabbedComponent;

#include <JuceHeader.h>
#include "../Components/BCMLookAndFeel.h"
#include "../Components/BCMComponent.h"
#include "../Components/BCMTabbedComponent.h"
#include "ScopeSync.h"
#include "BCMParameter.h"
#include "../Properties/ComponentProperties.h"
#include "../Properties/SliderProperties.h"
#include "../Properties/LabelProperties.h"
#include "../Properties/TextButtonProperties.h"
#include "../Properties/ComboBoxProperties.h"
#include "../Properties/TabbedComponentProperties.h"
#include "../Properties/TabProperties.h"

class ScopeSyncGUI : public Component, public Timer                
{
public:
    /* ========================== Initialisation ============================= */
    ScopeSyncGUI (ScopeSync& owner);
    ~ScopeSyncGUI();

    /* ========================== Public Actions ============================= */
    void          showUserSettings();
    void          chooseConfiguration();
    BCMParameter* getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping);
    void          addTabbedComponent(BCMTabbedComponent* tabbedComponent);
   
    ScopeSync& getScopeSync() const { return scopeSync; };
    void getTabbedComponentsByName(const String& name, Array<BCMTabbedComponent*>& tabbedComponentArray);
 
    /* =========================== Identifiers =============================== */
    static const Identifier mappingId;
    static const Identifier mappingParamId;
    static const Identifier mappingParamNameId;
    static const Identifier mappingMapToId;
    static const Identifier mappingTypeId;
    static const Identifier mappingDisplayTypeId;
    static const Identifier mappingCustomDisplayId;
    static const Identifier mappingSettingDownId;
    static const Identifier mappingSettingUpId;
    static const Identifier mappingComponentNameId;
    static const Identifier mappingSliderId;
    static const Identifier mappingLabelId;
    static const Identifier mappingComboBoxId;
    static const Identifier mappingTextButtonId;
    static const Identifier mappingRadioGroupId;

    /* ====================== Public member variables ========================= */
    ScopedPointer<ComponentProperties>       defaultComponentProperties;
    ScopedPointer<SliderProperties>          defaultSliderProperties;
    ScopedPointer<LabelProperties>           defaultLabelProperties;
    ScopedPointer<TextButtonProperties>      defaultTextButtonProperties;
    ScopedPointer<ComboBoxProperties>        defaultComboBoxProperties;
    ScopedPointer<TabbedComponentProperties> defaultTabbedComponentProperties;
    ScopedPointer<TabProperties>             defaultTabProperties;

    static const char* scopeSync_logo_png;
    static const int   scopeSync_logo_pngSize;
    static const char* loadConfigButton_off_png;
    static const int   loadConfigButton_off_pngSize;
    static const char* loadConfigButton_on_png;
    static const int   loadConfigButton_on_pngSize;
    
private:
    /* =================== Private Configuration Methods =======================*/
    void createGUI();
    void loadMapping();
    void setupLookAndFeels(XmlElement& lookAndFeelsXML, bool useImageCache);
    void setupLookAndFeel(XmlElement& lookAndFeelXML, bool useImageCache);
    void setupDefaults(XmlElement& defaultsXML);
    void createComponent(XmlElement& componentXML);
    void timerCallback();
    void showSystemError();

    /* ===================== Private member variables ========================= */
    ScopedPointer<BCMComponent> mainComponent;
    Array<BCMTabbedComponent*>  tabbedComponents;
    ScopedPointer<Label>        systemError;
    
    ScopeSync&    scopeSync;
    ValueTree     deviceMapping;
    TooltipWindow tooltipWindow;
    
    static const int timerFrequency;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeSyncGUI)
};

#define BCM_SET_BOUNDS \
    if (componentBounds.boundsType == BCMComponentBounds::relativeRectangle) \
        setBounds(componentBounds.relativeRectangleString); \
    else if (componentBounds.boundsType == BCMComponentBounds::inset) \
        setBoundsInset(componentBounds.borderSize); \
    else \
    { \
        setBounds( \
            componentBounds.x, \
            componentBounds.y, \
            componentBounds.width, \
            componentBounds.height \
        ); \
    } \

#endif  // SCOPESYNCGUI_H_INCLUDED
