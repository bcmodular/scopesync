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
#include "../Components/BCMLookAndFeel.h"

juce_ImplementSingleton (ImageLoader)

ImageLoader::ImageLoader()
{
    loadImageResources();
    defaultBCMLookAndFeel = new BCMLookAndFeel(true);
    LookAndFeel::setDefaultLookAndFeel(defaultBCMLookAndFeel);
}

ImageLoader::~ImageLoader()
{
    LookAndFeel::setDefaultLookAndFeel(nullptr);
    clearSingletonInstance();
}

void ImageLoader::loadImageResources()
{

    // ScopeSync Logo
    addImageResource("scopeSyncLogo", BinaryData::ScopeSync_Logo_png, BinaryData::ScopeSync_Logo_pngSize);
    addImageResource("scopeSyncLogoOff", BinaryData::Scopesync_logo_off_png, BinaryData::scopesync_logo_off_pngSize);
    addImageResource("scopeSyncLogoOn", BinaryData::Scopesync_logo_on_png, BinaryData::scopesync_logo_on_pngSize);
    
    // Load Config buttons
    addImageResource("loadConfigButtonOff",  BinaryData::load_config_button_off_png,  BinaryData::load_config_button_off_pngSize);
    addImageResource("loadConfigButtonOn",   BinaryData::load_config_button_on_png,   BinaryData::load_config_button_on_pngSize);
    addImageResource("loadConfigButtonOver", BinaryData::load_config_button_over_png, BinaryData::load_config_button_over_pngSize);
    
    // Patch Window buttons
    addImageResource("patchWindowButtonOff",  BinaryData::patch_window_button_off_png,  BinaryData::patch_window_button_off_pngSize);
    addImageResource("patchWindowButtonOn",   BinaryData::patch_window_button_on_png,   BinaryData::patch_window_button_on_pngSize);
    addImageResource("patchWindowButtonOver", BinaryData::patch_window_button_over_png, BinaryData::patch_window_button_over_pngSize);
    
    // Presets buttons
    addImageResource("presetsButtonOff",  BinaryData::presets_button_off_png,  BinaryData::presets_button_off_pngSize);
    addImageResource("presetsButtonOn",   BinaryData::presets_button_on_png,   BinaryData::presets_button_on_pngSize);
    addImageResource("presetsButtonOver", BinaryData::presets_button_over_png, BinaryData::presets_button_over_pngSize);
    
    // Reload buttons
    addImageResource("reloadButtonOff",  BinaryData::reload_button_off_png,  BinaryData::reload_button_off_pngSize);
    addImageResource("reloadButtonOn",   BinaryData::reload_button_on_png,   BinaryData::reload_button_on_pngSize);
    addImageResource("reloadButtonOver", BinaryData::reload_button_over_png, BinaryData::reload_button_over_pngSize);
    
    // Remove buttons
    addImageResource("removeButtonOff",  BinaryData::remove_button_off_png,  BinaryData::remove_button_off_pngSize);
    addImageResource("removeButtonOn",   BinaryData::remove_button_on_png,   BinaryData::remove_button_on_pngSize);
    addImageResource("removeButtonOver", BinaryData::remove_button_over_png, BinaryData::remove_button_over_pngSize);
    
    // Settings buttons
    addImageResource("settingsButtonOff",  BinaryData::settings_button_off_png,  BinaryData::settings_button_off_pngSize);
    addImageResource("settingsButtonOn",   BinaryData::settings_button_on_png,   BinaryData::settings_button_on_pngSize);
    addImageResource("settingsButtonOver", BinaryData::settings_button_over_png, BinaryData::settings_button_over_pngSize);
    
    // Snapshot buttons
    addImageResource("snapshotOff",  BinaryData::snapshot_button_off_png,  BinaryData::snapshot_button_off_pngSize);
    addImageResource("snapshotOn",   BinaryData::snapshot_button_on_png,   BinaryData::snapshot_button_on_pngSize);
    addImageResource("snapshotOver", BinaryData::snapshot_button_over_png, BinaryData::snapshot_button_over_pngSize);

	// Show Config Manager buttons
    addImageResource("showconfigurationmanagerOff",  BinaryData::config_manager_button_off_png,  BinaryData::config_manager_button_off_pngSize);
    addImageResource("showconfigurationmanagerOn",   BinaryData::config_manager_button_on_png,   BinaryData::config_manager_button_on_pngSize);
    addImageResource("showconfigurationmanagerOver", BinaryData::config_manager_button_over_png, BinaryData::config_manager_button_over_pngSize);

    // Save buttons
    addImageResource("saveOff",  BinaryData::save_button_off_png,  BinaryData::save_button_off_pngSize);
    addImageResource("saveOn",   BinaryData::save_button_on_png,   BinaryData::save_button_on_pngSize);
    addImageResource("saveOver", BinaryData::save_button_over_png, BinaryData::save_button_over_pngSize);

    // SaveAs buttons
    addImageResource("saveAsOff",  BinaryData::saveas_button_off_png,  BinaryData::saveas_button_off_pngSize);
    addImageResource("saveAsOn",   BinaryData::saveas_button_on_png,   BinaryData::saveas_button_on_pngSize);
    addImageResource("saveAsOver", BinaryData::saveas_button_over_png, BinaryData::saveas_button_over_pngSize);

    // Close buttons
    addImageResource("closeOff",  BinaryData::close_button_off_png,  BinaryData::close_button_off_pngSize);
    addImageResource("closeOn",   BinaryData::close_button_on_png,   BinaryData::close_button_on_pngSize);
    addImageResource("closeOver", BinaryData::close_button_over_png, BinaryData::close_button_over_pngSize);
	
    // Confirm buttons
    addImageResource("confirmOff",  BinaryData::confirm_button_off_png,  BinaryData::confirm_button_off_pngSize);
    addImageResource("confirmOn",   BinaryData::confirm_button_on_png,   BinaryData::confirm_button_on_pngSize);
    addImageResource("confirmOver", BinaryData::confirm_button_over_png, BinaryData::confirm_button_over_pngSize);
	
    // Copy buttons
    addImageResource("copyOff",  BinaryData::Copy_button_off_png,  BinaryData::copy_button_off_pngSize);
    addImageResource("copyOn",   BinaryData::copy_button_on_png,   BinaryData::copy_button_on_pngSize);
    addImageResource("copyOver", BinaryData::copy_button_over_png, BinaryData::copy_button_over_pngSize);
	
    // Divider
    addImageResource("divider",  BinaryData::divider_png,  BinaryData::divider_pngSize);
    
    // New configuration buttons
    addImageResource("newConfigOff",  BinaryData::newconfig_button_off_png,  BinaryData::newconfig_button_off_pngSize);
    addImageResource("newConfigOn",   BinaryData::newconfig_button_on_png,   BinaryData::newconfig_button_on_pngSize);
    addImageResource("newConfigOver", BinaryData::newconfig_button_over_png, BinaryData::newconfig_button_over_pngSize);
	
    // Paste buttons
    addImageResource("pasteOff",  BinaryData::paste_button_off_png,  BinaryData::paste_button_off_pngSize);
    addImageResource("pasteOn",   BinaryData::paste_button_on_png,   BinaryData::paste_button_on_pngSize);
    addImageResource("pasteOver", BinaryData::paste_button_over_png, BinaryData::paste_button_over_pngSize);
	
    // Redo buttons
    addImageResource("redoOff",  BinaryData::redo_button_off_png,  BinaryData::redo_button_off_pngSize);
    addImageResource("redoOn",   BinaryData::redo_button_on_png,   BinaryData::redo_button_on_pngSize);
    addImageResource("redoOver", BinaryData::redo_button_over_png, BinaryData::redo_button_over_pngSize);
	
    // Undo buttons
    addImageResource("undoOff",  BinaryData::undo_button_off_png,  BinaryData::undo_button_off_pngSize);
    addImageResource("undoOn",   BinaryData::undo_button_on_png,   BinaryData::undo_button_on_pngSize);
    addImageResource("undoOver", BinaryData::undo_button_over_png, BinaryData::undo_button_over_pngSize);

    // Help buttons
    addImageResource("helpOff",  BinaryData::help_button_off_png,  BinaryData::help_button_off_pngSize);
    addImageResource("helpOn",   BinaryData::help_button_on_png,   BinaryData::help_button_on_pngSize);
    addImageResource("helpOver", BinaryData::help_button_over_png, BinaryData::help_button_over_pngSize);

    // Toolbar buttons
    addImageResource("toolbarBevel", BinaryData::toolbar_bevel_png,       BinaryData::toolbar_bevel_pngSize);
    addImageResource("toolbarOff",   BinaryData::toolbar_button_off_png,  BinaryData::toolbar_button_off_pngSize);
    addImageResource("toolbarOn",    BinaryData::toolbar_button_on_png,   BinaryData::toolbar_button_on_pngSize);
    addImageResource("toolbarOver",  BinaryData::toolbar_button_over_png, BinaryData::toolbar_button_over_pngSize);

	// FX bypass buttons
    addImageResource("fxBypassOff",  BinaryData::fxbypass_button_off_png,  BinaryData::fxbypass_button_off_pngSize);
    addImageResource("fxBypassOn",   BinaryData::fxbypass_button_on_png,   BinaryData::fxbypass_button_on_pngSize);
    addImageResource("fxBypassOver", BinaryData::fxbypass_button_over_png, BinaryData::fxbypass_button_over_pngSize);

	// FX mono buttons
    addImageResource("fxMonoOff",  BinaryData::fxmono_button_off_png,  BinaryData::fxmono_button_off_pngSize);
    addImageResource("fxMonoOn",   BinaryData::fxmono_button_on_png,   BinaryData::fxmono_button_on_pngSize);
    addImageResource("fxMonoOver", BinaryData::fxmono_button_over_png, BinaryData::fxmono_button_over_pngSize);

	// Performance mode buttons
    addImageResource("perfModeOff",  BinaryData::perf_mode_button_off_png,  BinaryData::perf_mode_button_off_pngSize);
    addImageResource("perfModeOn",   BinaryData::perf_mode_button_on_png,   BinaryData::perf_mode_button_on_pngSize);
    addImageResource("perfModeOver", BinaryData::perf_mode_button_over_png, BinaryData::perf_mode_button_over_pngSize);

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
            return ImageCache::getFromMemory(imageResources[i]->image, imageResources[i]->imageSize);
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
