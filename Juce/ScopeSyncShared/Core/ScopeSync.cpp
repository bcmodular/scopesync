/**
 * The Core of BCModular's ScopeSync system. Handles configuration,
 * parameter management and pulling together the various aspects
 * of communication between different parts of the system.
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

#include "ScopeSync.h"

#include "../Utils/BCMMath.h"
#include "../Utils/BCMXml.h"
#include "../Resources/ImageLoader.h"
#include "ScopeSyncApplication.h"
#include "Global.h"
#include <utility>
#include "../Configuration/ConfigurationManager.h"
#include "../Components/UserSettings.h"
#include "../Resources/Icons.h"
#include "../Core/ScopeSyncGUI.h"

#ifndef __DLL_EFFECT__
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
#else
    #include "../../ScopeSyncFX/Source/ScopeFX.h"
#endif // __DLL_EFFECT__

const int ScopeSync::minHostParameters = 128;

const StringArray ScopeSync::scopeSyncCodes = StringArray::fromTokens(
"A1,A2,A3,A4,A5,A6,A7,A8,\
B1,B2,B3,B4,B5,B6,B7,B8,\
C1,C2,C3,C4,C5,C6,C7,C8,\
D1,D2,D3,D4,D5,D6,D7,D8,\
E1,E2,E3,E4,E5,E6,E7,E8,\
F1,F2,F3,F4,F5,F6,F7,F8,\
G1,G2,G3,G4,G5,G6,G7,G8,\
H1,H2,H3,H4,H5,H6,H7,H8,\
I1,I2,I3,I4,I5,I6,I7,I8,\
J1,J2,J3,J4,J5,J6,J7,J8,\
K1,K2,K3,K4,K5,K6,K7,K8,\
L1,L2,L3,L4,L5,L6,L7,L8,\
M1,M2,M3,M4,M5,M6,M7,M8,\
N1,N2,N3,N4,N5,N6,N7,N8,\
O1,O2,O3,O4,O5,O6,O7,O8,\
P1,P2,P3,P4,P5,P6,P7,P8",
",",""
);

const StringArray ScopeSync::scopeLocalCodes = StringArray::fromTokens(
"Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,\
Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8",
",",""
);

const String& ScopeSync::getScopeSyncCode(int scopeSync)
{
    return scopeSyncCodes[scopeSync];
}

const String& ScopeSync::getScopeLocalCode(int scopeLocal)
{
    return scopeLocalCodes[scopeLocal];
}

Array<ScopeSync*> ScopeSync::scopeSyncInstances;

ScopeSync::ScopeSync() : parameterValueStore("parametervalues")
{
    initialise();
}

#ifndef __DLL_EFFECT__
ScopeSync::ScopeSync(PluginProcessor* owner) : parameterValueStore("parametervalues")
{
    scopeSyncInstances.add(this);
    pluginProcessor = owner;
    initialise();
}
#else
ScopeSync::ScopeSync(ScopeFX* owner) : parameterValueStore("parametervalues")
{
    scopeSyncInstances.add(this);
    scopeFX = owner;
    initialise();
}
#endif // __DLL_EFFECT__

ScopeSync::~ScopeSync()
{
    hideConfigurationManager();
    scopeSyncInstances.removeAllInstancesOf(this);
}

void ScopeSync::initialise()
{
    showEditToolbar = false;
    initCommandManager();

    resetScopeCodeIndexes();

    configuration = new Configuration();
    applyConfiguration();
}

void ScopeSync::initCommandManager()
{
    commandManager = new ApplicationCommandManager();
}

void ScopeSync::showConfigurationManager(int posX, int posY)
{
    if (configurationManagerWindow == nullptr)
    {
        configurationManagerWindow = new ConfigurationManagerWindow(*this, posX, posY);
        
        if (ScopeSyncApplication::inScopeFXContext())
            configurationManagerWindow->setAlwaysOnTop(true);
    }

    configurationManagerWindow->toFront(true);
}

void ScopeSync::hideConfigurationManager()
{
	configurationManagerWindow = nullptr;
}

void ScopeSync::unload()
{
    if (configurationManagerWindow != nullptr)
    {
        configurationManagerWindow->unload();
        configurationManagerWindow = nullptr;
    }

    configuration->saveIfNeededAndUserAgrees(false);
}

int ScopeSync::getNumScopeSyncInstances() 
{ 
    return scopeSyncInstances.size(); 
};
    
void ScopeSync::reloadAllGUIs()
{
    for (int i = 0; i < scopeSyncInstances.size(); i++)
    {
        scopeSyncInstances[i]->setGUIReload(true);
    }
}

void ScopeSync::shutDownIfLastInstance()
{
    if (getNumScopeSyncInstances() == 0)
    {
        StyleOverrideClipboard::deleteInstance();
        ParameterClipboard::deleteInstance();
        Icons::deleteInstance();
        ImageLoader::deleteInstance();
        UserSettings::deleteInstance();

        ScopeSyncGUI::deleteTooltipWindow();

        if (ScopeSyncApplication::inScopeFXContext())
            shutdownJuce_GUI();
    }
}

void ScopeSync::resetScopeCodeIndexes()
{
    paramIdxByScopeSyncId.clear();

    for (int i = 0; i < ScopeSyncApplication::numScopeSyncParameters; i++)
        paramIdxByScopeSyncId.add(-1);

    paramIdxByScopeLocalId.clear();

    for (int i = 0; i < ScopeSyncApplication::numScopeLocalParameters; i++)
        paramIdxByScopeLocalId.add(-1);
}

void ScopeSync::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    (void)midiMessages;
#ifndef __DLL_EFFECT__
    scopeSyncAudio.processBlock(buffer);
#else
    (void)buffer;
#endif // __DLL_EFFECT__
}

void ScopeSync::snapshot()
{
#ifndef __DLL_EFFECT__
    scopeSyncAudio.snapshot();
#else
    scopeSyncAsync.createSnapshot();
#endif // __DLL_EFFECT__
}

void ScopeSync::beginParameterChangeGesture(BCMParameter* parameter)
{
    if (parameter != nullptr)
    {
#ifndef __DLL_EFFECT__
        int hostIdx = parameter->getHostIdx();
        
        if (!changingParams[hostIdx])
        {
            pluginProcessor->beginParameterChangeGesture(hostIdx);
            changingParams.setBit(hostIdx);
        }
#else
        parameter->setAffectedByUI(true);
#endif // __DLL_EFFECT__
    }
}

void ScopeSync::endParameterChangeGesture(BCMParameter* parameter)
{
    if (parameter != nullptr)
    {
#ifndef __DLL_EFFECT__
        int hostIdx = parameter->getHostIdx();
        
        if (changingParams[hostIdx])
        {
            pluginProcessor->endParameterChangeGesture(hostIdx); 
            changingParams.clearBit(hostIdx);
        }
#else
        parameter->setAffectedByUI(false);
#endif // __DLL_EFFECT__
    }
}

void ScopeSync::endAllParameterChangeGestures()
{
    for (int i = 0; i < hostParameters.size(); i++)
    {
        BCMParameter* parameter = hostParameters[i];
#ifndef __DLL_EFFECT__
        int hostIdx = parameter->getHostIdx();

        if (changingParams[hostIdx])
        {
            pluginProcessor->endParameterChangeGesture(hostIdx); 
            changingParams.clearBit(hostIdx);
        }
#else
        parameter->setAffectedByUI(false);
#endif // __DLL_EFFECT__
    }
}

void ScopeSync::receiveUpdatesFromScopeAudio()
{
    if (scopeSyncAudio.controlUpdates.size() > 0)
    {
        const ScopedLock cuLock(scopeSyncAudio.controlUpdateLock);
        audioControlUpdates.swapWith(scopeSyncAudio.controlUpdates);
        
        for (int i = 0; i < audioControlUpdates.size(); i++)
        {
            int   scopeSyncCode = audioControlUpdates[i].first;
            float newScopeValue = audioControlUpdates[i].second;
            
            int paramIdx = paramIdxByScopeSyncId[scopeSyncCode];
            
            if (paramIdx >= 0)
            {
                BCMParameter* parameter = hostParameters[paramIdx];
                parameter->setScopeFltValue(newScopeValue);
#ifndef __DLL_EFFECT__
                pluginProcessor->updateListeners(paramIdx, parameter->getHostValue());
#endif // __DLL_EFFECT__
            }
        }    
    }
    audioControlUpdates.clear();
}      

void ScopeSync::receiveUpdatesFromScopeAsync()
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.getAsyncUpdatesArray(asyncControlUpdates);

    int numUpdates = asyncControlUpdates.size();

    Array<int> processedParams;

    for (int i = numUpdates - 1; i >= 0 ; i--)
    {
        int scopeCode     = asyncControlUpdates[i].first;
        int newScopeValue = asyncControlUpdates[i].second;

        if (!(processedParams.contains(scopeCode)))
        {
            BCMParameter* parameter = nullptr;

            if (scopeCode < ScopeSyncApplication::numScopeSyncParameters)
            {
                int paramIdx = paramIdxByScopeSyncId[scopeCode];
                
                if (paramIdx >= 0)
                    parameter = hostParameters[paramIdx];
            }
            else
            {
                int paramIdx = paramIdxByScopeLocalId[scopeCode - ScopeSyncApplication::numScopeSyncParameters];
                
                if (paramIdx >= 0)
                    parameter = scopeLocalParameters[paramIdx];
            }
            
            if (parameter != nullptr)
                parameter->setScopeIntValue(newScopeValue);
            
            processedParams.add(scopeCode);
        }
        else
        {
            DBG("ScopeSync::receiveUpdatesFromScopeAsync: throwing away extra update: " + String(scopeCode));
        }
    }
    asyncControlUpdates.clear();
#endif // __DLL_EFFECT__
} 

void ScopeSync::sendToScopeSyncAudio(BCMParameter& parameter)
{
    int scopeCode = parameter.getScopeCode();

    if (scopeCode != -1)
    {
        float newScopeValue = parameter.getScopeFltValue();

        DBG("ScopeSync::sendToScopeSyncAudio: " + String(scopeCode) + ", scaled value: " + String(newScopeValue));
        scopeSyncAudio.setControlValue(scopeCode, newScopeValue);
    }
}

void ScopeSync::sendToScopeSyncAsync(BCMParameter& parameter)
{
#ifdef __DLL_EFFECT__
    // Try to grab the ScopeSyncId for the relevant parameter
    int scopeCode = parameter.getScopeCode();

    if (scopeCode != -1)
    {
        int newScopeValue = parameter.getScopeIntValue();

        DBG("ScopeSync::sendToScopeSyncAsync: " + String(scopeCode) + ", scaled value: " + String(newScopeValue));
        scopeSyncAsync.setValue(scopeCode, newScopeValue);
    }
    else
    {
        String shortDesc;
        String longDesc;
        parameter.getDescriptions(shortDesc, longDesc);
        DBG("ScopeSync::sendToScopeSyncAsync: couldn't find Scope code for parameter: " + longDesc);
    }
#else
    (void)parameter;
#endif // __DLL_EFFECT__
}

bool ScopeSync::guiNeedsReloading()
{
    const ScopedLock lock(flagLock);
    return reloadGUI;
};

void ScopeSync::setGUIReload(bool reloadGUIFlag)
{
    const ScopedLock lock(flagLock);
    reloadGUI = reloadGUIFlag;
};

void ScopeSync::receiveUpdates()
{
    if (ScopeSyncApplication::inPluginContext())
        receiveUpdatesFromScopeAudio();
    else
        receiveUpdatesFromScopeAsync();
}
    
int ScopeSync::getNumParametersForHost()
{
    int numHostParameters = hostParameters.size();

    if (numHostParameters < minHostParameters)
        numHostParameters = minHostParameters;

    //DBG("ScopeSync::getNumHostParameters - " + String(numHostParameters));
    return numHostParameters;
}

BCMParameter* ScopeSync::getParameterByName(const String& name)
{
    for (int i = 0; i < hostParameters.size(); i++)
    {
        if (hostParameters[i]->getName().equalsIgnoreCase(name))
            return hostParameters[i];
    }

    for (int i = 0; i < scopeLocalParameters.size(); i++)
    {
        if (scopeLocalParameters[i]->getName().equalsIgnoreCase(name))
            return scopeLocalParameters[i];
    }

    return nullptr;
}

float ScopeSync::getParameterHostValue(int hostIdx)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
        return hostParameters[hostIdx]->getHostValue();
    else
        return 0.0f;
}

void ScopeSync::getParameterNameForHost(int hostIdx, String& parameterName)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
    {
        String shortDesc;
        hostParameters[hostIdx]->getDescriptions(shortDesc, parameterName);
    }
    else
    {
        parameterName = "Dummy Param";
    }
}

void ScopeSync::getParameterText(int hostIdx, String& parameterText)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
    {
        hostParameters[hostIdx]->getUITextValue(parameterText);
    }
}

void ScopeSync::setParameterFromHost(int hostIdx, float newHostValue)
{
    if (isPositiveAndBelow(hostIdx, hostParameters.size()))
    {
        hostParameters[hostIdx]->setHostValue(newHostValue);
#ifdef __DLL_EFFECT__
        sendToScopeSyncAsync(*(hostParameters[hostIdx]));
#else
        sendToScopeSyncAudio(*(hostParameters[hostIdx]));
#endif // __DLL_EFFECT__
    }
}

void ScopeSync::setParameterFromGUI(BCMParameter& parameter, float newValue)
{
    parameter.setUIValue(newValue);
    
#ifdef __DLL_EFFECT__
    sendToScopeSyncAsync(parameter);
#else
    sendToScopeSyncAudio(parameter);
    pluginProcessor->updateListeners(parameter.getHostIdx(), parameter.getHostValue());
#endif // __DLL_EFFECT__
}

void ScopeSync::handleScopeSyncAsyncUpdate(Array<int>& asyncValues)
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.handleUpdate(asyncValues, initialiseScopeParameters);
    initialiseScopeParameters = false;
#else
    (void)asyncValues;
#endif // __DLL_EFFECT__
}

void ScopeSync::getSnapshot(Array<std::pair<int, int>>& snapshotSubset, int numElements)
{
#ifdef __DLL_EFFECT__
    scopeSyncAsync.getSnapshot(snapshotSubset, numElements);
#else
    (void)snapshotSubset;
    (void)numElements;
#endif // __DLL_EFFECT__
}

Value& ScopeSync::getSystemError()
{
    return systemError;
};

Value& ScopeSync::getSystemErrorDetails()
{
    return systemErrorDetails;
};

void ScopeSync::setSystemError(const String& errorText, const String& errorDetailsText)
{
    systemError        = errorText;
    systemErrorDetails = errorDetailsText;
}
    
XmlElement* ScopeSync::getSystemLookAndFeels()
{
    XmlDocument lookAndFeelsDocument(systemLookAndFeels);
    XmlElement* lookAndFeelsElement = lookAndFeelsDocument.getDocumentElement();
    return lookAndFeelsElement;
}

bool ScopeSync::hasConfigurationUpdate(String& fileName)
{
    const ScopedLock lock(configurationChanges.getLock());

    if (configurationChanges.size() > 0)
    {
        fileName = configurationChanges.getLast();
        configurationChanges.clear();
        return true;
    }
    else
    {
        return false;
    }
};

bool ScopeSync::processConfigurationChange()
{
    if (ScopeSyncApplication::inPluginContext())
        storeParameterValues(); 

    String newFileName;

    if (hasConfigurationUpdate(newFileName))
    {
        if (configurationManagerWindow != nullptr)
            hideConfigurationManager();
            
        if (configuration->replaceConfiguration(newFileName))
        {
            applyConfiguration();
            return true;
        }
        else
        {
            setSystemError(configuration->getLastError(), configuration->getLastErrorDetails());
            return false;
        }
    }
    else
    {
        return false;
    }
}

void ScopeSync::changeConfiguration(const String& fileName)
{
    configurationChanges.add(fileName);
    processConfigurationChange();
}

void ScopeSync::changeConfiguration(int uid)
{
    String fileName = UserSettings::getInstance()->getConfigurationFilePathFromUID(uid);

    if (fileName.isNotEmpty())
        configurationChanges.add(fileName);
}

void ScopeSync::unloadConfiguration()
{
    configuration->replaceConfiguration(String::empty);
    applyConfiguration();
}

void ScopeSync::reloadLayout()
{
    setGUIReload(true);
}

void ScopeSync::applyConfiguration()
{
    setGUIEnabled(false);
    endAllParameterChangeGestures();

    systemError        = String::empty;
    systemErrorDetails = String::empty;
    
    if (ScopeSyncApplication::inPluginContext())
        storeParameterValues();

    hostParameters.clear();
    scopeLocalParameters.clear();
    
    resetScopeCodeIndexes();

    // Firstly create the BCMParameter entries for each of the Host Parameters
    ValueTree hostParameterTree = configuration->getHostParameters();

    for (int i = 0; i < hostParameterTree.getNumChildren(); i++)
    {
        hostParameters.add(new BCMParameter(i, hostParameterTree.getChild(i), BCMParameter::hostParameter));
        
        int scopeSyncCode = hostParameters[i]->getScopeCode();
        // DBG("ScopeSync::applyConfiguration - Added host parameter: " + hostParameters[i]->getName() + ", ScopeSyncCode: " + String(scopeSyncCode));
        
        if (scopeSyncCode > -1 && scopeSyncCode < scopeSyncCodes.size())
            paramIdxByScopeSyncId.set(scopeSyncCode, i);
    }
    
    // Then do the same for each of the Scope Local Parameters
    ValueTree scopeLocalParameterTree = configuration->getScopeParameters();

    for (int i = 0; i < scopeLocalParameterTree.getNumChildren(); i++)
    {
        scopeLocalParameters.add(new BCMParameter(i, scopeLocalParameterTree.getChild(i), BCMParameter::scopeLocal));
        
        int scopeLocalCode = scopeLocalParameters[i]->getScopeCode() - ScopeSyncApplication::numScopeSyncParameters;
        // DBG("ScopeSync::applyConfiguration - Added scope local parameter: " + scopeLocalParameters[i]->getName() + ", ScopeLocalCode: " + String(scopeLocalCode));
        
        if (scopeLocalCode > -1 && scopeLocalCode < scopeLocalCodes.size())
            paramIdxByScopeLocalId.set(scopeLocalCode, i);
    }

#ifndef __DLL_EFFECT__
    pluginProcessor->updateHostDisplay();
#endif // __DLL_EFFECT__

    if (ScopeSyncApplication::inPluginContext())
        restoreParameterValues();
    else
        initialiseScopeParameters = true;

    UserSettings::getInstance()->updateConfigurationLibraryEntry(getConfigurationFile().getFullPathName(),
                                                                 getConfigurationFile().getFileName(),
                                                                 getConfigurationRoot());
    setGUIReload(true);
}

void ScopeSync::setGUIEnabled(bool shouldBeEnabled)
{
#ifndef __DLL_EFFECT__
    pluginProcessor->setGUIEnabled(shouldBeEnabled);
#else
    scopeFX->setGUIEnabled(shouldBeEnabled);
#endif // __DLL_EFFECT__
}

void ScopeSync::saveConfiguration()
{
    configuration->save(true, true);
}

void ScopeSync::saveConfigurationAs(const String& fileName)
{
    if (File::isAbsolutePath(fileName))
    {
        configuration->setLastFailedFile(configuration->getFile());
        configuration->setFile(File(fileName));
        saveConfiguration();
    }
}

void ScopeSync::reloadSavedConfiguration()
{
    configuration->loadFrom(configuration->getFile(), true);
    configuration->setMissingDefaultValues();
    applyConfiguration();
    undoManager.clearUndoHistory();
}

String ScopeSync::getConfigurationName(bool showUnsavedIndicator)
{
    String name = configuration->getDocumentTitle();

    if (showUnsavedIndicator && configuration->hasChangedSinceSaved())
        name += " *";

    return name;
}

bool ScopeSync::configurationHasUnsavedChanges()
{
    return configuration->hasChangedSinceSaved();
}

bool ScopeSync::configurationIsReadOnly()
{
    return getConfigurationRoot().getProperty(Ids::readOnly, false);
}

void ScopeSync::addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel) { bcmLookAndFeels.add(bcmLookAndFeel); }

BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id)
{
    // Try to find the LookAndFeel with a matching id
    for (int i = 0; i < bcmLookAndFeels.size(); i++)
    {
        String foundId = bcmLookAndFeels[i]->getId();
        
        if (foundId.equalsIgnoreCase(id))
            return bcmLookAndFeels[i];
    }

    return nullptr;
}

void ScopeSync::clearBCMLookAndFeels() { bcmLookAndFeels.clear(); }

int ScopeSync::getNumBCMLookAndFeels()
{
    return bcmLookAndFeels.size();
}

StringArray ScopeSync::getBCMLookAndFeelIds(const Identifier& componentType)
{
    StringArray specificList;
    StringArray genericList;
    
    for (int i = 0; i < getNumBCMLookAndFeels(); i++)
    {
        int res = bcmLookAndFeels[i]->appliesToComponentType(componentType);

        if (res == 1)
            specificList.add(bcmLookAndFeels[i]->getId());
        else if (res == 2)
            genericList.add(bcmLookAndFeels[i]->getId());
    }

    specificList.add(String::empty);
    specificList.addArray(genericList);

    return specificList;
}

void ScopeSync::storeParameterValues()
{
    int numHostParameters = hostParameters.size();
    
    if (numHostParameters > 0)
    {
        Array<float> currentParameterValues;
    
        for (int i = 0; i < numHostParameters; i++)
            currentParameterValues.set(i, getParameterHostValue(i));

        parameterValueStore = XmlElement("parametervalues");
        parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, numHostParameters)));

        //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
    }
    else
    {
        //DBG("ScopeSync::storeParameterValues - leaving storage alone, as we don't have any host parameters");
    }
}

void ScopeSync::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    //DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void ScopeSync::restoreParameterValues()
{
    Array<float> parameterValues;
    int numHostParameters = getNumParametersForHost();

    String floatCSV = parameterValueStore.getAllSubText();
    int numParametersToRead = jmin(numHostParameters, stringToFloatArray(floatCSV, parameterValues, numHostParameters));

    for (int i = 0; i < numParametersToRead; i++)
    {
        setParameterFromHost(i, parameterValues[i]);
    }

    //DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
}

const String ScopeSync::systemLookAndFeels =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<lookandfeels>\n"
"  <lookandfeel id=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <colours>\n"
"      <slider textboxbackgroundcolourid=\"ff000000\" textboxoutlinecolourid=\"ff121517\" textboxtextcolo"
"urid=\"ffb3bbbd\" textboxhighlightcolourid=\"ffa7aaae\" thumbcolourid=\"00ffffff\" backgroundcolourid=\"ff0"
"00000\"></slider>\n"
"      <label textcolourid=\"ff323739\"></label>\n"
"      <texteditor focusedoutlinecolourid=\"002f353a\"></texteditor>\n"
"      <tabbedcomponent outlinecolourid=\"00000000\"></tabbedcomponent>\n"
"      <tabbar tabtextcolourid=\"ffc3cace\" fronttextcolourid=\"ffff7f00\" taboutlinecolourid=\"00000000\">"
"</tabbar>\n"
"      <textbutton textcolouroffid=\"00ffffff\" textcolouronid=\"00ff0000\" buttoncolourid=\"ffff0000\"></t"
"extbutton>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:load_config_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"loadConfigButtonOff\" downfilename=\"loadConfigButtonOn\" mouseoverupfile"
"name=\"loadConfigButtonOver\" mouseoverdownfilename=\"loadConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:reload_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"reloadButtonOff\" downfilename=\"reloadButtonOn\" mouseoverupfilename=\"re"
"loadButtonOver\" mouseoverdownfilename=\"reloadButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:remove_config_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"removeConfigButtonOff\" downfilename=\"removeConfigButtonOn\" mouseoverup"
"filename=\"removeConfigButtonOver\" mouseoverdownfilename=\"removeConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:patch_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"patchWindowButtonOff\" downfilename=\"patchWindowButtonOn\" mouseoverupfi"
"lename=\"patchWindowButtonOver\" mouseoverdownfilename=\"patchWindowButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:presets_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"presetsButtonOff\" downfilename=\"presetsButtonOn\" mouseoverupfilename=\""
"presetsButtonOver\" mouseoverdownfilename=\"presetsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:settings_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"settingsButtonOff\" downfilename=\"settingsButtonOn\" mouseoverupfilename"
"=\"settingsButtonOver\" mouseoverdownfilename=\"settingsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:snapshot_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"snapshotOff\" downfilename=\"snapshotOn\" mouseoverupfilename=\"snapshotOv"
"er\" mouseoverdownfilename=\"snapshotOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:showconfigurationmanager_button\" parentid=\"system:default\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <images>\n"
"      <textbutton upfilename=\"showconfigurationmanagerOff\" downfilename=\"showconfigurationmanagerOn\""
" mouseoverupfilename=\"showconfigurationmanagerOver\" mouseoverdownfilename=\"showconfigurationmanagerO"
"n\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:configname\">\n"
"    <appliesto componenttype=\"none\" />\n"
"    <colours>\n"
"      <label textcolourid=\"ffa7aaae\" outlinecolourid=\"ffa7aaae\"></label>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"</lookandfeels>\n";
