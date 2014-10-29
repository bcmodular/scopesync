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

#include <JuceHeader.h>
class ScopeSync;
class BCMComponent;
class BCMTabbedComponent;
class ComponentProperties;
class SliderProperties;
class LabelProperties;
class TextButtonProperties;
class ComboBoxProperties;
class TabbedComponentProperties;
class TabProperties;
class BCMParameter;
class ConfigurationChooserWindow;
class NewConfigurationWindow;
#include "../Components/BCMSlider.h"
#include "../Utils/BCMMisc.h"

class ScopeSyncGUI : public Component,
                     public Timer,
                     public ApplicationCommandTarget,
                     public ChangeListener
{
public:
    /* ========================== Initialisation ============================= */
    ScopeSyncGUI (ScopeSync& owner);
    ~ScopeSyncGUI();

    /* ========================== Public Actions ============================= */
    void          hideUserSettings();
    void          showUserSettings();
    void          chooseConfiguration();
    BCMParameter* getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping);
    void          addTabbedComponent(BCMTabbedComponent* tabbedComponent);
    
    ScopeSync& getScopeSync() const { return scopeSync; };
    void getTabbedComponentsByName(const String& name, Array<BCMTabbedComponent*>& tabbedComponentArray);
    Slider::SliderStyle getDefaultRotarySliderStyle();

    static void deleteTooltipWindow() { tooltipWindow = nullptr; }
    
    /* ====================== Public member variables ========================= */
    ScopedPointer<ComponentProperties>       defaultComponentProperties;
    ScopedPointer<SliderProperties>          defaultSliderProperties;
    ScopedPointer<LabelProperties>           defaultLabelProperties;
    ScopedPointer<TextButtonProperties>      defaultTextButtonProperties;
    ScopedPointer<ComboBoxProperties>        defaultComboBoxProperties;
    ScopedPointer<TabbedComponentProperties> defaultTabbedComponentProperties;
    ScopedPointer<TabProperties>             defaultTabProperties;

    class Settings
    {
    public:
        BCMSlider::EncoderSnap       encoderSnap;
        BCMSlider::RotaryMovement    rotaryMovement;
        Slider::IncDecButtonMode     incDecButtonMode;
        BCMSlider::PopupEnabled      popupEnabled;
        BCMSlider::VelocityBasedMode velocityBasedMode;
        bool enableTooltips;
        int  tooltipDelayTime;
    };
    
    Settings settings;

private:
    /* ===================== Private member variables ========================= */
    ScopedPointer<BCMComponent> mainComponent;
    Array<BCMTabbedComponent*>  tabbedComponents;
    ScopedPointer<Label>        systemError;
    ScopedPointer<ConfigurationChooserWindow> configurationChooserWindow;
    ScopedPointer<NewConfigurationWindow>     addConfigurationWindow;
    
    ScopeSync& scopeSync;
    ValueTree  deviceMapping;
    static ScopedPointer<TooltipWindow> tooltipWindow;
    
    static const int timerFrequency;

    /* =================== Private Configuration Methods =======================*/
    void createGUI(bool forceReload);
    void setupLookAndFeels(XmlElement& lookAndFeelsXML, bool useImageCache);
    void setupLookAndFeel(XmlElement& lookAndFeelXML, bool useImageCache);
    void setupDefaults(XmlElement& defaultsXML);
    void readSettingsXml(XmlElement& defaultsXML);
    void createComponent(XmlElement& componentXML);
    void timerCallback();
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    
    static void alertBoxReloadConfirm(int result, ScopeSyncGUI*);
    static void alertBoxLaunchLocationEditor(int result, ScopeSyncGUI*);

    void changeListenerCallback(ChangeBroadcaster* source);

    void addConfig();
    void save();
    void saveAs();
    void checkNewConfigIsInLocation();
    void undo();
    void redo();
    void snapshot();
    void showConfigurationManager();
    void reloadSavedConfiguration();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeSyncGUI)
};

#endif  // SCOPESYNCGUI_H_INCLUDED
