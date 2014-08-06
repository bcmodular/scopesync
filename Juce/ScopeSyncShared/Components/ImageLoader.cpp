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

#include "ImageLoader.h"

juce_ImplementSingleton (ImageLoader)

ImageLoader::ImageLoader()
{
    loadImageResources();
}

ImageLoader::~ImageLoader()
{
    clearSingletonInstance();
}

void ImageLoader::loadImageResources()
{
    ImageResource* imageResource;
            
    imageResource = new ImageResource();
    imageResource->imageName = "scopeSync_logo_png";
    imageResource->image     = (const char*)resource_scopeSync_logo_png;
    imageResource->imageSize = 4151;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "loadConfigButton_off_png";
    imageResource->image     = (const char*)resource_loadConfigButton_off_png;
    imageResource->imageSize = 2901;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "loadConfigButton_on_png";
    imageResource->image     = (const char*)resource_loadConfigButton_on_png;
    imageResource->imageSize = 2904;
    imageResources.add(imageResource);
}

Image ImageLoader::loadImage(const String& imageFileName, bool useImageCache, const String& directoryPath)
{
    for (int i = 0; i < imageResources.size(); i++)
    {
        if (imageResources[i]->imageName.equalsIgnoreCase(imageFileName))
        {
            if (useImageCache)
                return ImageCache::getFromMemory(imageResources[i]->image, imageResources[i]->imageSize);
            else
                return ImageFileFormat::loadFrom(imageResources[i]->image, imageResources[i]->imageSize);
        }
    }

    File directory(directoryPath);
    File imageFile = directory.getChildFile(imageFileName);
    
    if (useImageCache)
        return ImageCache::getFromFile(imageFile);
    else
        return ImageFileFormat::loadFrom(imageFile);
}
