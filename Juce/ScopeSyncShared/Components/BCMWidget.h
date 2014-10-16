/**
 * Base class for Component classes that can map to a BCMParameter
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

#ifndef BCMPARAMETERWIDGET_H_INCLUDED
#define BCMPARAMETERWIDGET_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"
class ScopeSyncGUI;
class ScopeSync;
class WidgetProperties;

class BCMWidget
{
public:
    BCMWidget(ScopeSyncGUI& owner, Component* parent);
    ~BCMWidget() {};

    const String& getLookAndFeelId() { return bcmLookAndFeelId; }

protected:
    ScopeSyncGUI&      scopeSyncGUI;
    ScopeSync&         scopeSync;
    BCMComponentBounds componentBounds; // Position/Size information
    Component*         parentBCMComponent;
    ValueTree          styleOverride;
    String             bcmLookAndFeelId;

    virtual const Identifier getComponentType() const = 0;

    void applyWidgetProperties(WidgetProperties& properties);

private:
    void applyBounds();
    void applyLookAndFeel(bool noStyleOverride);
};

class BCMParameter;

class BCMParameterWidget : public BCMWidget,
                           public ApplicationCommandTarget,
                           public ChangeListener
{
public:
    BCMParameterWidget(ScopeSyncGUI& owner, Component* parent);
    ~BCMParameterWidget();

    // Indicates whether the widget has a parameter mapping
    bool hasParameter() { return mapsToParameter; };
    
    // Returns the parameter the widget is mapped to
    BCMParameter* getParameter() { return parameter.get(); };

protected:
    bool          mapsToParameter;         // Flag for whether the widget maps to a parameter
    WeakReference<BCMParameter> parameter; // Pointer to a mapped parameter

    Identifier  mappingComponentType;
    String      mappingComponentName;
    ValueTree   mapping;

    ApplicationCommandManager* commandManager; // ScopeSync's ApplicationCommandManager
   
    void saveAs();
    void undo();
    void redo();

    void showPopupMenu();
    void setupMapping(const Identifier& componentType,     const String& componentName,
                      const Identifier& mappingParentType, const String& mappingParent);
    
private:
    /* ================= Application Command Target overrides ================= */
    virtual void getAllCommands(Array<CommandID>& commands) override;
    virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    virtual bool perform(const InvocationInfo& info) override;
    virtual ApplicationCommandTarget* getNextCommandTarget();

    void deleteMapping();
    void editMapping();
    void editMappedParameter();
    void overrideStyle();
    void clearStyleOverride();
    
    void changeListenerCallback (ChangeBroadcaster* source);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMParameterWidget);
};



#endif  // BCMPARAMETERWIDGET_H_INCLUDED
