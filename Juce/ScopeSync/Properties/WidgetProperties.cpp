/**
 * Base class for extracting BCMWidget definitions from
 * XML layout files
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

#include "WidgetProperties.h"
#include "PropertiesHelper.h"

WidgetProperties::WidgetProperties()
{
    initialiseWidget();
}

WidgetProperties::WidgetProperties(XmlElement& widgetXML)
{
    initialiseWidget();
    setWidgetValuesFromXML(widgetXML);
}

WidgetProperties::WidgetProperties(XmlElement& widgetXML, WidgetProperties& parentWidgetProperties)
{
    copyWidgetProperties(parentWidgetProperties);
    setWidgetValuesFromXML(widgetXML);
}

WidgetProperties::~WidgetProperties() {}

void WidgetProperties::initialiseWidget()
{
    // Ultimate fall-back defaults, in case no defaults supplied in the XML
    name                 = String::empty;
    id                   = String::empty;
    bounds.width         = 100;
    bounds.height        = 100;
    bounds.x             = 0;
    bounds.y             = 0;
    bcmLookAndFeelId     = String::empty;
    widgetTemplateId     = String::empty;
    mappingParentType    = Identifier();
    mappingParent        = String::empty;
    noStyleOverride      = false;
};

void WidgetProperties::copyWidgetProperties(const WidgetProperties& parentWidgetProperties)
{
    name              = parentWidgetProperties.name;
    id                = parentWidgetProperties.id;
    bounds.x          = parentWidgetProperties.bounds.x;
    bounds.y          = parentWidgetProperties.bounds.y;
    bounds.width      = parentWidgetProperties.bounds.width;
    bounds.height     = parentWidgetProperties.bounds.height;
    bcmLookAndFeelId  = parentWidgetProperties.bcmLookAndFeelId;
    widgetTemplateId  = parentWidgetProperties.widgetTemplateId;
    mappingParentType = parentWidgetProperties.mappingParentType;
    mappingParent     = parentWidgetProperties.mappingParent;
    noStyleOverride   = parentWidgetProperties.noStyleOverride;
};

void WidgetProperties::setWidgetValuesFromXML(const XmlElement& widgetXML)
{
    name = widgetXML.getStringAttribute("name", name);
    id   = widgetXML.getStringAttribute("id",   name); // Default to name if no id set
    
    XmlElement* boundsXml = widgetXML.getChildByName("bounds");
    if (boundsXml != nullptr)
        getBoundsFromXml(*boundsXml, bounds);
    
    XmlElement* mappingParentXml = widgetXML.getChildByName("mappingparent");
    if (mappingParentXml != nullptr)
        getMappingParentFromXml(*mappingParentXml, mappingParentType, mappingParent);

    noStyleOverride  = widgetXML.getBoolAttribute("nostyleoverride", noStyleOverride);
    bcmLookAndFeelId = widgetXML.getStringAttribute("lfid", bcmLookAndFeelId);
    widgetTemplateId = widgetXML.getStringAttribute("wtid", widgetTemplateId);
};
