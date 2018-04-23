/**
 * Base class for Component classes that can map to a BCMParameter
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

#ifndef BCMPARAMETERWIDGET_H_INCLUDED
#define BCMPARAMETERWIDGET_H_INCLUDED

#include <JuceHeader.h>
#include "../Components/BCMComponentBounds.h"
#include "../Core/Clipboard.h"

class ScopeSyncGUI;
class ScopeSync;
class WidgetProperties;

class BCMWidget : public ChangeListener,
                  public ApplicationCommandTarget
{
public:
    BCMWidget(ScopeSyncGUI& owner);
    ~BCMWidget();

    void setParentWidget(Component* parent);
    const String& getLookAndFeelId() const { return bcmLookAndFeelId; }

protected:
    ScopeSyncGUI&      scopeSyncGUI;
    ScopeSync&         scopeSync;
    ApplicationCommandManager* commandManager; // ScopeSync's ApplicationCommandManager
    UndoManager&       undoManager;
    BCMComponentBounds componentBounds; // Position/Size information
    Component*         parentWidget;
    ValueTree          styleOverride;
    String             bcmLookAndFeelId;
    String             widgetTemplateId;
	
	virtual const Identifier getComponentType() const = 0;

    virtual void applyLookAndFeel(bool noStyleOverride);
    void applyWidgetProperties(WidgetProperties& properties);

    virtual void overrideStyle();
    void clearStyleOverride();
    void copyStyleOverride() const;
    void copyStyleOverrideToAll() const;
    void pasteStyleOverride();
    bool canPasteStyleOverride() const;

    virtual void showPopupMenu();

    /* ================= Application Command Target overrides ================= */
	void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
	ApplicationCommandTarget* getNextCommandTarget() override;

	void changeListenerCallback (ChangeBroadcaster* source) override;
	void applyBounds() const;

private:
	SharedResourcePointer<StyleOverrideClipboard> styleOverrideClipboard;
	
	bool debugWidgetPosition;
    
};

class BCMParameter;

class BCMParameterWidget : public BCMWidget
{
public:
    BCMParameterWidget(ScopeSyncGUI& owner);
    ~BCMParameterWidget() {};

    // Indicates whether the widget has a parameter mapping
    bool hasParameter() const { return mapsToParameter; };
    
    // Returns the parameter the widget is mapped to
    BCMParameter* getParameter() const { return parameter.get(); };

protected:
    bool          mapsToParameter;         // Flag for whether the widget maps to a parameter
    WeakReference<BCMParameter> parameter; // Pointer to a mapped parameter

    Identifier  mappingComponentType;
    String      mappingComponentName;
    ValueTree   mapping;

	void setParameter(BCMParameter* param);
    void setupMapping(const Identifier& componentType,     const String& componentName,
                      const Identifier& mappingParentType, const String& mappingParent);

    void showPopupMenu() override;

private:
    SharedResourcePointer<ParameterClipboard> parameterClipboard;

    void deleteMapping();
    void editMapping();
    void editMappedParameter();
    void deleteMappedParameter();
    void copyParameter();
    void pasteParameter();
	bool canPasteParameter() const;
    void addParameter(bool fromClipboard) const;
	void copyOSCPath();

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMParameterWidget);
};

#endif  // BCMPARAMETERWIDGET_H_INCLUDED
