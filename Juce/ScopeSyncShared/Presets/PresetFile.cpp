/*
  ==============================================================================

    Presets.cpp
    Created: 2 Nov 2014 7:31:28am
    Author:  giles

  ==============================================================================
*/

#include "PresetFile.h"
#include "../Core/Global.h"
#include "../Utils/BCMMath.h"
#include "../Utils/BCMMisc.h"
#include "../Core/ScopeSync.h"
#include "../Configuration/ConfigurationPanel.h"
#include "../Configuration/SettingsTable.h"

/* =========================================================================
 * NewPresetFileWindow
 */
NewPresetFileWindow::NewPresetFileWindow(int posX, int posY,
                                         PresetFile& pf,
                                         UndoManager& um,
                                         const File& file,
                                         ApplicationCommandManager* acm)
    : DocumentWindow("New Preset File",
                     Colour::greyLevel(0.6f),
                     DocumentWindow::allButtons,
                     true),
      newFile(file),
      presetFile(pf),
      undoManager(um)
{
    cancelled = true;

    setUsingNativeTitleBar (true);
    
    settings = ValueTree(Ids::presets);

    setContentOwned(new NewPresetFileEditor(settings, um, file.getFullPathName(), acm), true);
    
    restoreWindowPosition(posX, posY);
    
    setVisible(true);
    setResizable(true, false);

    setWantsKeyboardFocus (false);

    setResizeLimits(400, 200, 32000, 32000);
}

NewPresetFileWindow::~NewPresetFileWindow() {}

void NewPresetFileWindow::addPresetFile()
{
    cancelled = false;
    sendChangeMessage();
}

ValueTree NewPresetFileWindow::getSettings()
{
    return settings;
}

void NewPresetFileWindow::cancel() { sendChangeMessage(); }

void NewPresetFileWindow::closeButtonPressed() { sendChangeMessage(); }

void NewPresetFileWindow::restoreWindowPosition(int posX, int posY)
{
    setCentrePosition(posX, posY);
}

/* =========================================================================
 * PresetFilePanel
 */
PresetFilePanel::PresetFilePanel(ValueTree& node, UndoManager& um, ApplicationCommandManager* acm)
    : BasePanel(node, um, acm)
{
    rebuildProperties();
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

    rebuildProperties();

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
    ParameterPanel::createDescriptionProperties(props, undoManager, valueTree, BCMParameter::preset);
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

        ColourGradient resizerCG (Colours::black.withAlpha (0.25f), 0, (float) resizerY, 
                                  Colours::transparentBlack,        0, (float) (resizerY + shadowSize), false);
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
        resizerBar = nullptr;
        settingsTable = nullptr;
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
    valueTree.setProperty(Ids::uiSkewMidpoint, String::empty, &undoManager);
    valueTree.setProperty(Ids::scopeDBRef, 0.0f, &undoManager);

    settingsTable = new SettingsTable(settings, undoManager, commandManager, valueTree);
    settingsTable->setBounds(0, 0, getWidth(), lastSettingsTableHeight);
    addAndMakeVisible(settingsTable);

    addAndMakeVisible(resizerBar = new ResizableEdgeComponent(settingsTable, &settingsTableConstrainer,
                                                                ResizableEdgeComponent::topEdge));
    resizerBar->setAlwaysOnTop (true);
}

/* =========================================================================
 * NewPresetFileEditor
 */
NewPresetFileEditor::NewPresetFileEditor(ValueTree& settings,
                                         UndoManager& um,
                                         const String& filePath,
                                         ApplicationCommandManager* acm)
    : commandManager(acm),
      addButton("Add Preset File"),
      cancelButton("Cancel"),
      filePathLabel("File Path"),
      panel(settings, um, acm),
      undoManager(um)
{
    commandManager->registerAllCommandsForTarget(this);

    addButton.setCommandToTrigger(commandManager, CommandIDs::addConfig, true);
    addAndMakeVisible(addButton);

    cancelButton.setCommandToTrigger(commandManager, CommandIDs::cancel, true);
    addAndMakeVisible(cancelButton);

    filePathLabel.setText(filePath, dontSendNotification);
    filePathLabel.setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(filePathLabel);

    addAndMakeVisible(panel);

    addKeyListener(commandManager->getKeyMappings());

    setBounds(0, 0, 600, 400);
}

