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

    virtual bool mightContainSubItems() override;
    Font         getFont() const override;
    virtual Icon getIcon() const override;
    float        getIconSize() const override;
    virtual var  getDragSourceDescription() override;
    virtual bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    virtual void changePanel() override;
    
    virtual void copyItem() override {}
    virtual void pasteItem() override {}
    virtual bool canPasteItem() override { return false; }

    virtual void deleteItem() override {}
    virtual void addItem() override {}
    virtual void addItemFromClipboard() override {}
    
    virtual void showPopupMenu() override {};
    
    virtual String getDisplayName() const override;
    
protected:
    ConfigurationManager& configurationManager;
    Configuration&        configuration;

private:
    virtual void refreshSubItems() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationItem)
};

#endif  // CONFIGURATIONTREEITEM_H_INCLUDED
