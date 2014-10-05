/*
  ==============================================================================

    BCMParameterWidget.h
    Created: 5 Oct 2014 10:25:20am
    Author:  giles

  ==============================================================================
*/

#ifndef BCMPARAMETERWIDGET_H_INCLUDED
#define BCMPARAMETERWIDGET_H_INCLUDED

#include <JuceHeader.h>
#include "../Core/BCMParameter.h"

class BCMParameterWidget : public ApplicationCommandTarget
{
public:
    BCMParameterWidget(ApplicationCommandManager* acm);
    ~BCMParameterWidget() {}

    // Indicates whether a BCMSlider has a parameter mapping
    bool hasParameter() { return mapsToParameter; };
    
    // Returns the parameter a BCMSlider is mapped to
    BCMParameter* getParameter() { return parameter.get(); };

protected:
    bool          mapsToParameter;         // Flag for whether the widget maps to a parameter
    WeakReference<BCMParameter> parameter; // Pointer to a mapped parameter

    ValueTree   mapping;

    ApplicationCommandManager* commandManager; // ScopeSync's ApplicationCommandManager

    /* ================= Application Command Target overrides ================= */
    virtual void getAllCommands(Array<CommandID>& commands) override;
    virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    virtual bool perform(const InvocationInfo& info) override;
    virtual ApplicationCommandTarget* getNextCommandTarget();

    virtual void deleteMapping() {};
    virtual void editMapping() {};
    virtual void editMappedParameter() {};
    virtual void showPopup();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BCMParameterWidget);
};



#endif  // BCMPARAMETERWIDGET_H_INCLUDED