NewPresetFileEditor::~NewPresetFileEditor() {}

void NewPresetFileEditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}
    
void NewPresetFileEditor::resized()
{
    Rectangle<int> localBounds(getLocalBounds());
    Rectangle<int> buttonBar(localBounds.removeFromBottom(30));
    
    addButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    cancelButton.setBounds(buttonBar.removeFromLeft(140).reduced(3, 3));
    filePathLabel.setBounds(localBounds.removeFromBottom(30).reduced(3, 3));
    panel.setBounds(localBounds.reduced(4, 4));
}

void NewPresetFileEditor::addPresetFile()
{
    NewPresetFileWindow* parent = static_cast<NewPresetFileWindow*>(getParentComponent());
    parent->addPresetFile();
}

void NewPresetFileEditor::cancel()
{
    NewPresetFileWindow* parent = static_cast<NewPresetFileWindow*>(getParentComponent());
    parent->cancel();
}

void NewPresetFileEditor::getAllCommands(Array <CommandID>& commands)
{
    const CommandID ids[] = {CommandIDs::addPresetFile,
                             CommandIDs::cancel
                             };
    
    commands.addArray(ids, numElementsInArray (ids));
}

void NewPresetFileEditor::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case CommandIDs::addPresetFile:
        result.setInfo("Add Preset File", "Create a new Preset File", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('w', ModifierKeys::commandModifier, 0));
        break;
    case CommandIDs::cancel:
        result.setInfo("Cancel", "Cancel current action", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress ('q', ModifierKeys::commandModifier, 0));
        break;
    }
}

bool NewPresetFileEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::addPresetFile: addPresetFile(); break;
        case CommandIDs::cancel:        cancel(); break;
        default:                        return false;
    }

    return true;
}

ApplicationCommandTarget* NewPresetFileEditor::getNextCommandTarget()
{
    return nullptr;
}

/* =========================================================================
 * PresetFile
 */
PresetFile::PresetFile(): FileBasedDocument(presetFileFileExtension,
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
    return presetFileRoot.getProperty(Ids::name, String::empty);
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

PropertiesFile& PresetFile::getPresetProperties()
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

void PresetFile::createPresetFile(const File& newFile)
{
    if (saveIfNeededAndUserAgrees(true) == savedOk)
    {
        setFile(newFile);
        
        ValueTree presetFile(Ids::presets);
        setPresetFileRoot(presetFile);
    }
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
            String newShortDescription = newPreset.getProperty(Ids::shortDescription).toString();
            
            if (newShortDescription.isEmpty())
                newShortDescription = getDefaultPreset().getProperty(Ids::shortDescription);

            newShortDescription += " " + String(settingNum);

            String newFullDescription  = newPreset.getProperty(Ids::fullDescription).toString();
            
            if (newFullDescription.isEmpty())
                newFullDescription = getDefaultPreset().getProperty(Ids::fullDescription);

            newFullDescription +=  " " + String(settingNum);

            newPreset.setProperty(Ids::name,             newPresetName, um);
            newPreset.setProperty(Ids::shortDescription, newShortDescription, um);
            newPreset.setProperty(Ids::fullDescription,  newShortDescription, um);

            break;
        }
    }

    presetFileRoot.addChild(newPreset, targetIndex, um);
}

