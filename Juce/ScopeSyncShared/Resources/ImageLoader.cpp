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
#include "../Resources/ImageResources.h"

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
    // ScopeSync Logo
    addImageResource("scopeSyncLogo", ImageResources::scopesync_logo_png, ImageResources::scopesync_logo_pngSize);
    
    // Load Config buttons
    addImageResource("loadConfigButtonOff",  ImageResources::load_config_button_off_png,  ImageResources::load_config_button_off_pngSize);
    addImageResource("loadConfigButtonOn",   ImageResources::load_config_button_on_png,   ImageResources::load_config_button_on_pngSize);
    addImageResource("loadConfigButtonOver", ImageResources::load_config_button_over_png, ImageResources::load_config_button_over_pngSize);
    
    // Patch Window buttons
    addImageResource("patchWindowButtonOff",  ImageResources::patch_window_button_off_png,  ImageResources::patch_window_button_off_pngSize);
    addImageResource("patchWindowButtonOn",   ImageResources::patch_window_button_on_png,   ImageResources::patch_window_button_on_pngSize);
    addImageResource("patchWindowButtonOver", ImageResources::patch_window_button_over_png, ImageResources::patch_window_button_over_pngSize);
    
    // Presets buttons
    addImageResource("presetsButtonOff",  ImageResources::presets_button_off_png,  ImageResources::presets_button_off_pngSize);
    addImageResource("presetsButtonOn",   ImageResources::presets_button_on_png,   ImageResources::presets_button_on_pngSize);
    addImageResource("presetsButtonOver", ImageResources::presets_button_over_png, ImageResources::presets_button_over_pngSize);
    
    // Reload buttons
    addImageResource("reloadButtonOff",  ImageResources::reload_button_off_png,  ImageResources::reload_button_off_pngSize);
    addImageResource("reloadButtonOn",   ImageResources::reload_button_on_png,   ImageResources::reload_button_on_pngSize);
    addImageResource("reloadButtonOver", ImageResources::reload_button_over_png, ImageResources::reload_button_over_pngSize);
    
    // Remove buttons
    addImageResource("removeButtonOff",  ImageResources::remove_button_off_png,  ImageResources::remove_button_off_pngSize);
    addImageResource("removeButtonOn",   ImageResources::remove_button_on_png,   ImageResources::remove_button_on_pngSize);
    addImageResource("removeButtonOver", ImageResources::remove_button_over_png, ImageResources::remove_button_over_pngSize);
    
    // Settings buttons
    addImageResource("settingsButtonOff",  ImageResources::settings_button_off_png,  ImageResources::settings_button_off_pngSize);
    addImageResource("settingsButtonOn",   ImageResources::settings_button_on_png,   ImageResources::settings_button_on_pngSize);
    addImageResource("settingsButtonOver", ImageResources::settings_button_over_png, ImageResources::settings_button_over_pngSize);
    
    // Snapshot buttons
    addImageResource("snapshotOff",  ImageResources::snapshot_button_off_png,  ImageResources::snapshot_button_off_pngSize);
    addImageResource("snapshotOn",   ImageResources::snapshot_button_on_png,   ImageResources::snapshot_button_on_pngSize);
    addImageResource("snapshotOver", ImageResources::snapshot_button_over_png, ImageResources::snapshot_button_over_pngSize);
	
	// Config Manager buttons
    addImageResource("showconfigurationmanagerOff",  ImageResources::config_manager_button_off_png,  ImageResources::config_manager_button_off_pngSize);
    addImageResource("showconfigurationmanagerOn",   ImageResources::config_manager_button_on_png,   ImageResources::config_manager_button_on_pngSize);
    addImageResource("showconfigurationmanagerOver", ImageResources::config_manager_button_over_png, ImageResources::config_manager_button_over_pngSize);
}

void ImageLoader::addImageResource(String imageName, const char* image, int imageSize)
{
    ImageResource* imageResource = new ImageResource();
    imageResource->imageName = imageName;
    imageResource->image     = image;
    imageResource->imageSize = imageSize;
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

        if (directory.exists())
        {
            File imageFile = directory.getChildFile(imageFileName);

            if (useImageCache)
                return ImageCache::getFromFile(imageFile);
            else
                return ImageFileFormat::loadFrom(imageFile);
        }
    }

    return Image();
}
