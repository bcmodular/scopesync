ScopeSyncPlugin
===============
### Introduction.
These are the ScopeSync modules that can be hosted within a VST-compatible Host application.

### Build information
There are builds in the /Builds directory for Visual Studio Express 2010 and Visual Studio Express 2013, but other build targets can be added in the Introjucer application using the ScopeSyncFX.jucer file if necessary. There is a 32-bit Debug target for each and Release targets for 32- and 64-bit architectures.

#### Dependencies
There is a dependency on the Juce modules being available. The builds assume that these will be found in ../../../JUCE/modules (relative to the .jucer file), so it's recommended that you clone the JUCE GitHub repository (https://github.com/julianstorer/JUCE) alongside the scopesync repository (and keep it regularly updated!)

There is also a dependency on the VST SDK, which is included in the git repository

Suggested directory layout:
```
<devroot>/github/JUCE/modules
<devroot>/github/scopesync/Juce
```

#### Debugging
Juce's "audio plugin host" (found in JUCE/extras) is pretty good for debugging VSTs during the build cycle.

### Authors and Contributors
Main authors: @bcsharc, @wellis and @jssbrandt

### Support or Contact
For any questions, please find us at the [BCModular Forum](http://www.bcmodular.co.uk/forum/)
