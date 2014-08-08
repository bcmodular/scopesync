ScopeSyncPlugin
===============
### Introduction.
This is the ScopeSync module that can be hosted within a VST-compatible Host application.

### Build information
There are builds in the /Builds directory for Visual Studio Express 2010 and Visual Studio Express 2013, but other build targets can be added in the Introjucer application using the ScopeSyncFX.jucer file if necessary. There is a 32-bit Debug target for each and Release targets for 32- and 64-bit architectures.

#### Dependencies
There is a dependency on the Juce modules being available. The builds assume that these will be found in ../../../JUCE/modules (relative to the .jucer file), so it's recommended that you clone the JUCE GitHub repository alongside the scopesync repository.

There is also a dependency on the VST SDK and the build assumes they live in ../../../../SDKs/ (see suggested layout below. VST3 support is work in progress, but can be switched on in the .jucer configuration.

Suggested directory layout:
<devroot>/github/JUCE/modules
<devroot>/github/scopesync/Juce
<devroot>/SDKs/VST3SDK
<devroot>/SDKs/vstsdk2.4

#### Debugging
Juce's "audio plugin host" (found in JUCE/extras) is pretty good for debugging VSTs during the build cycle. If you do use it, remember to build it with ASIO support* if you want to test communications with Scope. This will require the ASIO libraries to be available to the build. A recommended location for these is:
<devroot>/SDKs/ASIOSDK2.3

* ASIO support is added using the following steps in the Introjucer:
- In the Config Tab, select "juce_audio_devices" under "Modules" and switch JUCE_ASIO to "Enabled"
- Add ../../../../../../SDKs/ASIOSDK2.3/common to the Header search paths for each of the relevant Debug/Release configurations

Also note that the standard .jucer for the Plugin Host assumes absolute paths to the VST SDKs, so you probably want to change the VST Folder and VST3 Folder settings for your chosen Export Target (e.g. Visual Studio 2013) as follows:
../../../../SDKs/vstsdk2.4
../../../../SDKs/VST3 SDK

### Authors and Contributors
Main authors: @bcsharc, @wellis and @jssbrandt

### Support or Contact
For any questions, please find us at the [BCModular Forum](http://www.bcmodular.co.uk/forum/)