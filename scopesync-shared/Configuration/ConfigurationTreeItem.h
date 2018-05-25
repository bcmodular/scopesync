/**
 * Various TreeViewItem classes that support the Configuration
 * Manager TreeView.
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

#ifndef CONFIGURATIONTREEITEM_H_INCLUDED
#define CONFIGURATIONTREEITEM_H_INCLUDED

#include <JuceHeader.h>
#include "../Utils/BCMTreeView.h"
#include "../Resources/Icons.h"
#include "../Core/Clipboard.h"

class ConfigurationManager;
class Configuration;

/* =========================================================================
 * ConfigurationItem: Base class for Configuration Manager TreeViewItems
 */
class ConfigurationItem : public BCMTreeItem
{
public:
    ConfigurationItem(ConfigurationManager& cm, const ValueTree& v, UndoManager& um);
    ~ConfigurationItem();

    bool mightContainSubItems() override;
    Font         getFont() const override;
    Icon getIcon() const override;
    float        getIconSize() const override;
    var  getDragSourceDescription() override;
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void changePanel() override;
    
    void copyItem() override {}
    void pasteItem() override {}
    bool canPasteItem() override { return false; }

    void deleteItem() override {}
    void addItem() override {}
    void addItemFromClipboard() override {}
    
    void showPopupMenu() override {};
    
    String getDisplayName() const override;
    
protected:
    ConfigurationManager& configurationManager;
    Configuration&        configuration;
	SharedResourcePointer<Icons> icons;

private:
    virtual void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationItem)
};

#endif  // CONFIGURATIONTREEITEM_H_INCLUDED
