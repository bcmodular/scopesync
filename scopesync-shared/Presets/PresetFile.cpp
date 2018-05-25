/**
 * Classes used in managing Parameter Preset files
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

#include "PresetFile.h"
#include "../Core/Global.h"
#include "../Utils/BCMMisc.h"
#include "../Configuration/ConfigurationPanel.h"
#include "../Configuration/SettingsTable.h"

/* =========================================================================
 * PresetFilePanel
 */
PresetFilePanel::PresetFilePanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm)
    : BasePanel(node, um, acm)
{
    PresetFilePanel::rebuildProperties();
}

PresetFilePanel::~PresetFilePanel() {}

void PresetFilePanel::rebuildProperties()
{
    propertyPanel.clear();

    PropertyListBuilder props;

    props.clear();
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::name,       &undoManager), "Name",        256,  false), "Name of Configuration");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::librarySet, &undoManager), "Library Set", 256,  false), "Library Set for Configuration");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::author,     &undoManager), "Author",      256,  false), "Name of Configuration's creator");
    props.add(new TextPropertyComponent(valueTree.getPropertyAsValue(Ids::blurb,      &undoManager), "Blurb",       1024, true),  "Textual description of Configuration (shown in Chooser)");

    propertyPanel.addSection("Preset File Settings", props.components);
}

/* =========================================================================
 * PresetPanel
 */
PresetPanel::PresetPanel(ValueTree& preset, UndoManager& um, PresetFile& pf,
                         ApplicationCommandManager* acm)
    : BasePanel(preset, um, acm), presetFile(pf)
{
    // Listen for changes to the valueType, to decide whether or not to show
    // the SettingsTable
    valueType = valueTree.getPropertyAsValue(Ids::valueType, &undoManager);
    valueType.addListener(this);

    PresetPanel::rebuildProperties();

    if (int(valueType.getValue()) == 1)
        createSettingsTable();
}

PresetPanel::~PresetPanel()
{
    valueType.removeListener(this);
}

void PresetPanel::rebuildProperties()
{
    propertyPanel.clear();

    PropertyListBuilder props;
    ParameterPanel::createDescriptionProperties(props, undoManager, valueTree, true);
    propertyPanel.addSection("Main Properties", props.components, true);

    ParameterPanel::createScopeProperties(props, undoManager, valueTree, valueType.getValue());
    propertyPanel.addSection("Scope Properties", props.components, true);
    
    ParameterPanel::createUIProperties(props, undoManager, valueTree, valueType.getValue());
    propertyPanel.addSection("UI Properties", props.components, true);
}

void PresetPanel::childBoundsChanged(Component* child)
{
    if (child == settingsTable)
    {
        presetFile.getPresetProperties().setValue("lastSettingsTableHeight", settingsTable->getHeight());
        resized();
    }      
}

void PresetPanel::paintOverChildren(Graphics& g)
{
    if (settingsTable != nullptr)
    {
        const int shadowSize = 15;
    
        const int resizerY = resizerBar->getY();

        ColourGradient resizerCG (Colours::black.withAlpha (0.25f), 0, static_cast<float>(resizerY), 
                                  Colours::transparentBlack,        0, static_cast<float>(resizerY + shadowSize), false);
        resizerCG.addColour (0.4, Colours::black.withAlpha (0.07f));
        resizerCG.addColour (0.6, Colours::black.withAlpha (0.02f));

        g.setGradientFill(resizerCG);
        g.fillRect(resizerBar->getX(), resizerY, resizerBar->getWidth(), shadowSize);
    }
}

void PresetPanel::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    
    localBounds.removeFromLeft(4);
    localBounds.removeFromRight(8);
    localBounds.removeFromTop(8);

    if (settingsTable != nullptr)
    {
        settingsTable->setBounds(localBounds.removeFromBottom(settingsTable->getHeight()));
        resizerBar->setBounds(localBounds.removeFromBottom(4));
    }

    propertyPanel.setBounds(localBounds);
}

void PresetPanel::valueChanged(Value& valueThatChanged)
{
    if (int(valueThatChanged.getValue()) == 0)
    {
        resizerBar.reset();
        settingsTable.reset();
        ParameterPanel::setParameterUIRanges(0, 100, 0, undoManager, valueTree);
        rebuildProperties();
        resized();
    }
    else
    {
        rebuildProperties();
        createSettingsTable();
        resized();
    }
}

