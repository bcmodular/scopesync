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
    imageResource->imageName = "scopeSyncLogo";
    imageResource->image     = (const char*)resourceScopeSyncLogo;
    imageResource->imageSize = 4151;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "loadConfigButtonOff";
    imageResource->image     = (const char*)resourceLoadConfigButtonOff;
    imageResource->imageSize = 2901;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "loadConfigButtonOn";
    imageResource->image     = (const char*)resourceLoadConfigButtonOn;
    imageResource->imageSize = 2904;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "patchWindowButtonOff";
    imageResource->image     = (const char*)resourcePatchWindowButtonOff;
    imageResource->imageSize = 2902;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "patchWindowButtonOn";
    imageResource->image     = (const char*)resourcePatchWindowButtonOn;
    imageResource->imageSize = 2884;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "presetsButtonOff";
    imageResource->image     = (const char*)resourcePresetsButtonOff;
    imageResource->imageSize = 2896;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "presetsButtonOn";
    imageResource->image     = (const char*)resourcePresetsButtonOn;
    imageResource->imageSize = 2895;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "reloadConfigButtonOff";
    imageResource->image     = (const char*)resourceReloadConfigButtonOff;
    imageResource->imageSize = 3107;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "reloadConfigButtonOn";
    imageResource->image     = (const char*)resourceReloadConfigButtonOn;
    imageResource->imageSize = 3143;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "settingsButtonOff";
    imageResource->image     = (const char*)resourceSettingsButtonOff;
    imageResource->imageSize = 3338;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "settingsButtonOn";
    imageResource->image     = (const char*)resourceSettingsButtonOn;
    imageResource->imageSize = 3335;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "snapshotOff";
    imageResource->image     = (const char*)resourceSnapshotOff;
    imageResource->imageSize = 770;
    imageResources.add(imageResource);

    imageResource = new ImageResource();
    imageResource->imageName = "snapshotOn";
    imageResource->image     = (const char*)resourceSnapshotOn;
    imageResource->imageSize = 788;
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

    if (directoryPath.isNotEmpty())
    {
        File directory(directoryPath);
        File imageFile = directory.getChildFile(imageFileName);

        if (useImageCache)
            return ImageCache::getFromFile(imageFile);
        else
            return ImageFileFormat::loadFrom(imageFile);
    }
    else
        return Image();
}
