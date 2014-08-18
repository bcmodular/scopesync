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
#include "../Components/ImageLoader.h"
#include <utility>

#ifndef __DLL_EFFECT__
    #include "../../ScopeSyncPlugin/Source/PluginProcessor.h"
const ScopeSync::AppContext ScopeSync::appContext = plugin;
#else
    #include "../../ScopeSyncFX/Source/ScopeFX.h"
const ScopeSync::AppContext ScopeSync::appContext = scopefx;
#endif // __DLL_EFFECT__

const int ScopeSync::minHostParameters = 128;
const int ScopeSync::MIN_SCOPE_INTEGER = INT_MIN;
const int ScopeSync::MAX_SCOPE_INTEGER = INT_MAX;

const int ScopeSync::numScopeSyncParameters  = 128;
const int ScopeSync::numScopeLocalParameters = 16;

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

ScopeSync::ScopeSync() : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    initialise();
}

#ifndef __DLL_EFFECT__
ScopeSync::ScopeSync(PluginProcessor* owner) : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    configurationLoading = true;
    pluginProcessor = owner;
    initialise();
}
#else
ScopeSync::ScopeSync(ScopeFX* owner) : parameterValueStore("parametervalues"), configurationXml("configuration")
{
    configurationLoading = true;
    scopeFX = owner;
    initialise();
}
#endif // __DLL_EFFECT__

ScopeSync::~ScopeSync()
{
    ImageLoader::deleteInstance();
}

