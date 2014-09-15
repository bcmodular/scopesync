### Updating Resources
(windows-only for now)
#### Images
- Open a command prompt
- cd to wherever this directory is on your machine, e.g.
`cd "C:\development\github\scopesync\Juce\ScopeSyncShared\Resources"`
- Run the following command:
`BinaryBuilder.exe image . ImageResources *.png`
- Add/update content in the loadImageResources() method of https://github.com/bcmodular/scopesync/blob/master/Juce/ScopeSyncShared/Components/ImageLoader.cpp
