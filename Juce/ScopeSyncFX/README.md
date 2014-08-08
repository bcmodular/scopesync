ScopeSyncFX
===========
### Introduction.
This is the ScopeSync module that can be hosted within a Scope application (regular or SDK, from v4 onwards). It uses the ScopeFX library from Sonic|Core (hence the name). Note that this is Windows-only at the moment, as the Scope application itself only runs on Windows (XP onwards).

### Build information
There are builds in the /Builds directory for Visual Studio Express 2010 and Visual Studio Express 2013, but other build targets can be added in the Introjucer application using the ScopeSyncFX.jucer file if necessary. The DLL build is for 32-bit architecture, but this works fine with both 32- and 64-bit versions of Scope.

#### Dependencies
There is a dependency on the Juce modules being available. The builds assume that these will be found in ../../../JUCE/modules (relative to the .jucer file), so it's recommended that you clone the JUCE GitHub repository (https://github.com/julianstorer/JUCE) alongside the scopesync repository (and keep it regularly updated!)

Suggested directory layout:
```
<devroot>/github/JUCE/modules
<devroot>/github/scopesync/Juce
```

### Authors and Contributors
Main authors: @bcsharc, @wellis and @jssbrandt

### Support or Contact
For any questions, please find us at the [BCModular Forum](http://www.bcmodular.co.uk/forum/)