ValueTree PresetFile::getDefaultPreset()
{
    ValueTree defaultPreset(Ids::parameter);
    defaultPreset.setProperty(Ids::name,             "PARAM",       nullptr);
    defaultPreset.setProperty(Ids::shortDescription, "Param",       nullptr);
    defaultPreset.setProperty(Ids::fullDescription,  "Parameter",   nullptr);
    defaultPreset.setProperty(Ids::scopeSync,        -1,            nullptr);
    defaultPreset.setProperty(Ids::scopeLocal,       -1,            nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMin,    0,             nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMax,    2147483647,    nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMinFlt, 0,             nullptr);
    defaultPreset.setProperty(Ids::scopeRangeMaxFlt, 1,             nullptr);
    defaultPreset.setProperty(Ids::scopeDBRef,       0,             nullptr);
    defaultPreset.setProperty(Ids::valueType,        0,             nullptr);
    defaultPreset.setProperty(Ids::uiResetValue,     0,             nullptr);
    defaultPreset.setProperty(Ids::uiSkewFactor,     1,             nullptr);
    defaultPreset.setProperty(Ids::skewUIOnly,       false,         nullptr);
    defaultPreset.setProperty(Ids::uiRangeMin,       0,             nullptr);
    defaultPreset.setProperty(Ids::uiRangeMax,       100,           nullptr);
    defaultPreset.setProperty(Ids::uiRangeInterval,  0.0001,        nullptr);
    defaultPreset.setProperty(Ids::uiSuffix,         String::empty, nullptr);
    
    return defaultPreset;
}

bool PresetFile::presetNameExists(const String& presetName)
{
    if (presetFileRoot.getChildWithProperty(Ids::name, presetName).isValid())
        return true;

    return false;
}

Result PresetFile::saveDocument (const File& /* file */)
{
    //UserSettings::getInstance()->updatePresetLibraryEntry(getFile().getFullPathName(), getFile().getFileName(), presetFileRoot);

    ScopedPointer<XmlElement> outputXml = presetFileRoot.createXml();

    if (outputXml->writeToFile(getFile(), String::empty, "UTF-8", 120))
        return Result::ok();
    else
        return Result::fail("Failed to save Preset File");
}

static File lastDocumentOpened;

File PresetFile::getLastDocumentOpened()                   { return lastDocumentOpened; }
void PresetFile::setLastDocumentOpened (const File& file)  { lastDocumentOpened = file; }

FileBasedDocument::SaveResult PresetFile::saveIfNeededAndUserAgrees(bool offerCancelOption)
{
    if (!(hasChangedSinceSaved()) || getFile() == File::nonexistent)
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
    if (property == Ids::scopeRangeMin)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::scopeRangeMinFlt, newValue, nullptr);
    }
    else if (property == Ids::scopeRangeMax)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::scopeRangeMaxFlt, newValue, nullptr);
    }
    else if (property == Ids::intValue)
    {
        double newValue = scopeIntToDouble(treeWhosePropertyHasChanged.getProperty(property));
        treeWhosePropertyHasChanged.setProperty(Ids::value, newValue, nullptr);
    }
    else if (property == Ids::uiRangeMin || property == Ids::uiRangeMax || property == Ids::uiSkewMidpoint)
    {
        float minimum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMin);
        float maximum    = treeWhosePropertyHasChanged.getProperty(Ids::uiRangeMax);
        float midpoint   = treeWhosePropertyHasChanged.getProperty(Ids::uiSkewMidpoint);
        float skewFactor = 1.0f;

        if (maximum > minimum && midpoint > minimum && midpoint < maximum)
            skewFactor = (float)(log(0.5) / log((midpoint - minimum) / (maximum - minimum)));

        treeWhosePropertyHasChanged.setProperty(Ids::uiSkewFactor, skewFactor, nullptr);
    }

    changed();
}

void PresetFile::valueTreeChildAdded(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenAdded */)     { changed(); }
void PresetFile::valueTreeChildRemoved(ValueTree& /* parentTree */, ValueTree& /* childWhichHasBeenRemoved*/ ) { changed(); }
void PresetFile::valueTreeChildOrderChanged(ValueTree& /* parentTreeWhoseChildrenHaveMoved */)                 { changed(); }
void PresetFile::valueTreeParentChanged(ValueTree& /* treeWhoseParentHasChanged */)                            { changed(); }