void PresetPanel::createSettingsTable()
{
    int minHeight;
    int maxHeight;
    int lastSettingsTableHeight;

    minHeight = 200;
    maxHeight = 700;
    lastSettingsTableHeight = presetFile.getPresetProperties().getIntValue("lastSettingsTableHeight", 250);

    settingsTableConstrainer.setMinimumHeight(minHeight);
    settingsTableConstrainer.setMaximumHeight(maxHeight);

    ValueTree settings = valueTree.getOrCreateChildWithName(Ids::settings, &undoManager);
    
    int maxValue = jmax(settings.getNumChildren() - 1, 1);
    ParameterPanel::setParameterUIRanges(0, maxValue, 0, undoManager, valueTree);

    // Reset Skew values to default to save confusion
    valueTree.setProperty(Ids::uiSkewMidpoint, String(), &undoManager);
    valueTree.setProperty(Ids::scopeDBRef, 0.0f, &undoManager);

    settingsTable = new SettingsTable(settings, undoManager, commandManager, valueTree, commandManager->getFirstCommandTarget(CommandIDs::addPresetFile));
    settingsTable->setBounds(0, 0, getWidth(), lastSettingsTableHeight);
    addAndMakeVisible(settingsTable);

    addAndMakeVisible(resizerBar = new ResizableEdgeComponent(settingsTable, &settingsTableConstrainer,
                                                                ResizableEdgeComponent::topEdge));
    resizerBar->setAlwaysOnTop (true);
}

/* =========================================================================
 * PresetFile
 */
PresetFile::PresetFile()
    : FileBasedDocument(presetFileFileExtension,
                        String ("*") + presetFileFileExtension,
                        "Choose a Preset File to load",
                        "Save Preset File"),
    presetFileRoot(Ids::presets)
{
    setPresetFileRoot(presetFileRoot);
    setChangedFlag(false);
    presetFileRoot.addListener(this);
    setupPresetProperties();
}

PresetFile::~PresetFile()
{
    presetFileRoot.removeListener(this);
};

void PresetFile::setPresetFileRoot(const ValueTree& newRoot)
{
    presetFileRoot.removeListener(this);
    presetFileRoot = newRoot;
    presetFileRoot.addListener(this);
}

const char* PresetFile::presetFileFileExtension = ".presets";

String PresetFile::getDocumentTitle()
{
    return presetFileRoot.getProperty(Ids::name, String());
}

void PresetFile::setupPresetProperties()
{
    const String filename ("PresetFileManager");

    PropertiesFile::Options options;
    options.applicationName     = filename;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    properties = new PropertiesFile(options);
}

PropertiesFile& PresetFile::getPresetProperties() const
{
    return *properties;
}

Result PresetFile::loadDocument(const File& file)
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(file));

    if (xml == nullptr || !(xml->hasTagName(Ids::presets)))
        return Result::fail("Not a valid ScopeSync Preset File");

    ValueTree newTree (ValueTree::fromXml(*xml));

    if (!newTree.hasType(Ids::presets))
        return Result::fail("The document contains errors and couldn't be parsed");

    setPresetFileRoot(newTree);
    
    return Result::ok();
}

void PresetFile::createPresetFile()
{
    setFile(File());
        
    ValueTree emptyTree(Ids::presets);
    setPresetFileRoot(emptyTree);

    save(true, true);
}

void PresetFile::addNewPreset(ValueTree& newPreset, const ValueTree& presetValues, int targetIndex, UndoManager* um)
{
    if (presetValues.isValid())
        newPreset = presetValues.createCopy();
    else
        newPreset = getDefaultPreset().createCopy();

    String presetNameBase = newPreset.getProperty(Ids::name);

    if (presetNameBase.isEmpty())
        presetNameBase = getDefaultPreset().getProperty(Ids::name);

    String newPresetName;
    int settingNum = 1;
    
    for (;;)
    {
        newPresetName = presetNameBase + String(settingNum);
        
        if (presetNameExists(newPresetName))
        {
            settingNum++;
            continue;
        }
        else
        {
            newPreset.setProperty(Ids::name, newPresetName, um);
            break;
        }
    }

    presetFileRoot.addChild(newPreset, targetIndex, um);
}

