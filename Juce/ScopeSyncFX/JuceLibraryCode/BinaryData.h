/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#ifndef BINARYDATA_H_38268870_INCLUDED
#define BINARYDATA_H_38268870_INCLUDED

namespace BinaryData
{
    extern const char*   empty_configuration;
    const int            empty_configurationSize = 216;

    extern const char*   loader_layout;
    const int            loader_layoutSize = 511;

    extern const char*   standardFooterContent_layout;
    const int            standardFooterContent_layoutSize = 791;

    extern const char*   standardHeaderContent_layout;
    const int            standardHeaderContent_layoutSize = 4823;

    extern const char*   standardSliderLnFs_layout;
    const int            standardSliderLnFs_layoutSize = 22039;

    extern const char*   system_lookandfeels;
    const int            system_lookandfeelsSize = 6264;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 6;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}

#endif
