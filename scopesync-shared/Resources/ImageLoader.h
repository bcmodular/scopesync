/**
 * Image Loader utility class. Handles loading from file, or via
 * Image Cache. Also owns shared in-memory Image resources.
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

#ifndef IMAGELOADER_H_INCLUDED
#define IMAGELOADER_H_INCLUDED

#include <JuceHeader.h>
#include "../Windows/UserSettings.h"

class BCMLookAndFeel;

class ImageLoader
{
public:
    ImageLoader();
    ~ImageLoader();

    struct ImageResource {
        String      imageName;
        const char* image;
        int         imageSize;
    };
    
    // Load image, either direct from file, via Image Cache, or from resources
    Image loadImage(const String& imageFileName, const String& directoryPath) const;
    
private:
    // Initialise image resources
    void loadImageResources();
    void addImageResource(String imageName, const char* image, int imageSize);

    OwnedArray<ImageResource> imageResources;
	SharedResourcePointer<UserSettings> userSettings;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageLoader)
};

#endif  // IMAGELOADER_H_INCLUDED