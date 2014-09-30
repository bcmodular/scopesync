/**
 * The Configuration Manager's TreeView
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

#ifndef CONFIGURATIONTREE_H_INCLUDED
#define CONFIGURATIONTREE_H_INCLUDED

#include <JuceHeader.h>
class Configuration;
class ConfigurationItem;
class ConfigurationManagerMain;

class ConfigurationTree : public  Component,
                          public  DragAndDropContainer
{
public:
    ConfigurationTree(ConfigurationManagerMain& cmm, Configuration& config, UndoManager& um);
    ~ConfigurationTree();

    void paint (Graphics& g) override;
    void resized() override;
    void saveTreeViewState();

    void copyItem();
    void pasteItem();
    void addItem();
    void deleteSelectedItems();
    void changePanel();

    void storeSelectedItem(int row);
    void moveToSelectedItem();

    enum ColourIds
    {
        mainBackgroundColourId = 0x2340000,
    };    

private:
    TreeView       tree;
    ScopedPointer<ConfigurationItem> rootItem;
    UndoManager&   undoManager;
    Configuration& configuration;
    ConfigurationManagerMain& configurationManagerMain;

    int storedRow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationTree);
};



#endif  // CONFIGURATIONTREE_H_INCLUDED
