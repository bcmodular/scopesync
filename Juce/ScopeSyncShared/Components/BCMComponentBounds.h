/*
  ==============================================================================

    BCMComponentBounds.h
    Created: 17 Aug 2014 9:54:39pm
    Author:  giles

  ==============================================================================
*/

#ifndef BCMCOMPONENTBOUNDS_H_INCLUDED
#define BCMCOMPONENTBOUNDS_H_INCLUDED
#include <JuceHeader.h>

class BCMComponentBounds
{
public:
    int                  width;
    int                  height;
    int                  x;
    int                  y;
    String               relativeRectangle;
    BorderSize<int>      borderSize; 
    Justification::Flags justificationFlags;
    bool                 onlyReduceInSize;
};

#endif  // BCMCOMPONENTBOUNDS_H_INCLUDED
