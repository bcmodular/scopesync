/*
 * Miscellaneous utility classes & methods
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

#include "BCMMisc.h"

String createAlphaNumericUID()
{
    String uid;
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    Random r;

    uid << chars [r.nextInt (52)]; // make sure the first character is always a letter

    for (int i = 5; --i >= 0;)
    {
        r.setSeedRandomly();
        uid << chars [r.nextInt (62)];
    }

    return uid;
}

/* =========================================================================
 * PropertyListBuilder
 */
void PropertyListBuilder::add(PropertyComponent* propertyComp)
{
    components.add(propertyComp);
}

void PropertyListBuilder::add(PropertyComponent* propertyComp, const String& tooltip)
{
    propertyComp->setTooltip (tooltip);
    add(propertyComp);
}

void PropertyListBuilder::clear()
{
    components.clear();
}
