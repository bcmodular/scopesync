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
 * the Free Software Foundation, either version 3 of the License, or
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
#include "../Components/BCMLookAndFeel.h"
class ScopeSync;
class Configuration;
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
#include "../Resources/ImageLoader.h"
#include "../Windows/UserSettings.h"

class AboutBoxWindow : public DocumentWindow
{
public:
    AboutBoxWindow();
    
    void closeButtonPressed() override;
    
private:

    class AboutBox : public Component
    {
    public:
        AboutBox();

        void paint(Graphics& g) override;
        void resized() override;

    private:
        Label           scopeSyncVersion;
        Label           moduleVersion;
        Label           credits;
        HyperlinkButton scopeSyncLink;

		SharedResourcePointer<ImageLoader> imageLoader;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutBox)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutBoxWindow)
};

class ScopeSyncGUI : public Component,
                     public Timer,
                     public ApplicationCommandTarget
{
public:
    /* ========================== Initialisation ============================= */
    ScopeSyncGUI (ScopeSync& owner);
    ~ScopeSyncGUI();

    /* ========================== Public Actions ============================= */
    void          hideUserSettings();
    void          showUserSettings() const;
    void          chooseConfiguration();
    BCMParameter* getUIMapping(Identifier componentTypeId, const String& componentName, ValueTree& mapping) const;
    void          addTabbedComponent(BCMTabbedComponent* tabbedComponent);
    
    ScopeSync& getScopeSync() const { return scopeSync; };
    void getTabbedComponentsByName(const String& name, Array<BCMTabbedComponent*>& tabbedComponentArray) const;
    Slider::SliderStyle getDefaultRotarySliderStyle() const;
    
    void hideConfigurationChooserWindow();
    
    /* ====================== Public member variables ========================= */
    ScopedPointer<ComponentProperties>       defaultComponentProperties;
    ScopedPointer<SliderProperties>          defaultSliderProperties;
    ScopedPointer<LabelProperties>           defaultLabelProperties;
    ScopedPointer<TextButtonProperties>      defaultTextButtonProperties;
    ScopedPointer<ComboBoxProperties>        defaultComboBoxProperties;
    ScopedPointer<TabbedComponentProperties> defaultTabbedComponentProperties;
    ScopedPointer<TabProperties>             defaultTabProperties;

    OwnedArray<ComponentProperties>          componentTemplates;
    OwnedArray<SliderProperties>             sliderTemplates;
    OwnedArray<LabelProperties>              labelTemplates;
    OwnedArray<TextButtonProperties>         textButtonTemplates;
    OwnedArray<ComboBoxProperties>           comboBoxTemplates;
    OwnedArray<TabbedComponentProperties>    tabbedComponentTemplates;
    
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
    
    Settings settings{};

private:
    
    /* ===================== Private member variables ========================= */
    ScopedPointer<BCMComponent> mainComponent;
    Array<BCMTabbedComponent*>  tabbedComponents;
    ScopedPointer<Label>        systemError;
    ScopedPointer<ConfigurationChooserWindow> configurationChooserWindow;
    
    ScopeSync& scopeSync;
    ValueTree  deviceMapping;
    SharedResourcePointer<TooltipWindow>  tooltipWindow;
    SharedResourcePointer<BCMDefaultLookAndFeel> defaultBCMLookAndFeel;
	SharedResourcePointer<UserSettings> userSettings;
	SharedResourcePointer<AboutBoxWindow> aboutBox;

    static const int timerFrequency;

    /* =================== Private Configuration Methods =======================*/
    void createGUI(bool forceReload);
    void setupLookAndFeels(XmlElement& lookAndFeelsXML); 
    void setupStandardLookAndFeels(XmlElement& xml);
    void setupLookAndFeel(XmlElement& lookAndFeelXML) const;
    void setupDefaults(XmlElement& defaultsXML);
    void clearWidgetTemplates();
    void setupWidgetTemplates(XmlElement& widgetTemplatesXML);
    void readSettingsXml(XmlElement& settingsXML);
    void createComponent(XmlElement& componentXML);
    void timerCallback() override;
    
    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    
    static void alertBoxReloadConfirm(int result, ScopeSyncGUI*);

    void addConfig() const;
    void save() const;
    void saveAs() const;
    void undo() const;
    void redo() const;
    void snapshot() const;
    void showConfigurationManager() const;
    void reloadSavedConfiguration();
    void showAboutBox() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeSyncGUI)
};

#endif  // SCOPESYNCGUI_H_INCLUDED