ValueTree PresetFile::getDefaultPreset()
{
    ValueTree defaultPreset(Ids::preset);
    defaultPreset.setProperty(Ids::name,             "Preset ",     nullptr);
    defaultPreset.setProperty(Ids::blurb,            String(), nullptr);
    defaultPreset.setProperty(Ids::scopeParamGroup,  -1,            nullptr);
    defaultPreset.setProperty(Ids::scopeParamId,     -1,            nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMin,    0,             nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMax,    2147483647,    nullptr);
    defaultPreset.setProperty(Ids::scopeDBRef,       0,             nullptr);
    defaultPreset.setProperty(Ids::valueType,        0,             nullptr);
    defaultPreset.setProperty(Ids::uiResetValue,     0,             nullptr);
    defaultPreset.setProperty(Ids::uiSkewFactor,     1,             nullptr);
    defaultPreset.setProperty(Ids::skewUIOnly,       false,         nullptr);
    defaultPreset.setProperty(Ids::uiRangeMin,       0,             nullptr);
    defaultPreset.setProperty(Ids::uiRangeMax,       100,           nullptr);
    defaultPreset.setProperty(Ids::uiRangeInterval,  0.0001,        nullptr);
    defaultPreset.setProperty(Ids::uiSuffix,         String(), nullptr);
    
    return defaultPreset;
}

bool PresetFile::presetNameExists(const String& presetName) const
{
    if (presetFileRoot.getChildWithProperty(Ids::name, presetName).isValid())
        return true;

    return false;
}

Result PresetFile::saveDocument (const File& /* file */)
{
    userSettings->updatePresetLibraryEntry(getFile().getFullPathName(), getFile().getFileName(), presetFileRoot);

    ScopedPointer<XmlElement> outputXml = presetFileRoot.createXml();

    if (outputXml->writeToFile(getFile(), String(), "UTF-8", 120))
        return Result::ok();
    else
        return Result::fail("Failed to save Preset File");
}

static File lastDocumentOpened;

File PresetFile::getLastDocumentOpened()                   { return lastDocumentOpened; }
void PresetFile::setLastDocumentOpened (const File& file)  { lastDocumentOpened = file; }

const String& PresetFile::getLastError() const
{
    return lastError;
}

const String& PresetFile::getLastErrorDetails() const
{
    return lastErrorDetails;
}

FileBasedDocument::SaveResult PresetFile::saveIfNeededAndUserAgrees(bool offerCancelOption)
{
    if (!(hasChangedSinceSaved()) || !getFile().existsAsFile())
        return savedOk;

    int result;

    if (offerCancelOption)
    {
        result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                                   TRANS("Loading New Preset File..."),
                                                   TRANS("Do you want to save the changes to \"DCNM\"?")
                                                    .replace ("DCNM", getDocumentTitle()),
                                                   TRANS("Save"),
                                                   TRANS("Discard changes"),
                                                   TRANS("Cancel"));
    }
    else
    {
        bool buttonState = AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon,
                                                   TRANS("Loading New Preset File..."),
                                                   TRANS("Do you want to save the changes to \"DCNM\"?")
                                                    .replace ("DCNM", getDocumentTitle()),
                                                   TRANS("Save"),
                                                   TRANS("Discard changes"));
        if (buttonState)
            result = 1;
        else
            result = 2;
    }

    if (result == 1)  // save changes
        return save(true, true);

    if (result == 2)  // discard changes
        return savedOk;

    return userCancelledSave;
}

void PresetFile::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
	if (property == Ids::uiRangeMin || property == Ids::uiRangeMax || property == Ids::uiSkewMidpoint)
    {
        float minimum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMin);
        float maximum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMax);
        float midpoint   = treeWhosePropertyHasChanged.getProperty(Ids::uiSkewMidpoint);
        float skewFactor = 1.0f;

        if (maximum > minimum && midpoint > minimum && midpoint < maximum)
            skewFactor = static_cast<float>(log(0.5) / log((midpoint - minimum) / (maximum - minimum)));

        treeWhosePropertyHasChanged.setProperty(Ids::uiSkewFactor, skewFactor, nullptr);
    }

    changed();
}

void PresetFile::valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */) { changed(); }
void PresetFile::valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved*/, int /* oldIndex */ ) { changed(); }
void PresetFile::valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */, int /* oldIndex */, int /* newIndex */) { changed(); }
void PresetFile::valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */) { changed(); }

ValueTree PresetFile::getPresetFileRoot() const
{
    return presetFileRoot;
}