void ScopeSync::initialise()
{
    for (int i = 0; i < numScopeSyncParameters; i++)
        paramIdxByScopeSyncId.add(-1);

    for (int i = 0; i < numScopeLocalParameters; i++)
        paramIdxByScopeLocalId.add(-1);

    loadConfiguration(true, false, true);
    configurationFilePath.addListener(this);
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

void ScopeSync::beginParameterChangeGesture(BCMParameter& parameter)
{
#ifndef __DLL_EFFECT__
    if (pluginProcessor)
        pluginProcessor->beginParameterChangeGesture(parameter.getHostIdx()); 
#else
    parameter.setAffectedByUI(true);
#endif // __DLL_EFFECT__
}

void ScopeSync::endParameterChangeGesture(BCMParameter& parameter)
{
#ifndef __DLL_EFFECT__
    pluginProcessor->endParameterChangeGesture(parameter.getHostIdx()); 
#else
    parameter.setAffectedByUI(false);
#endif // __DLL_EFFECT__
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

            if (scopeCode < numScopeSyncParameters)
            {
                int paramIdx = paramIdxByScopeSyncId[scopeCode];
                
                if (paramIdx >= 0)
                    parameter = hostParameters[paramIdx];
            }
            else
            {
                int paramIdx = paramIdxByScopeLocalId[scopeCode - numScopeSyncParameters];
                
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

void ScopeSync::timerCallback()
{
    if (inPluginContext())
        receiveUpdatesFromScopeAudio();
    else
        receiveUpdatesFromScopeAsync();
}
    
void ScopeSync::valueChanged(Value& valueThatChanged)
{
    if (valueThatChanged.refersToSameSourceAs(configurationFilePath))
    {
        loadConfiguration(false, retainParameterState, true);
    }
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

Value& ScopeSync::getConfigurationName()
{
    return configurationName;
};

Value& ScopeSync::getConfigurationFilePath()
{
    return configurationFilePath;
};

Value& ScopeSync::getSystemError()
{
    return systemError;
};

void ScopeSync::setConfigurationFilePath(const String& newFilePath, bool retainState)
{ 
    configurationFilePath = newFilePath;
    retainParameterState  = retainState;
};

PropertiesFile& ScopeSync::getAppProperties()
{
    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.folderName          = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";
    appProperties.setStorageParameters(options);

    PropertiesFile* properties = appProperties.getUserSettings();

    return *properties;
}
    
bool ScopeSync::configurationIsLoading()
{
    const ScopedLock lock(flagLock);
    return configurationLoading;
}

void ScopeSync::setConfigurationLoading(bool configurationLoadingFlag)
{
    const ScopedLock lock(flagLock);
    configurationLoading = configurationLoadingFlag;
};

XmlElement* ScopeSync::getSystemLookAndFeels()
{
    XmlDocument lookAndFeelsDocument(systemLookAndFeels);
    XmlElement* lookAndFeelsElement = lookAndFeelsDocument.getDocumentElement();
    return lookAndFeelsElement;
}

bool ScopeSync::loadConfiguration(bool loadLoader, bool retainState, bool clearSystemErrorMessage)
{
    setConfigurationLoading(true);
    numParameters = 0;
    hostParameters.clear();
    scopeLocalParameters.clear();
    
    if (clearSystemErrorMessage)
        clearSystemError();

    if (!loadSystemParameterTypes())
    {
        DBG("Failed to load system parameter types");
        setConfigurationLoading(false);
        return false;
    }
    
    ScopedPointer<XmlElement> configElement;

    if (loadLoader)
    {
        XmlDocument configurationDocument(loaderConfiguration);
        configElement = configurationDocument.getDocumentElement();
    }
    else
    {
        File configurationFile(configurationFilePath.getValue());

        DBG("ScopeSync::loadConfiguration - Trying to load: " + configurationFile.getFullPathName());

        if (!(configurationFile.existsAsFile()))
        {
            setSystemError("Configuration file not found");
            setConfigurationLoading(false);
            loadConfiguration(true, false, false);
            return false;
        }
    
        XmlDocument configurationDocument(configurationFile);
        configElement = configurationDocument.getDocumentElement();
        
        if (configElement == nullptr)
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Configuration XML Parsing error: " + configurationDocument.getLastParseError());
            setConfigurationLoading(false);
            return false;
        }
    }
    
    configurationXml  = XmlElement(*configElement);
    configurationName = configurationXml.getStringAttribute("name", "ScopeSync");

    XmlElement* paramTypesXml = configurationXml.getChildByName("paramtypes");
    if (paramTypesXml)
        overrideParameterTypes(*paramTypesXml);
    
    XmlElement* deviceXml = configurationXml.getChildByName("device");
    if (deviceXml)
        loadDeviceParameters(*deviceXml);
    
    XmlElement* mappingXml = configurationXml.getChildByName("mapping");
    if (mappingXml && mappingXml->hasAttribute("filename"))
        loadMappingFile(*mappingXml);
    
    XmlElement* layoutXml = configurationXml.getChildByName("layout");
    if (layoutXml && layoutXml->hasAttribute("filename"))
        loadLayoutFile(*layoutXml);
    
#ifndef __DLL_EFFECT__
    pluginProcessor->updateHostDisplay();
#endif // __DLL_EFFECT__

    if (retainState && inPluginContext())
        restoreParameterValues();
    else
        initialiseScopeParameters = true;

    setGUIReload(true);
    setConfigurationLoading(false);
    return true;
}

void ScopeSync::addBCMLookAndFeel(BCMLookAndFeel* bcmLookAndFeel)
{
    bcmLookAndFeels.add(bcmLookAndFeel);
}

BCMLookAndFeel* ScopeSync::getBCMLookAndFeelById(String id)
{
    //DBG("ScopeSync::getBCMLookAndFeelById: Looking for LookAndFeel by id: id = " + id);

    // Set LookAndFeel to system:default, in case we don't find one that matches, or a layout default
    BCMLookAndFeel* bcmLookAndFeel = bcmLookAndFeels[0];
    
    bool foundLnF = false;

    // Try to find the LookAndFeel with a matching id
    for (int i = 1; i < bcmLookAndFeels.size(); i++)
    {
        String foundId = bcmLookAndFeels[i]->getId();
        
        if (foundId.equalsIgnoreCase(id))
        {
            bcmLookAndFeel = bcmLookAndFeels[i];
            foundLnF       = true;
            break;
        }
    }

    if (!foundLnF)
    {
        // We didn't find a matching one, so try to find the layout default instead
        for (int i = 1; i < bcmLookAndFeels.size(); i++)
        {
            String foundId = bcmLookAndFeels[i]->getId();
        
            if (foundId.equalsIgnoreCase("default"))
            {
                bcmLookAndFeel = bcmLookAndFeels[i];
                break;
            }
        }
    }

    return bcmLookAndFeel;
}

void ScopeSync::clearBCMLookAndFeels()
{
    DBG("ScopeSync::clearBCMLookAndFeels: array currently contains " + String(bcmLookAndFeels.size()) + " entries");
    bcmLookAndFeels.clear();
    DBG("ScopeSync::clearBCMLookAndFeels: array now contains " + String(bcmLookAndFeels.size()) + " entries");
}

int ScopeSync::getNumBCMLookAndFeels()
{
    return bcmLookAndFeels.size();
}

void ScopeSync::storeParameterValues()
{
    Array<float> currentParameterValues;

    for (int i = 0; i < numParameters; i++)
        currentParameterValues.set(i, getParameterHostValue(i));

    parameterValueStore = XmlElement("parametervalues");
    parameterValueStore.addTextElement(String(floatArrayToString(currentParameterValues, numParameters)));

    DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void ScopeSync::storeParameterValues(XmlElement& parameterValues)
{
    parameterValueStore = XmlElement(parameterValues);
    
    DBG("ScopeSync::storeParameterValues - Storing XML: " + parameterValueStore.createDocument(""));
}

void ScopeSync::restoreParameterValues()
{
    Array<float> parameterValues;

    String floatCSV = parameterValueStore.getAllSubText();
    int numParametersToRead = jmin(numParameters, stringToFloatArray(floatCSV, parameterValues, numParameters));

    for (int i = 0; i < numParametersToRead; i++)
    {
        setParameterFromHost(i, parameterValues[i]);
    }

    DBG("ScopeSync::restoreParameterValues - Restoring XML: " + parameterValueStore.createDocument(""));
}

bool ScopeSync::loadSystemParameterTypes()
{
    // Create the default parameter type
    ValueTree defaultParameterType(BCMParameter::paramTypeId);
    defaultParameterType.setProperty(BCMParameter::paramTypeNameId,             "default",                nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeUISuffixId,         "",                       nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeScopeRangeMinId,    0,                        nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeScopeRangeMaxId,    MAX_SCOPE_INTEGER,        nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeScopeRangeMinFltId, 0.0f,                     nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeScopeRangeMaxFltId, 1.0f,                     nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeUIRangeMinId,       0,                        nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeUIRangeMaxId,       100,                      nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeUIRangeIntervalId,  0.0001,                   nullptr);
    defaultParameterType.setProperty(BCMParameter::paramTypeValueTypeId,        BCMParameter::continuous, nullptr);

    XmlDocument               parameterTypesDocument(systemParameterTypes);
    ScopedPointer<XmlElement> parameterTypesXml = parameterTypesDocument.getDocumentElement();
    
    // Initialise the parameterTypes Tree and add the default entry
    parameterTypes = ValueTree(BCMParameter::paramTypesId);
    parameterTypes.addChild(defaultParameterType, 0, nullptr);

    if (parameterTypesXml != nullptr)
    {
        forEachXmlChildElementWithTagName(*parameterTypesXml, child, "parametertype")
        {
            ValueTree parameterType = defaultParameterType.createCopy();

            getParameterTypeFromXML(*child, parameterType);

            DBG("ScopeSync::loadSystemParameterTypes - Adding Parameter Type:" + newLine + parameterType.toXmlString());
            parameterTypes.addChild(parameterType, -1, nullptr);
        }
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading System Parameter Types: " + parameterTypesDocument.getLastParseError());
        return false;
    }

    return true;
}

bool ScopeSync::overrideParameterTypes(XmlElement& parameterTypesXml)
{
    if (parameterTypesXml.hasAttribute("filename"))
    {
        File configurationFile(configurationFilePath.getValue());
        File configurationFileDir = configurationFile.getParentDirectory();

        File parameterTypesFile = configurationFileDir.getChildFile(parameterTypesXml.getStringAttribute("filename"));
        
        DBG("ScopeSync::overrideParameterTypes - Trying to load: " + parameterTypesFile.getFullPathName());

        XmlDocument               parameterTypesDocument(parameterTypesFile);
        ScopedPointer<XmlElement> loadedParameterTypesXML = parameterTypesDocument.getDocumentElement();

        if (loadedParameterTypesXML != nullptr)
        {
            parameterTypesXml = *loadedParameterTypesXML;
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Failed to load Configuration's Parameter Types. Parsing error: " + parameterTypesDocument.getLastParseError());
            return false;
        }
    }

    forEachXmlChildElementWithTagName(parameterTypesXml, child, "parametertype")
    {
        // Initialise a parameter type value tree, in case this is a new one
        ValueTree parameterType = parameterTypes.getChild(0).createCopy();
        bool      newParameterType = true;
        
        String parameterTypeName = child->getStringAttribute(BCMParameter::paramTypeNameId);

        for (int i = 0; i < parameterTypes.getNumChildren(); i++)
        {
            if (parameterTypes.getChild(i).getProperty(BCMParameter::paramTypeNameId) == parameterTypeName)
            {
                parameterType    = parameterTypes.getChild(i);
                newParameterType = false;
                break;
            }
        }
        
        getParameterTypeFromXML(*child, parameterType);

        if (newParameterType) parameterTypes.addChild(parameterType, -1, nullptr);
    }
    return true;
}

void ScopeSync::getParameterTypeFromXML(XmlElement& xml, ValueTree& parameterType)
{
    forEachXmlChildElement(xml, child)
    {
        Identifier xmlId = child->getTagName();

        if (xmlId == BCMParameter::paramTypeNameId || xmlId == BCMParameter::paramTypeUISuffixId)
        {
            parameterType.setProperty(xmlId, child->getAllSubText(), nullptr);
        }
        else if (xmlId == BCMParameter::paramTypeScopeRangeMinId || xmlId == BCMParameter::paramTypeScopeRangeMaxId)
        {
            int intValue = 0;

            if (child->getAllSubText().equalsIgnoreCase("MIN_INT"))
                intValue = MIN_SCOPE_INTEGER;
            else if (child->getAllSubText().equalsIgnoreCase("MAX_INT"))
                intValue = MAX_SCOPE_INTEGER;
            else
                intValue = child->getAllSubText().getIntValue();

            parameterType.setProperty(xmlId, intValue, nullptr);

            float fltValue = 0.0f;

            if (intValue >= 0)
                fltValue = (float)intValue / (float)MAX_SCOPE_INTEGER;
            else
                fltValue = -((float)intValue / (float)MIN_SCOPE_INTEGER);  
            
            if (xmlId == BCMParameter::paramTypeScopeRangeMinId)
                parameterType.setProperty(BCMParameter::paramTypeScopeRangeMinFltId, fltValue, nullptr);
            else
                parameterType.setProperty(BCMParameter::paramTypeScopeRangeMaxFltId, fltValue, nullptr);
        }
        else if
            (
                xmlId == BCMParameter::paramTypeUIRangeMinId
             || xmlId == BCMParameter::paramTypeUIRangeMaxId
             || xmlId == BCMParameter::paramTypeUIRangeIntervalId
             || xmlId == BCMParameter::paramTypeUIResetValueId
            )
        {
            float fltValue = child->getAllSubText().getFloatValue();
            parameterType.setProperty(xmlId, fltValue, nullptr);
        }

    }

    XmlElement* skewFactor = xml.getChildByName(BCMParameter::paramTypeUISkewFactorId);
    
    if (skewFactor != nullptr)
    {
        float uiMinValue = parameterType.getProperty(BCMParameter::paramTypeUIRangeMinId);
        float uiMaxValue = parameterType.getProperty(BCMParameter::paramTypeUIRangeMaxId);
        readUISkewFactorXml(*skewFactor, parameterType, uiMinValue, uiMaxValue);
    }
     
    XmlElement* paramSettings = xml.getChildByName(BCMParameter::paramTypeSettingsId);

    if (paramSettings != nullptr)
    {
        // Remove any existing settings, in case we're overriding from the main parameter type
        parameterType.removeChild(parameterType.getChildWithName(BCMParameter::paramTypeSettingsId), nullptr);
                
        ValueTree parameterSettings(BCMParameter::paramTypeSettingsId);

        int numSettings = 0;

        // Need to find out how many settings we have, so we can calculate the
        // automatic values below
        forEachXmlChildElementWithTagName(*paramSettings, paramSettingXml, BCMParameter::paramTypeSettingId)
        {
            numSettings++;
        }

        if (numSettings > 0)
        {
            float uiRangeMax = (float)numSettings - 1.0f;

            parameterType.setProperty(BCMParameter::paramTypeUIRangeMinId, 0.0f, nullptr);
            parameterType.setProperty(BCMParameter::paramTypeUIRangeMaxId, uiRangeMax, nullptr);

            int settingCounter = 0;

            parameterType.setProperty(BCMParameter::paramTypeValueTypeId, BCMParameter::discrete, nullptr);

            forEachXmlChildElementWithTagName(*paramSettings, paramSettingXml, BCMParameter::paramTypeSettingId)
            {
                ValueTree parameterSetting(BCMParameter::paramTypeSettingId);

                String    settingName = paramSettingXml->getAllSubText();
                parameterSetting.setProperty(BCMParameter::paramTypeSettingNameId, settingName, nullptr);

                float autoSettingValue = (float)scaleDouble(0, numSettings - 1, 0, 1, settingCounter);
                float settingValue = (float)(paramSettingXml->getDoubleAttribute(BCMParameter::paramTypeSettingValueId, autoSettingValue));
                parameterSetting.setProperty(BCMParameter::paramTypeSettingValueId, settingValue, nullptr);

                parameterSettings.addChild(parameterSetting, -1, nullptr);
                settingCounter++;
            }

            parameterType.setProperty(BCMParameter::paramTypeUIRangeMinId, 0, nullptr);
            parameterType.setProperty(BCMParameter::paramTypeUIRangeMaxId, settingCounter - 1, nullptr);

            parameterType.setProperty(BCMParameter::paramTypeValueTypeId, BCMParameter::discrete, nullptr);
            parameterType.addChild(parameterSettings, -1, nullptr);
        }
        else
        {
            // In this case, i.e. we have an empty "settings" element, set the parameter type
            // as being "continuous" instead of "discrete"
            parameterType.setProperty(BCMParameter::paramTypeValueTypeId, BCMParameter::continuous, nullptr);
        }
    }
}

void ScopeSync::readUISkewFactorXml(const XmlElement& xml, ValueTree& parameterType, double uiMinValue, double uiMaxValue)
{
    String skewFactorType = xml.getStringAttribute("type", "standard");
    double skewFactor = 1.0f;
    
    String skewFactorText = xml.getAllSubText();

    if (skewFactorText.isNotEmpty())
        skewFactor = skewFactorText.getDoubleValue();
    
    if (skewFactorType == "frommidpoint")
        skewFactor = log(0.5) / log((skewFactor - uiMinValue) / (uiMaxValue - uiMinValue));
    
    parameterType.setProperty(BCMParameter::paramTypeUISkewFactorId, skewFactor, nullptr);
    parameterType.setProperty(BCMParameter::paramTypeSkewUIOnlyId, xml.getBoolAttribute("uionly", false), nullptr);
}

bool ScopeSync::loadDeviceParameters(XmlElement& deviceXml)
{
    if (deviceXml.hasAttribute("filename"))
    {
        File configurationFile(configurationFilePath.getValue());
        File configurationFileDir = configurationFile.getParentDirectory();

        File deviceFile = configurationFileDir.getChildFile(deviceXml.getStringAttribute("filename"));
        
        DBG("ScopeSync::loadDeviceParameters - Trying to load: " + deviceFile.getFullPathName());

        XmlDocument               deviceDocument(deviceFile);
        ScopedPointer<XmlElement> loadedDeviceXML = deviceDocument.getDocumentElement();

        if (loadedDeviceXML != nullptr)
        {
            deviceXml = *loadedDeviceXML;
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading Configuration's Device File: " + deviceDocument.getLastParseError());
            return false;
        }
    }

    deviceParameters = ValueTree(BCMParameter::deviceId);
    numParameters = 0;
    
    forEachXmlChildElementWithTagName(deviceXml, child, "parameter")
    {
        ValueTree parameter     = ValueTree(BCMParameter::paramId);
        bool scopeLocalParameter = false;
        
        parameter.setProperty(BCMParameter::paramNameId, child->getStringAttribute(BCMParameter::paramNameId, "__NO_NAME__"), nullptr);
        
        // In case we don't find a match or no parameter type is supplied in the XML,
        // initialise to the default parameter type
        ValueTree parameterType = parameterTypes.getChild(0).createCopy();
        
        int scopeSyncCode  = -1;
        int scopeLocalCode = -1;

        forEachXmlChildElement(*child, subChild)
        {
            Identifier xmlId = subChild->getTagName();

            if (xmlId == BCMParameter::paramShortDescId || xmlId == BCMParameter::paramFullDescId)
            {
                parameter.setProperty(xmlId, subChild->getAllSubText(), nullptr);
            }
            else if (xmlId == BCMParameter::paramScopeSyncId)
            {
                // Ignore ScopeSync value if we already have a ScopeLocal value
                if (scopeLocalParameter)
                    continue;
                else
                {
                    scopeSyncCode = scopeSyncCodes.indexOf(subChild->getAllSubText());
                
                    if (scopeSyncCode != -1)
                        parameter.setProperty(xmlId, scopeSyncCode, nullptr);
                }
            }
            else if (xmlId == BCMParameter::paramScopeLocalId)
            {
                scopeLocalCode = scopeLocalCodes.indexOf(subChild->getAllSubText());
                
                if (scopeLocalCode != -1 && inScopeFXContext())
                {
                    scopeLocalParameter = true;
                    parameter.setProperty(xmlId, scopeLocalCode, nullptr);
                    // Get rid of any ScopeSync value that may have been set
                    parameter.removeProperty(BCMParameter::paramScopeSyncId, nullptr);
                }   
                else
                {
                    DBG("ScopeSync::loadDeviceParameters - Invalid ScopeLocal code or not in scopefx context, so ignoring parameter");
                    scopeLocalParameter = true;
                    break;
                }
            }
            else if (xmlId == BCMParameter::paramTypeId)
            {
                String parameterTypeName = subChild->getStringAttribute(BCMParameter::paramTypeNameId);

                for (int i = 0; i < parameterTypes.getNumChildren(); i++)
                {
                    if (parameterTypes.getChild(i).getProperty(BCMParameter::paramTypeNameId) == parameterTypeName)
                    {
                        parameterType = parameterTypes.getChild(i).createCopy();
                        break;
                    }
                }

                // Apply any overrides
                getParameterTypeFromXML(*subChild, parameterType);
            }
        }
        
        parameter.addChild(parameterType, -1, nullptr);
        
        if (scopeLocalParameter)
        {
            if (!inPluginContext())
            {
                int paramIdx = scopeLocalParameters.size();
                scopeLocalParameters.add(new BCMParameter(paramIdx, parameter));
                
                DBG("ScopeSync::loadDeviceParameters - setting Parameter Index - scopeLocalCode: " + String(scopeLocalCode) + ", paramIdx: " + String(paramIdx));
                paramIdxByScopeLocalId.set(scopeLocalCode, paramIdx);
            }
            else
            {
                // Ignore ScopeLocal parameters if in a plugin context
                continue;
            }   
        }
        else
        {
            int paramIdx = hostParameters.size();
            hostParameters.add(new BCMParameter(paramIdx, parameter));
            DBG("ScopeSync::loadDeviceParameters - setting Parameter Index - scopeSyncCode: " + String(scopeSyncCode) + ", paramIdx: " + String(paramIdx));
            paramIdxByScopeSyncId.set(scopeSyncCode, paramIdx);
        }

        numParameters++;
        
        deviceParameters.addChild(parameter, -1, nullptr);      
    }

    //DeviceParameterComparator comparator;
    //deviceParameters.sort(comparator, nullptr, true);
    
    DBG("ScopeSync::loadDeviceParameters - Param definition");
    DBG("==================================================");
    for (int i = 0; i < deviceParameters.getNumChildren(); i++)
    {
        ValueTree param = deviceParameters.getChild(i);
        String paramDefinition = param.toXmlString();
        DBG(paramDefinition);
    }
    DBG("==================================================");
    
    return true;
}

bool ScopeSync::loadMappingFile(XmlElement& mappingXml)
{
    File configurationFile(configurationFilePath.getValue());
    File configurationFileDir = configurationFile.getParentDirectory();

    File mappingFile = configurationFileDir.getChildFile(mappingXml.getStringAttribute("filename"));
        
    DBG("ScopeSync::loadMappingFile - Trying to load: " + mappingFile.getFullPathName());

    XmlDocument               mappingDocument(mappingFile);
    ScopedPointer<XmlElement> loadedMappingXml = mappingDocument.getDocumentElement();

    if (loadedMappingXml != nullptr)
    {
        mappingXml = *loadedMappingXml;
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"Error","Problem reading Configuration's Mapping File: " + mappingDocument.getLastParseError());
        return false;
    }

    return true;
}

bool ScopeSync::loadLayoutFile(XmlElement& layoutXml)
{
    bool layoutLoaded = false;
    File configurationFile(configurationFilePath.getValue());
    File configurationFileDir = configurationFile.getParentDirectory();

    File layoutFile = configurationFileDir.getChildFile(layoutXml.getStringAttribute("filename"));
        
    DBG("ScopeSync::loadLayoutFile - Trying to load: " + layoutFile.getFullPathName());

    XmlDocument               layoutDocument(layoutFile);
    ScopedPointer<XmlElement> loadedLayoutXml = layoutDocument.getDocumentElement();

    if (loadedLayoutXml != nullptr)
    {
        if (loadedLayoutXml->hasTagName("layout"))
        {
            // No XSD validation header
            layoutXml     = *loadedLayoutXml;
            layoutLoaded = true;
        }
        else
        {
            // Look for a layout element at the 2nd level down instead
            XmlElement* subXml = loadedLayoutXml->getChildByName("layout");
            
            if (subXml != nullptr)
            {
                layoutXml = *subXml;
                layoutLoaded = true;
            }
        }
    }
    
    if (!layoutLoaded)
    {
        setSystemError("Problem reading Configuration's Layout File: " + layoutDocument.getLastParseError());
        DBG("Problem reading Configuration's Layout File: " + layoutDocument.getLastParseError());
        ScopedPointer<XmlElement> configElement;

        XmlDocument configurationDocument(loaderConfiguration);
        configElement = configurationDocument.getDocumentElement();

        layoutXml = *(configElement->getChildByName("layout"));
    }

    return layoutLoaded;
}

const String ScopeSync::systemLookAndFeels =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<lookandfeels>\n"
"  <lookandfeel id=\"system:default\">\n"
"    <colours>\n"
"      <textbutton textcolouroffid=\"00ffffff\" textcolouronid=\"00ff0000\" buttoncolourid=\"00ff0000\"></t"
"extbutton>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:snapshot_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"snapshotOff\" downfilename=\"snapshotOn\" mouseoverupfilename=\"snapshotOf"
"f\" mouseoverdownfilename=\"snapshotOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:settings_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"settingsButtonOff\" downfilename=\"settingsButtonOn\" mouseoverupfilename"
"=\"settingsButtonOff\" mouseoverdownfilename=\"settingsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:load_config_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"loadConfigButtonOff\" downfilename=\"loadConfigButtonOn\" mouseoverupfile"
"name=\"loadConfigButtonOff\" mouseoverdownfilename=\"loadConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:reload_config_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"reloadConfigButtonOff\" downfilename=\"reloadConfigButtonOn\" mouseoverup"
"filename=\"reloadConfigButtonOff\" mouseoverdownfilename=\"reloadConfigButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:patch_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"patchWindowButtonOff\" downfilename=\"patchWindowButtonOn\" mouseoverupfi"
"lename=\"patchWindowButtonOff\" mouseoverdownfilename=\"patchWindowButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:presets_button\" parentid=\"system:default\">\n"
"    <images>\n"
"      <textbutton upfilename=\"presetsButtonOff\" downfilename=\"presetsButtonOn\" mouseoverupfilename=\""
"presetsButtonOff\" mouseoverdownfilename=\"presetsButtonOn\"></textbutton>\n"
"    </images>\n"
"  </lookandfeel>\n"
"  <lookandfeel id=\"system:systemerror\">\n"
"    <colours>\n"
"      <label textcolourid=\"fffe9ddb\"></label>\n"
"    </colours>\n"
"  </lookandfeel>\n"
"</lookandfeels>";

const String ScopeSync::systemParameterTypes =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<parametertypes>\n"
"    <parametertype>\n"
"        <name>0-100%</name>\n"
"        <uisuffix> %</uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>100</uirangemax>\n"
"        <uirangeinterval>0.001</uirangeinterval>\n"
"        <uiresetvalue>45</uiresetvalue>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>Freq (Hz)</name>\n"
"        <uisuffix> Hz</uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>24000</uirangemax>\n"
"        <uirangeinterval>0.01</uirangeinterval>\n"
"        <uiskewfactor type=\"standard\">2</uiskewfactor>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>Env Time (s)</name>\n"
"        <uisuffix></uisuffix>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <uirangemin>0</uirangemin>\n"
"        <uirangemax>2147483647</uirangemax>\n"
"        <uirangeinterval>1</uirangeinterval>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>MultimodeOsc Wave</name>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <settings>\n"
"            <setting>Sine</setting>\n"
"            <setting>Tri</setting>\n"
"            <setting>Saw Up</setting>\n"
"            <setting>Saw Dn</setting>\n"
"            <setting>Square</setting>\n"
"            <setting>Input</setting>\n"
"        </settings>\n"
"    </parametertype>\n"
"    <parametertype>\n"
"        <name>ON/OFF Toggle</name>\n"
"        <scoperangemin>0</scoperangemin>\n"
"        <scoperangemax>MAX_INT</scoperangemax>\n"
"        <settings>\n"
"            <setting>OFF</setting>\n"
"            <setting>ON</setting>\n"
"        </settings>\n"
"    </parametertype>\n"
"</parametertypes>";

const String ScopeSync::loaderConfiguration =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<configuration>\n"
"  <device>\n"
"    <parameter name=\"PARAM132\">\n"
"      <shortdescription>Param 132</shortdescription>\n"
"      <fulldescription>Load New Configuration</fulldescription>\n"
"      <parametertype name=\"ON/OFF Toggle\" />\n"
"      <scopelocal>Z8</scopelocal>\n"
"    </parameter>\n"
"  </device>\n"
"  <mapping>\n"
"    <textbutton name=\"B69\">\n"
"      <parameter name=\"PARAM132\" settingdown=\"ON\" settingup=\"OFF\" type=\"notoggle\" />\n"
"    </textbutton>\n"
"  </mapping>\n"
"  <layout>\n"
"    <component backgroundcolour=\"ff2b2e30\">\n"
"      <bounds x=\"0\" y=\"0\" width=\"600\" height=\"40\"></bounds>\n"
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"host\" name=\"chooseconfiguration\" "
"tooltip=\"Load New Configuration\">\n"
"        <bounds x=\"170\" y=\"10\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <textbutton lfid=\"system:load_config_button\" displaycontext=\"scope\" name=\"B69\" tooltip=\"Load N"
"ew Configuration\">\n"
"        <bounds x=\"170\" y=\"10\" width=\"21\" height=\"21\"></bounds>\n"
"      </textbutton>\n"
"      <component backgroundcolour=\"00000000\" backgroundimage=\"scopeSyncLogo\">\n"
"        <bounds x=\"10\" y=\"12\" width=\"151\" height=\"16\"></bounds>\n"
"      </component>\n"
"      <label lfid=\"system:systemerror\" name=\"SystemError\" text=\"\">\n"
"        <justification right=\"true\" />\n"
"        <bounds x=\"200\" y=\"10\" width=\"400\" height=\"20\"></bounds>\n"
"      </label>\n"
"    </component>\n"
"  </layout>\n"
"</configuration>";
