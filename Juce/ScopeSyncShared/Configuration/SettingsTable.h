/*
  ==============================================================================

    SettingsTable.h
    Created: 25 Sep 2014 10:51:31am
    Author:  giles

  ==============================================================================
*/

#ifndef SETTINGSTABLE_H_INCLUDED
#define SETTINGSTABLE_H_INCLUDED
#include <JuceHeader.h>
#include "ConfigurationManagerMain.h"
#include "../Core/Global.h"

/* =========================================================================
 * SettingsTable: TableListBox for managing Parameter Settings
 */
class SettingsTable : public  Component,
                      private TableListBoxModel,
                      private ValueTree::Listener,
                      public  ApplicationCommandTarget
{
public:
    SettingsTable(const ValueTree& vt, UndoManager& um, ConfigurationManagerMain& cmm,
                  ValueTree& parameter);
    ~SettingsTable();

    void       resized() override;
    
    int        getNumRows();
    void       paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void       paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
    void       sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void       selectedRowsChanged(int lastRowSelected) override;
    void       backgroundClicked(const MouseEvent&) override;
    void       deleteKeyPressed(int) override;

    // Overridden methods for ValueTree::Listener
    void valueTreePropertyChanged(ValueTree& /* treeWhosePropertyHasChanged */, const Identifier& /* property */) override { table.updateContent(); };
    void valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) override { table.updateContent(); };
    void valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved */) override { table.updateContent(); };
    void valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */) override { table.updateContent(); };
    void valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) override { table.updateContent(); };

private:
    TableListBox table;
    Font         font;
    ValueTree    tree;
    UndoManager& undoManager;
    ConfigurationManagerMain& configurationManagerMain;
    Label        numSettingsTextLabel;
    Label        numSettingsToAddLabel;
    TextButton   addSettingsButton;
    TextButton   removeSettingsButton;
    TextButton   autoFillValuesButton;
    TextButton   moveUpButton;
    TextButton   moveDownButton;

    Value        numSettingsToAdd;
    ValueTree    parameter;

    class LabelComp;
    friend class LabelComp;

    void textWasEdited();
    void addSettings();
    void removeSettings();
    void autoFill();
    void moveSettings(bool moveUp);

    /* ================= Application Command Target overrides ================= */
    void getAllCommands(Array<CommandID>& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;
    ApplicationCommandTarget* getNextCommandTarget();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsTable)
};

//==============================================================================
// A comparator used to sort our data when the user clicks a column header
class SettingsSorter
{
public:
    SettingsSorter (const int columnIdToSort, bool forwards)
        : columnId(columnIdToSort),
          direction (forwards ? 1 : -1)
    {
    }

    int compareElements(const ValueTree& first, const ValueTree& second) const
    {
        int result = 0;

        if (columnId == 2)
        {
            String firstString  = first.getProperty(Ids::name, String::empty);
            String secondString = second.getProperty(Ids::name, String::empty);
            result = firstString.compareNatural(secondString);
        }
        else if (columnId == 3)
        {
            int firstInt  = first.getProperty(Ids::intValue);
            int secondInt = second.getProperty(Ids::intValue);
            result = (firstInt < secondInt) ? -1 : ((secondInt < firstInt) ? 1 : 0);
        }
        
        return direction * result;
    }

private:
    int columnId;
    int direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsSorter)
};

#endif  // SETTINGSTABLE_H_INCLUDED
